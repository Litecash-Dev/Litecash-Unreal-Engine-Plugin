// Fill out your copyright notice in the Description page of Project Settings.


#include "LitecashGameInstance.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
FString defaultWalletAPIURL = "http://127.0.0.1:9999/api/wallet";
FProcHandle nodeProcess, exProcess, walProcess, wapiProcess;
void* nodeReadPipe;
void* nodeWritePipe;
void* exReadPipe;
void* exWritePipe;
void* walReadPipe; 
void* walWritePipe; 
void* wapiReadPipe;
void* wapiWritePipe;

void ULitecashGameInstance::GetRequest(FString URL, FHttpRequestResponse Callback)
{
	auto Http = &FHttpModule::Get();
	
	FHttpRequestPtr Request = Http->CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			Callback.ExecuteIfBound(Response->GetContentAsString(), Response->GetResponseCode(), bWasSuccessful);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::PostRequest(FString URL, FString Data, FHttpRequestResponse Callback)
{
	auto Http = &FHttpModule::Get();

	FHttpRequestPtr Request = Http->CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetContentAsString(Data);
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			Callback.ExecuteIfBound(Response->GetContentAsString(), Response->GetResponseCode(), bWasSuccessful);
		});
	Request->ProcessRequest();
}

FHttpRequestPtr WalletAPIReq()
{
	auto Http = &FHttpModule::Get();
	FString valueReceived = defaultWalletAPIURL;
	GConfig->GetString(TEXT("Litecash"), TEXT("WalletAPIURL"), valueReceived, GGameIni);
	FHttpRequestPtr Request = Http->CreateRequest();
	Request->SetURL(valueReceived);
	Request->SetVerb("POST");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");
	return Request;
}

