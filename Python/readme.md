# PhantasmaSDK

## Python Sample app

This is a simple wallet sample that needs to be connected to a RPC node. By default it uses the localhost:7077/rpc endpoint, but you can switch to your own URL.

To run the sample app, follow these steps:

1. Overwrite the phantasma.py on the sample folder with the latest one from Output/temp/Python/Libs

2. Install "requests" module (Run "pip install requests" on command line inside app folder)

3. Run the sample app!

## Python VM Samples

VM Module implements the following classes EventDecoder, ScriptBuilder and Transaction in order to provide support to:

- Decode TX events data.
Examples:
/Python/Samples/VMSamples/parsetxevents.py

- Create Scripts, Transactions and Sign them using HEX Private Key.
Examples:
/Python/Samples/VMSamples/transferFungible.py
/Python/Samples/VMSamples/transferNonFungible.py
