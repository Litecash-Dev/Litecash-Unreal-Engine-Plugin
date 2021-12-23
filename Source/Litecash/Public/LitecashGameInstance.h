// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#if UE_BUILD_SHIPPING
#ifndef _WIN32
#include "GenericPlatform/GenericPlatformProcess.h"
#endif
#endif
#include "LitecashGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FWalletStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		int Available;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		int Receiving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		int Sending;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		int Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		int Maturing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		float Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		FString CurrentStateHash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|WalletStatus")
		FString PreviousStateHash;
};


USTRUCT(BlueprintType)
struct FTx
{
    GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		FString Comment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		int Confirmations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		int CreateTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		int Fee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		int Height;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		bool Income;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		FString Kernel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		FString Receiver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		FString Sender;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		int Status;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		FString StatusString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		FString TxId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
		int Value;
};

USTRUCT(BlueprintType)
struct FTxStatus
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        FString Comment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        int CreateTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        int Fee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        bool Income;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        FString FailureReason;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        FString Receiver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        FString Sender;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        int Status;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        FString StatusString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        FString TxId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|TX")
        int Value;
};

USTRUCT(BlueprintType)
struct FUTXO
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		int Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		FString CreateTxId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		int Maturity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		int Session;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		FString SpentTxId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		int Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		FString StatusString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|UTXO")
		FString Type;
};

USTRUCT(BlueprintType)
struct FAddress
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        FString Address;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        FString Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        FString Comment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        int CreateTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        int Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        bool Expired;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Litecash|Addresses")
        bool Own;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHttpRequestResponse, FString, ResponseString, int, HttpStatus, bool, Success);

DECLARE_DYNAMIC_DELEGATE_OneParam(FWalletAPIStatusResponse, const FWalletStatus&, WalletStatus);

DECLARE_DYNAMIC_DELEGATE_OneParam(FWalletAPITxListResponse, const TArray<FTx>&, Transactions);

DECLARE_DYNAMIC_DELEGATE_OneParam(FWalletAPIUTXOResponse, const TArray<FUTXO>&, Transactions);

DECLARE_DYNAMIC_DELEGATE_OneParam(FWalletAPIAddressListResponse, const TArray<FAddress>&, Addresses);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDeleteAddressResponse, FString, Result, bool, Success);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FEditAddressResponse, FString, Result, bool, Success);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateAddressResponse, FString, NewAddress, bool, Success);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FValidateAddressResponse, bool, Mine, bool, Valid);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FTxStatusResponse, FTxStatus, TxStatus, bool, Success);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FTxSendResponse, FString, Result, bool, Success);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FTxSplitResponse, FString, ReturnValue, bool, bSuccessful);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FTxResponse, FString, ReturnValue, bool, bSuccessful);

UENUM(BlueprintType, Category = "Litecash|Addresses")
enum EAddressExpiration
{
    day,
    never,
    expired
};

UCLASS()
class ULitecashGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintCallable, Category = "Litecash|HTTPQuery")
        static void GetRequest(FString URL, FHttpRequestResponse Callback);

    UFUNCTION(BlueprintCallable, Category = "Litecash|HTTPQuery")
        static void PostRequest(FString URL, FString Data, FHttpRequestResponse Callback);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APIQuery")
        static void APIWalletStatus(FWalletAPIStatusResponse Callback);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APIQuery")
        static void APITxList(FWalletAPITxListResponse Callback);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APIQuery")
        static void APIUTXOList(FWalletAPIUTXOResponse Callback);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APIQuery")
        static void APIAddressList(FWalletAPIAddressListResponse Callback, bool OwnAddresses);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Tx")
        static void TxCancel(FTxResponse Callback, FString TxId);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Tx")
        static void TxStatus(FTxStatusResponse Callback, FString TxId);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Tx")
        static void TxSplit(FTxSplitResponse Callback, const TArray<FString>& Coins, int Fee);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Tx")
        static void TxSend(FTxSendResponse Callback, int Value, int Fee, FString From, FString To, FString Comment);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Address")
        static void DeleteAddress(FDeleteAddressResponse Callback, FString Address);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Address")
        static void EditAddress(FEditAddressResponse Callback, FString Address, EAddressExpiration Expiration);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Address")
        static void CreateAddress(FCreateAddressResponse Callback, EAddressExpiration Expiration);

    UFUNCTION(BlueprintCallable, Category = "Litecash|APICommand|Address")
        static void ValidateAddress(FValidateAddressResponse Callback, FString Address);

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Litecash|Node")
        bool IsNodeRunning() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|ExplorerNode")
        bool IsExplorerNodeRunning() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Wallet")
        bool IsWalletRunning() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|WalletAPI")
        bool IsWalletAPIRunning() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Node")
        FString NodeStatus() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|ExplorerNode")
        FString ExplorerNodeStatus() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Wallet")
        FString WalletStatus() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|WalletAPI")
        FString WalletAPIStatus() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Node")
        FString StartNode();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|ExplorerNode")
        FString StartExplorerNode();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Wallet")
        FString StartWallet();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|WalletAPI")
        FString StartWalletAPI();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Node")
        FString StopNode();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|ExplorerNode")
        FString StopExplorerNode();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|Wallet")
        FString StopWallet();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Litecash|WalletAPI")
        FString StopWalletAPI();
};