void ULitecashGameInstance::APIWalletStatus(FWalletAPIStatusResponse Callback)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"wallet_status\"}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FWalletStatus wstatus;
			TSharedPtr<FJsonObject> obj;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				obj = ResultJson->Values.Find("result")->Get()->AsObject();
				wstatus.Available = obj->GetIntegerField("available");
				wstatus.Receiving = obj->GetIntegerField("receiving");
				wstatus.Sending = obj->GetIntegerField("sending");
				wstatus.Height = obj->GetIntegerField("current_height");
				wstatus.Maturing = obj->GetIntegerField("available");
				wstatus.Difficulty = obj->GetNumberField("difficulty");
				wstatus.CurrentStateHash = obj->GetStringField("current_state_hash");
				wstatus.PreviousStateHash = obj->GetStringField("previous_state_hash");
			}
			Callback.ExecuteIfBound(wstatus);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::APITxList(FWalletAPITxListResponse Callback)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"tx_list\"}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			TArray<FTx> txlist;
			TSharedPtr<FJsonObject> obj;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				for (int i = 0; i < ResultJson->Values.Find("result")->Get()->AsArray().Num(); i++)
				{
					obj = ResultJson->Values.Find("result")->Get()->AsArray()[i]->AsObject();
					FTx tx;
					tx.Comment = obj->GetStringField("comment");
					tx.Confirmations = obj->GetIntegerField("confirmations");
					tx.CreateTime = obj->GetIntegerField("create_time");
					tx.Fee = obj->GetIntegerField("fee");
					tx.Height = obj->GetIntegerField("height");
					tx.Income = obj->GetBoolField("income");
					tx.Kernel = obj->GetStringField("kernel");
					tx.Receiver = obj->GetStringField("receiver");
					tx.Sender = obj->GetStringField("sender");
					tx.Status = obj->GetIntegerField("status");
					tx.StatusString = obj->GetStringField("status_string");
					tx.TxId = obj->GetStringField("txId");
					tx.Value = obj->GetIntegerField("value");
					txlist.Add(tx);
				}
			}
			Callback.ExecuteIfBound(txlist);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::APIAddressList(FWalletAPIAddressListResponse Callback,bool OwnAddresses)
{
	FHttpRequestPtr Request = WalletAPIReq();
	FString own = OwnAddresses ? "true" : "false";
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"addr_list\",\"params\":{\"own\":"+ own +"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			TArray<FAddress> addlist;
			TSharedPtr<FJsonObject> obj;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				for (int i = 0; i < ResultJson->Values.Find("result")->Get()->AsArray().Num(); i++)
				{
					obj = ResultJson->Values.Find("result")->Get()->AsArray()[i]->AsObject();
					FAddress addr;
					addr.Address = obj->GetStringField("address");
					addr.Category = obj->GetStringField("category");
					addr.Comment = obj->GetStringField("comment");
					addr.CreateTime = obj->GetIntegerField("create_time");
					addr.Duration = obj->GetIntegerField("duration");
					addr.Expired = obj->GetBoolField("expired");
					addr.Own = obj->GetBoolField("own");
					addlist.Add(addr);
				}
			}
			Callback.ExecuteIfBound(addlist);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::APIUTXOList(FWalletAPIUTXOResponse Callback)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"get_utxo\"}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			TArray<FUTXO> utxolist;
			TSharedPtr<FJsonObject> obj;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				for (int i = 0; i < ResultJson->Values.Find("result")->Get()->AsArray().Num(); i++)
				{
					obj = ResultJson->Values.Find("result")->Get()->AsArray()[i]->AsObject();
					FUTXO utxo;
					utxo.Amount = obj->GetIntegerField("amount");
					utxo.CreateTxId = obj->GetStringField("createTxId");
					utxo.Id = obj->GetStringField("id");
					utxo.Maturity = obj->GetIntegerField("maturity");
					utxo.Session = obj->GetIntegerField("session");
					utxo.SpentTxId = obj->GetStringField("spentTxId");
					utxo.Status = obj->GetIntegerField("status");
					utxo.StatusString = obj->GetStringField("status_string");
					utxo.Type = obj->GetStringField("type");
					utxolist.Add(utxo);
				}
			}
			Callback.ExecuteIfBound(utxolist);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::TxCancel(FTxResponse Callback, FString TxId)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"tx_cancel\",\"params\":{\"txId\":\""+TxId+"\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString obj = "";
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error")->Get())
				{
					obj = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else
				{
					obj = Response->GetContentAsString();
					success = true;
				}
			}
			Callback.ExecuteIfBound(obj,success);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::TxStatus(FTxStatusResponse Callback, FString TxId)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"tx_status\",\"params\":{\"txId\":\"" + TxId + "\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FTxStatus tx;
			TSharedPtr<FJsonObject> obj;
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error"))
				{
					tx.FailureReason = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else if(ResultJson->Values.Find("result"))
				{
					obj = ResultJson->Values.Find("result")->Get()->AsObject();
					tx.Comment = obj->GetStringField("comment");
					tx.CreateTime = obj->GetIntegerField("create_time");
					tx.Fee = obj->GetIntegerField("fee");
					tx.Income = obj->GetBoolField("income");
					tx.FailureReason = obj->GetStringField("failure_reason");
					tx.Receiver = obj->GetStringField("receiver");
					tx.Sender = obj->GetStringField("sender");
					tx.Status = obj->GetIntegerField("status");
					tx.StatusString = obj->GetStringField("status_string");
					tx.TxId = obj->GetStringField("txId");
					tx.Value = obj->GetIntegerField("value");
					success = true;
				}
			}
			Callback.ExecuteIfBound(tx, success);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::TxSplit(FTxSplitResponse Callback, const TArray<FString>& Coins, int Fee)
{
	FHttpRequestPtr Request = WalletAPIReq();
	FString coins = "";
	for (int i =0;i<Coins.Num();i++)
	{
		coins += "\"" + Coins[i] + "\"";
		if (i < Coins.Num() - 1)
		{
			coins += ",";
		}
	}
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"tx_split\",\"params\":{\"coins\":{" + coins + "},\"fee\":"+FString::FromInt(Fee)+"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString obj;
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error"))
				{
					obj = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else if (ResultJson->Values.Find("result"))
				{
					obj = ResultJson->Values.Find("result")->Get()->AsString();
					success = true;
				}
			}
			Callback.ExecuteIfBound(obj, success);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::TxSend(FTxSendResponse Callback, int Value, int Fee, FString From, FString To, FString Comment)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"tx_send\",\"params\":{\"value\":" + FString::FromInt(Value) + ",\"fee\":" + FString::FromInt(Fee) + ",\"from\":\"" + From + "\",\"address\":\"" + To + "\",\"comment\":\"" + Comment + "\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString obj;
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error"))
				{
					obj = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else if (ResultJson->Values.Find("result"))
				{
					obj = ResultJson->Values.Find("result")->Get()->AsString();
					success = true;
				}
			}
			Callback.ExecuteIfBound(obj, success);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::DeleteAddress(FDeleteAddressResponse Callback, FString Address)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"delete_address\",\"params\":{\"address\":\"" + Address + "\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString obj = "";
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error"))
				{
					obj = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else if (ResultJson->Values.Find("result"))
				{
					obj = ResultJson->Values.Find("result")->Get()->AsString();
					success = true;
				}
			}
			Callback.ExecuteIfBound(obj,success);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::EditAddress(FEditAddressResponse Callback, FString Address, EAddressExpiration Expiration)
{
	FHttpRequestPtr Request = WalletAPIReq();
	FString exp = "24h";
	switch (Expiration)
	{
	case EAddressExpiration::expired:
		exp = "expired";
		break;
	case EAddressExpiration::never:
		exp = "never";
		break;
	default:
		exp = "24h";
		break;
	}
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"edit_address\",\"params\":{\"address\":\"" + Address + "\",\"expiration\":\""+exp+"\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString obj = "";
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error"))
				{
					obj = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else if (ResultJson->Values.Find("result"))
				{
					obj = ResultJson->Values.Find("result")->Get()->AsString();
					success = true;
				}
			}
			Callback.ExecuteIfBound(obj, success);
		});
	Request->ProcessRequest();
}


