# PhantasmaSDK

## Java Sample app instructions

This is a simple wallet sample that needs to be connected to a RPC node. By default it uses the localhost:7077/rpc endpoint, but you can switch to your own URL.

To run the sample app, follow these steps:

1. Overwrite the existing PhantasmaAPI.java on the sample project with the latest one from Output/temp/Java/Libs

2. Run the sample app! Take note that this is a maven project, so you have to compile and run it accordingly.

For your own projects, you will need the following dependencies (all of them available on maven):
	https://github.com/arteam/simple-json-rpc
	https://hc.apache.org/httpcomponents-client-4.5.x/download.html
	https://github.com/google/guava