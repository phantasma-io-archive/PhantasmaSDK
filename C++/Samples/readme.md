# PhantasmaSDK

## C++ Sample app

This is a simple wallet sample that needs to be connected to a RPC node. By default it uses the localhost:7077/rpc endpoint, but you can switch to your own URL [here](https://github.com/phantasma-io/PhantasmaSDK/blob/master/C%23/WalletSample/WalletSample/Program.cs#L37).

To run the sample app, follow these steps:

1. Install vcpkg, following the instructions on https://github.com/Microsoft/vcpkg

2. After successfully installing vcpkg, install cpprestsdk with "vcpkg install cpprestsdk cpprestsdk:x64-windows" on a command prompt

3. Overwrite the PhantasmaAPI.cpp and .h files with the ones on Output/temp/C++/libs

4. Run the sample app!

For your own projects, take note that the sample project has a dependency on https://github.com/nlohmann/json which you should use as well. Easiest ways to get it are from either vcpkg or Nuget.  