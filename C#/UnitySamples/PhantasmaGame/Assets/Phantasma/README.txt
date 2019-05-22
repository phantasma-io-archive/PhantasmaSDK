PHANTASMA SDK
______________________________________

Welcome to Phantasma Unity SDK!

Phantasma allows integrating your game with blockchain, in a completly free way, allowing you to have a full distributed game economy at zero cost, and without needing to run servers.

______________________________________
SETUP / DOWNLOADS
______________________________________

In order to test and integrate Phantasma into your own projects you will need to setup a local Phantasma development network using Spook.
Spook and other Phantasma software tools can be downloaded separately from:
https://github.com/phantasma-io/PhantasmaSDK/releases

The current included sample showcases how to create a non-fungible token (NFT) store for sharing and selling digital assets (eg: game weapons, skins, characters etc)

______________________________________
 BLOCKCHAIN CONCEPTS
______________________________________

Before integrating blockchain with your game, it is necessary to understand certain concepts.

Private Key: Every user has its own private key that gives it permissions to do sign transactions. Randomly generated once an player account is created (and stored for example along game save files).
Transaction: A transaction is a specific action that modifies the state of something stored in the blockchain. Must be cryptographically signed by someone.
Address: A string that identifies a player in an anonymous way. Cryptographically derived from the private key.
Smart Contract: A custom piece of code that is added to a blockchain to extend its features. Equivalent to a game shader that adds features to a graphics card.

A game integration with blockchain will usually consist of generating private keys for each player, then using transactions to manipulate the game state.
The transactions done by the game can represent anything like "Send item X to player Y" or "Attack player X with my character Y", as long as the things envolved are registered in the blockchain somehow.

Blockchain transactions need to be confirmed by the network, meaning they are verified to make sure they contain a valid request signed by the proper user.
This means that if someone creates a transaction saying "send item X from player Y to player Z", the network will automatically verify that the transaction was signed by player Y, and that player Y also owns item X.

This validation usually takes several seconds, which means that blockchain is more suitable for slow games, specially turn-based games.
But if you have a fast action game, you can keep the gameplay itself out of the blockchain, and only use the blockchain for the economy of the game, bringing you the best of both worlds!

______________________________________
 NON FUNGIBLE TOKENS
______________________________________


The main concept behind a blockchain-based economy is that of a Non-Fungible-Token (NFT).
A NFT represents a unique piece of your game (eg: a character, potion or weapon), that is uniquely owned by someone.
Blockchain technology fully guarantees that the NFT can't be duplicated, and only the owner can decide what to do with it, be it using it, transfer it to someone else or even destroying it.

The included demo showcases how to create a NFT for car game, where each car is an NFT that can be purchased from a marketplace.
Note that the marketplace implementation is fully abstracted, meaning you can connect it either to the decentralized Phantasma marketplace using cryptocurrencies, or with another playment service, eg: iAPs

The steps here described are already implemented in the demo, but the list here gives you a good description of the concepts and steps necessary to make your own.

Setting up the NFT token:

    1 - Creating the NFT token type
        Create transaction that calls 'CreateToken' from 'Nexus' contract, passing as arguments (address, symbol, name, maxSupply, decimals, flags)
			address = a new address created using Spook
			symbol = "CAR"
			name = "Car Demo Token"
			maxSupply = 100000
			decimals = 0
			flags = Transferable | Finite

		Explanation: For each type of token, you need to register the type in the blockchain. This is done once.
		
		Note: The NFT symbol is a simple three or four letter string choosen by you, to represent the NFT.
		This symbol works like an id in the global Phantasma network, meaning it needs to be unique, as it is used to properly identify all requests related to your game.

    2 - Minting a new CAR
        Create transaction that calls 'MintToken' from 'Token' contract, passing as arguments (address, symbol, data)
        address = any address, the new minted token will appear in this address
        symbol = "CAR"
        data = byte array containing a serialized Car struct eg: var data = Phantasma.Serialization.Serializate(car)
        the tx returns an ID that identifies the newly minted token
		
		Explanation: After you registered a token type in the previous step, you are now allowed to mint NFTs of this type.
		Minting is the term used when a new token is generated in the blockchain, think of minting as creating a new weapon or character of a specific previously registered type.
		

    3 - Transfering the CAR
        Create transaction that calls 'TransferToken' from 'Token' contract, passing as arguments (sourceAddress, destAddress, symbol, tokenID)
        source address = the address from previous step
        dest address = any other address
        symbol = "CAR"
        tokenID = the ID received in previous step
	
		Explanation: Once a token is created, the owner of it can transfer it to someone else.

______________________________________
FAQ
______________________________________

Q: I keep getting an error when starting the demo: "Cannot connect to host" 
A: This happens usually because Spook is not running in your machine. Read the "SETUP" section carefully.

______________________________________
Support:
For support please visit Phantasma at Discord:
https://discord.gg/RsKn8EN

For more info about Phantasma consult the Medium releases:
https://medium.com/phantasticphantasma