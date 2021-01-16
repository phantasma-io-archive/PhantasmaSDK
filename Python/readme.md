# PhantasmaSDK

## Python Sample app

This is a simple wallet sample that needs to be connected to a RPC node. By default it uses the localhost:7077/rpc endpoint, but you can switch to your own URL.

To run the sample app, follow these steps:

1. Overwrite the phantasma.py on the sample folder with the latest one from Output/temp/Python/Libs

2. Install "requests" module (Run "pip install requests" on command line inside app folder)

3. Run the sample app!

## Python VM Samples

PhanVM Module implements the basic tools you need for:

1. Decode transaction events.
/Python/Samples/VMSamples/parsetxevents.py

2. Create and Sign Transactions.
/Python/Samples/VMSamples/transferFungible.py
/Python/Samples/VMSamples/transferNonFungible.py