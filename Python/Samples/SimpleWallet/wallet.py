from Phantasma import PhantasmaAPI

api = PhantasmaAPI("http://localhost:7077/rpc")
print(api)
response = api.getAccount("P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr")
print(response)