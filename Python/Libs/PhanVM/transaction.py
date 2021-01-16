from datetime import datetime
import binascii
from ecpy.curves import Curve,Point
from ecpy.keys import ECPublicKey, ECPrivateKey
from ecpy.eddsa import EDDSA
import hashlib

from .scriptbuilder import ScriptBuilder


class Transaction():
	nexusName = None
	chainName = None
	script = None
	expiration = None
	payload = None

	signatures = []

	def __init__(self, nexusName, chainName, script, expiration = None, payload = 'PHANPY-1.0'):

		if expiration is None:
		    now = datetime.now()
		    expiration = int(datetime.timestamp(now)) + 1000

		self.nexusName = nexusName
		self.chainName = chainName
		self.script = script
		self.expiration = expiration
		self.payload = payload
		self.signatures = []

	def sign(self, pk):
		signature = self.getSign(self.toString(False),pk)
		self.signatures.insert(0, signature)

	def toString(self, withSignature):

		num = self.expiration
		a = (num & 0xff000000) >> 24
		b = (num & 0x00ff0000) >> 16
		c = (num & 0x0000ff00) >> 8
		d = num & 0x000000ff

		expirationBytes = [d, c, b, a]

		sb = ScriptBuilder()
		sb.emitVarString(self.nexusName)
		sb.emitVarString(self.chainName)
		sb.emitVarInt(int(len(self.script) / 2))
		sb.appendHexEncoded(self.script)
		sb.emitBytes(expirationBytes)
		sb.emitVarString(self.payload)

		if (withSignature):
			sb.emitVarInt(len(self.signatures))
			for s in self.signatures:
				sb.appendByte(1)
				sig = s.decode("utf-8").upper()
				sb.emitVarInt(int(len(sig) / 2))
				sb.appendHexEncoded(sig)

		return sb.data

	def getSign(self, msgHex, pk):

		hexBytes = binascii.unhexlify(msgHex)
		hexPk = int(pk)

		cv = Curve.get_curve('Ed25519')
		pv_key = ECPrivateKey(hexPk,cv)
		signer = EDDSA(hashlib.sha512)

		sig = signer.sign(hexBytes,pv_key)
		sig = binascii.hexlify(sig)
		
		return(sig)