void ULitecashGameInstance::CreateAddress(FCreateAddressResponse Callback, EAddressExpiration Expiration)
{
	FHttpRequestPtr Request = WalletAPIReq();
	FString exp = "24h";
	switch (Expiration)
	{
	case EAddressExpiration::expired:
		exp = "expired";
		break;
	case EAddressExpiration::never:
		exp = "never";
		break;
	default:
		exp = "24h";
		break;
	}
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"create_address\",\"params\":{\"expiration\":\""+exp+"\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			FString obj = "";
			bool success = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("error"))
				{
					obj = ResultJson->Values.Find("error")->Get()->AsObject()->Values.Find("message")->Get()->AsString();
					success = false;
				}
				else if (ResultJson->Values.Find("result"))
				{
					obj = ResultJson->Values.Find("result")->Get()->AsString();
					success = true;
				}
			}
			Callback.ExecuteIfBound(obj, success);
		});
	Request->ProcessRequest();
}

void ULitecashGameInstance::ValidateAddress(FValidateAddressResponse Callback, FString Address)
{
	FHttpRequestPtr Request = WalletAPIReq();
	Request->SetContentAsString("{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"validate_address\",\"params\":{\"address\":\"" + Address + "\"}}");
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			bool mine = false;
			bool valid = false;
			TSharedPtr<FJsonObject> ResultJson = MakeShareable(new FJsonObject);
			TSharedRef<TJsonReader<TCHAR> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, ResultJson))
			{
				if (ResultJson->Values.Find("result"))
				{
					mine = ResultJson->Values.Find("result")->Get()->AsObject()->GetBoolField("is_mine");
					valid = ResultJson->Values.Find("result")->Get()->AsObject()->GetBoolField("is_valid");
				}
			}
			Callback.ExecuteIfBound(mine, valid);
});
	Request->ProcessRequest();
}

