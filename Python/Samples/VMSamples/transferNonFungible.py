import json
import requests
import sys

sys.path.append('../../Libs')
from PhanVM import ScriptBuilder, Transaction

if __name__ == '__main__':

	gasPrice = 100000
	minGasLimit = 100000

	#Sender's PHAN address (String)
	senderAddress = '<sender address>'

	#Sender's private key in Hex, needs to be a native hex var starting with 0x not use HEX string here.
	senderHexPk = 0x<HEXPK>

	#Destination PHAN address (String)
	destination='<destination address>'

	#NFT symbol, example 'GHOST'
	nftSymbol = 'GHOST'
	#NFT id example: https://ghostmarket.io/asset/pha/ghost/24008292598686948699485601600159961578713145374071314218471279306976607160732/
	nftId = 24008292598686948699485601600159961578713145374071314218471279306976607160732

	# Script Creation
	sb = ScriptBuilder()
	sb.beginScript()
	sb.allowGas(senderAddress, sb.nullAddress, gasPrice, minGasLimit)

	#NFT
	sb.callInterop("Runtime.TransferToken", [senderAddress, destination, nftSymbol, nftId])

	sb.spendGas(senderAddress)
	script = sb.endScript()

	print("\nTX SCRIPT: ", script)

	# TX Creation & Signing
	tx = Transaction(
		"mainnet", #NEXUS (mainnet or testnet)
		"main", #CHAIN
		script, #SCRIPT
		None, #EXPIRATION (Leave it empty and the module will create a valid one for you)
		"PHANTASMAROCKS" #PAYLOAD 
	)

	tx.sign(senderHexPk)

	signedTx = tx.toString(True)

	# Signed TX ready to be sent using PHAN API...
	print("\nSIGNED TX: ", signedTx)

