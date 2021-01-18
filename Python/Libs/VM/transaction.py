from datetime import datetime
import binascii
from ecpy.curves import Curve, Point
from ecpy.keys import ECPublicKey, ECPrivateKey
from ecpy.eddsa import EDDSA
import hashlib

from .scriptbuilder import ScriptBuilder


class Transaction():

    '''
    The Transaction object implements all the basic methods needed to build a Phantasma Transaction string
    and sign it with HEX PK in order to broadcast to the Phantasma Blockchain.

    Args:
        nexusName (str): mainnet (production) or testnet (testing).
        chainName (str): chain name (main).
        script (str): script to execute.
        expiration (int): expiration timestamp.
        payload (str): payload message.

    Attributes:
        nexusName (str): mainnet (production) or testnet (testing).
        chainName (str): chain name (main).
        script (str): script to execute.
        expiration (int): expiration timestamp.
        payload (str): payload message.
        signatures (list): list of signatures, at least one is needed to broadcast tx.
    '''

    nexusName: str = None
    chainName: str = None
    script: str = None
    expiration: int = None
    payload: str = None

    signatures = []

    def __init__(
            self,
            nexusName: str,
            chainName: str,
            script: str,
            expiration: int,
            payload: str = 'PHANPY-1.0') -> None:

        if expiration is None:
            now = datetime.now()
            expiration = int(datetime.timestamp(now)) + 1000

        self.nexusName = nexusName
        self.chainName = chainName
        self.script = script
        self.expiration = expiration
        self.payload = payload
        self.signatures = []

    def sign(self, pk: int) -> None:
        '''Sign the transaction with HEX PK of the Sender's Address.

        Args:
            pk (int): HEX private key. Example: 0x411d7dabb39b455aadc49897e2fa13234585116d4f5eee198105f33f8f62d0b8

        Returns:
            None (The generated signature is added to the signatures list)
        '''
        signature = self.getSign(self.toString(False), pk)
        self.signatures.insert(0, signature)

    def toString(self, withSignature: bool) -> str:
        '''By using a ScriptBuilder object this method generates the needed string to broadcast the tx to
        the Phanstasma Blockchain.

        Args:
            withSignature (bool): flags that indicates if need to attach signatures or not to the end result.

        Returns:
            toString (str): Transaction object as string.
        '''

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

    def getSign(self, msgHex: str, pk: int) -> bytes:
        '''Method that given a message to sign & HEX private key returns the signature needed.

        Args:
           msgHex (str): Message to sign as Hex String.
           pk (int): HEX private key. Example: 0x411d7dabb39b455aadc49897e2fa13234585116d4f5eee198105f33f8f62d0b8

        Returns:
            getSign (bytes): Signature as bytes.
        '''
        hexBytes = binascii.unhexlify(msgHex)
        hexPk = int(pk)

        cv = Curve.get_curve('Ed25519')
        pv_key = ECPrivateKey(hexPk, cv)
        signer = EDDSA(hashlib.sha512)

        sig = signer.sign(hexBytes, pv_key)
        sig = binascii.hexlify(sig)

        return(sig)