bool ULitecashGameInstance::IsNodeRunning_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::IsProcRunning(nodeProcess);
#else
	return FPlatformProcess::IsProcRunning(nodeProcess);
#endif
#else
	return false;
#endif
}

bool ULitecashGameInstance::IsExplorerNodeRunning_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::IsProcRunning(exProcess);
#else
	return FPlatformProcess::IsProcRunning(exProcess);
#endif
#else
	return false;
#endif
}

bool ULitecashGameInstance::IsWalletRunning_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::IsProcRunning(walProcess);
#else
	return FPlatformProcess::IsProcRunning(walProcess);
#endif
#else
	return false;
#endif
}

bool ULitecashGameInstance::IsWalletAPIRunning_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::IsProcRunning(wapiProcess);
#else
	return FPlatformProcess::IsProcRunning(wapiProcess);
#endif
#else
	return false;
#endif
}

FString ULitecashGameInstance::NodeStatus_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::ReadPipe(nodeReadPipe);
#else
	return FPlatformProcess::ReadPipe(nodeReadPipe);
#endif
#else
	return "Litecash node only runs in Shipping builds.";
#endif
}

FString ULitecashGameInstance::ExplorerNodeStatus_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::ReadPipe(exReadPipe);
#else
	return FPlatformProcess::ReadPipe(exReadPipe);
#endif
#else
	return "Litecash explorer node only runs in Shipping builds.";
#endif
}

FString ULitecashGameInstance::WalletStatus_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::ReadPipe(walReadPipe);
#else
	return FPlatformProcess::ReadPipe(walReadPipe);
#endif
#else
	return "Litecash node only runs in Shipping builds.";
#endif
}

FString ULitecashGameInstance::WalletAPIStatus_Implementation() const
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	return FGenericPlatformProcess::ReadPipe(wapiReadPipe);
#else
	return FPlatformProcess::ReadPipe(wapiReadPipe);
#endif
#else
	return "Litecash wallet API only runs in Shipping builds.";
#endif
}

FString ULitecashGameInstance::StartNode_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	FGenericPlatformProcess::CreatePipe(nodeReadPipe, nodeWritePipe);
	nodeProcess = FGenericPlatformProcess::CreateProc(FString("litecash-node").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, nodeWritePipe, nodeReadPipe);
	if (FGenericPlatformProcess::IsProcRunning(nodeProcess))
	{
		return "Litecash Node Started";
	}
	else
	{
		return "Litecash Node Failed To Start";
	}
#else
	FPlatformProcess::CreatePipe(nodeReadPipe, nodeWritePipe);
	nodeProcess = FPlatformProcess::CreateProc(FString("litecash-node.exe").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, nodeWritePipe, nodeReadPipe);
	if (FPlatformProcess::IsProcRunning(nodeProcess))
	{
		return "Litecash Node Started";
	}
	else
	{
		return "Litecash Node Failed To Start";
	}
#endif
#else
	return "The node can only start in Shipping builds.";
#endif
}

FString ULitecashGameInstance::StartExplorerNode_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	FGenericPlatformProcess::CreatePipe(exReadPipe, exWritePipe);
	exProcess = FGenericPlatformProcess::CreateProc(FString("explorer-node").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, exWritePipe, exReadPipe);
	if (FGenericPlatformProcess::IsProcRunning(exProcess))
	{
		return "Litecash Explorer Node Started";
	}
	else
	{
		return "Litecash Explorer Node Failed To Start";
	}
#else
	FPlatformProcess::CreatePipe(exReadPipe, exWritePipe);
	exProcess = FPlatformProcess::CreateProc(FString("explorer-node.exe").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, exWritePipe,exReadPipe);
	if (FPlatformProcess::IsProcRunning(exProcess))
	{
		return "Litecash Explorer Node Started";
	}
	else
	{
		return "Litecash Explorer Node Failed To Start";
	}
#endif
#else
	return "The Explorer Node can only start in Shipping builds.";
#endif
}

