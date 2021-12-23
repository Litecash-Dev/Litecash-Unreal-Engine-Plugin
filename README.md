# Litecash Unreal Engine Plugin
Supports UE 4.27

Getting Started:

Copy the plugin to your game's source directory. <Game>/Plugins/Litecash

Configure the Litecash CLI applications to run your wallet or init a new one.

While in the editor the processes cannot be launched, but are easily accessible by running the Litecash CLI exes along with the editor.

Open Project settings.

Select Maps & Modes.

At the bottom set the gameinstance to LitecashGameInstanceBP

If your game already has a custom GameInstance, open that game instance, click settings, and change its parent to LitecashGameInstanceBP

You're ready to implement the included widgets anywhere in your game, or create new ones from the functions provided by the game instance.

When you're ready to ship, package your game for shipping, and copy the Litecash exe files to the bin directory of the packaged game.
