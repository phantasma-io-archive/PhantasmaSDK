part I
    1 - setup and run Spook
    2 - create a new address

part II
setting up the NFT token

    1 - Creating the NFT token type
        Create tx that calls 'CreateToken' from 'Nexus' contract, passing as arguments (address, symbol, name, maxSupply, decimals, flags)
        address = the address created in part I
        symbol = "CAR"
        name = "Car Demo Token"
        maxSupply = 100000
        decimals = 0
        flags = Transferable | Finite

    2 - Minting a new CAR
        Create tx that calls 'MintToken' from 'Token' contract, passing as arguments (address, symbol, data)
        address = any address, the new minted token will appear in this address
        symbol = "CAR"
        data = byte array containing a serialized Car struct eg: var data = Phantasma.Serialization.Serializate(car)
        the tx returns an ID that identifies the newly minted token

    3 - Transfering the CAR
        Create tx that calls 'TransferToken' from 'Token' contract, passing as arguments (sourceAddress, destAddress, symbol, tokenID)
        source address = the address from previous step
        dest address = any other address
        symbol = "CAR"
        tokenID = the ID received in previous step

part III
building the store
    (esta parte foi o que fizeste já)