FString ULitecashGameInstance::StartWallet_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	FGenericPlatformProcess::CreatePipe(walReadPipe, walWritePipe);
	walProcess = FGenericPlatformProcess::CreateProc(FString("litecash-wallet").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, walWritePipe, walReadPipe);
	if (FGenericPlatformProcess::IsProcRunning(walProcess))
	{
		return "Litecash Wallet Started";
	}
	else
	{
		return "Litecash Wallet Failed To Start";
	}
#else
	FPlatformProcess::CreatePipe(walReadPipe, walWritePipe);
	walProcess = FPlatformProcess::CreateProc(FString("litecash-wallet.exe").GetCharArray().GetData(), FString("--command=listen").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, walWritePipe, walReadPipe);
	if (FPlatformProcess::IsProcRunning(walProcess))
	{
		return "Litecash Wallet Started";
	}
	else
	{
		return "Litecash Wallet Failed To Start";
	}
#endif
#else
	return "The Wallet can only start in Shipping builds.";
#endif
}

FString ULitecashGameInstance::StartWalletAPI_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	FGenericPlatformProcess::CreatePipe(wapiReadPipe, wapiWritePipe);
	wapiProcess = FGenericPlatformProcess::CreateProc(FString("wallet-api").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, wapiWritePipe, wapiReadPipe);
	if (FGenericPlatformProcess::IsProcRunning(wapiProcess))
	{
		return "Litecash Wallet API Started";
	}
	else
	{
		return "Litecash Wallet API Failed To Start";
	}
#else
	FPlatformProcess::CreatePipe(wapiReadPipe, wapiWritePipe);
	wapiProcess = FPlatformProcess::CreateProc(FString("wallet-api.exe").GetCharArray().GetData(), FString("").GetCharArray().GetData(), false, false, true, NULL, 0, NULL, wapiWritePipe, wapiReadPipe);
	if (FPlatformProcess::IsProcRunning(nodeProcess))
	{
		return "Litecash Wallet API Started";
	}
	else
	{
		return "Litecash Wallet API Failed To Start";
	}
#endif
#else
	return "The Wallet API can only start in Shipping builds.";
#endif
}

FString ULitecashGameInstance::StopNode_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	if (FGenericPlatformProcess::IsProcRunning(nodeProcess))
	{
		FGenericPlatformProcess::TerminateProc(nodeProcess, true);
	}
#else
	if (FPlatformProcess::IsProcRunning(nodeProcess))
	{
		FPlatformProcess::TerminateProc(nodeProcess, true);
	}
#endif
#endif
	return "Node Stopped";
}

FString ULitecashGameInstance::StopExplorerNode_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	if (FGenericPlatformProcess::IsProcRunning(exProcess))
	{
		FGenericPlatformProcess::TerminateProc(exProcess, true);
	}
#else
	if (FPlatformProcess::IsProcRunning(exProcess))
	{
		FPlatformProcess::TerminateProc(exProcess, true);
	}
#endif
#endif
	return "Explorer Node Stopped";
}

FString ULitecashGameInstance::StopWallet_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	if (FGenericPlatformProcess::IsProcRunning(walProcess))
	{
		FGenericPlatformProcess::TerminateProc(walProcess, true);
	}
#else
	if (FPlatformProcess::IsProcRunning(walProcess))
	{
		FPlatformProcess::TerminateProc(walProcess, true);
	}
#endif
#endif
	return "Wallet Stopped";
}

FString ULitecashGameInstance::StopWalletAPI_Implementation()
{
#if UE_BUILD_SHIPPING
#ifndef _WIN32
	if (FGenericPlatformProcess::IsProcRunning(wapiProcess))
	{
		FGenericPlatformProcess::TerminateProc(wapiProcess, true);
	}
#else
	if (FPlatformProcess::IsProcRunning(wapiProcess))
	{
		FPlatformProcess::TerminateProc(wapiProcess, true);
	}
#endif
#endif
	return "Wallet API Stopped";
}