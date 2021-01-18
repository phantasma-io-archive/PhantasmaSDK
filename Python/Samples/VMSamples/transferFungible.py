import json
import requests
import sys

sys.path.append('../../Libs')
from VM import VMType, Opcode, ScriptBuilder, Transaction

if __name__ == '__main__':

    gasPrice = 100000
    minGasLimit = 100000

    # Sender's PHAN address (String)
    senderAddress = '<sender address>'

    # Sender's private key in Hex, needs to be a native hex var starting with
    # 0x not use HEX string here.
    # Example: 0x411d7dabb39b455aadc49897e2fa13234585116d4f5eee198105f33f8f62d0b8
    senderHexPk = 0x0

    # Destination PHAN address (String)
    destination = '<destination address>'

    # FT symbol, example 'KCAL'
    tokenSymbol = 'KCAL'

    # FT decimals, for KCAL is 10
    tokenDecimals = 10

    # Amount to transfer without decimals (in this example we are sending just
    # 1 KCAL)
    tokenAmount = int(1 * pow(10, tokenDecimals))

    # Script Creation
    sb = ScriptBuilder()
    sb.beginScript()

    # print(VMType.STRING)

    sb.allowGas(senderAddress, sb.nullAddress, gasPrice, minGasLimit)

    # FT
    sb.callInterop(
        "Runtime.TransferTokens", [
            senderAddress, destination, tokenSymbol, tokenAmount])

    sb.spendGas(senderAddress)
    script = sb.endScript()

    print("\nTX SCRIPT: ", script)

    # TX Creation & Signing
    tx = Transaction(
        "mainnet",  # NEXUS (mainnet or testnet)
        "main",  # CHAIN
        script,  # SCRIPT
        None,
        # EXPIRATION (Leave it empty and the module will create a valid one for
        # you)
        "PHANTASMAROCKS"  # PAYLOAD
    )

    tx.sign(senderHexPk)

    signedTx = tx.toString(True)

    # Signed TX ready to be sent using PHAN API...
    print("\nSIGNED TX: ", signedTx)
