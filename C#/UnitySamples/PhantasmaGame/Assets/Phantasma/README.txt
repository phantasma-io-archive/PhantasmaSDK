Welcome to Phantasma Unity SDK!

In order to test and integrate Phantasma into your own projects you will need to setup a local Phantasma development network using Spook.
Spook and other Phantasma software tools can be downloaded separately from:
https://github.com/phantasma-io/PhantasmaSDK/releases

The current included sample showcases how to create a non-fungible token (NFT) store for sharing and selling digital assets (eg: game weapons, skins, characters etc)

Setting up the NFT token:

    1 - Creating the NFT token type
        Create transaction that calls 'CreateToken' from 'Nexus' contract, passing as arguments (address, symbol, name, maxSupply, decimals, flags)
			address = a new address created using Spook
			symbol = "CAR"
			name = "Car Demo Token"
			maxSupply = 100000
			decimals = 0
			flags = Transferable | Finite

    2 - Minting a new CAR
        Create transaction that calls 'MintToken' from 'Token' contract, passing as arguments (address, symbol, data)
        address = any address, the new minted token will appear in this address
        symbol = "CAR"
        data = byte array containing a serialized Car struct eg: var data = Phantasma.Serialization.Serializate(car)
        the tx returns an ID that identifies the newly minted token

    3 - Transfering the CAR
        Create transaction that calls 'TransferToken' from 'Token' contract, passing as arguments (sourceAddress, destAddress, symbol, tokenID)
        source address = the address from previous step
        dest address = any other address
        symbol = "CAR"
        tokenID = the ID received in previous step

Support:
For support please visit Phantasma at Discord:
https://discord.gg/RsKn8EN

For more info about Phantasma consult the Medium releases:
https://medium.com/phantasticphantasma