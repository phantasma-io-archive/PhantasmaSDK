from datetime import datetime
from typing import Any
from enum import Enum, IntEnum
from .opcode import Opcode
from .vmtype import VMType


class ScriptBuilder():

    '''
    The ScriptBuilder object implements all the methods needed to build a Phantasma Script string
    which is needed to interact with the Phantasma Blockchain.

    Args:
        None

    Attributes:
        data (str): This is where we store the script string.
        Nexus (dict): Dictionary which contains Phantasma Blockchain contract names.
        nullAddress (str): Phantasma Blockchain NULL address representation.
        _labelLocations (list): Label's location list
        _jumpLocations (list): Jump's location list
    '''

    Nexus = dict(
        GasContractName='gas',
        BlockContractName='block',
        StakeContractName='stake',
        SwapContractName='swap',
        AccountContractName='account',
        ConsensusContractName='consensus',
        GovernanceContractName='governance',
        StorageContractName='storage',
        ValidatorContractName='validator',
        InteropContractName='interop',
        ExchangeContractName='exchange',
        PrivacyContractName='privacy',
        RelayContractName='relay',
        RankingContractName='ranking'
    )

    MAX_REGISTER_COUNT = 32

    data: str = None
    _labelLocations = []
    _jumpLocations = []

    nullAddress = "S1111111111111111111111111111111111"

    def __init__(self) -> None:
        self.data = ""
        self._labelLocations = []
        self._jumpLocations = []

    def beginScript(self) -> None:
        '''This method initializes an empty script.

        Args:
            None

        Returns:
            None
        '''
        
        self.data = ""
        self._labelLocations = []
        self._jumpLocations = []

    def getScript(self) -> str:
        '''This method returns the generated script as string.

        Args:
            None

        Returns:
            getScript (str): end result script.
        '''
        return self.data

    def endScript(self) -> str:
        '''This method finishes the script and return it as string.

        Args:
            None

        Returns:
            endScript (str): end result script.
        '''
        self.emit(Opcode.RET)
        return self.data

    def emit(self, opcode: Opcode, data: Any = None) -> None:
        '''This method emits an opcode with its data.

        Args:
            opcode (Opcode): Blockchain opcode to be emitted.
            data (Any): Data related to the opcode to be emitted.

        Returns:
            None
        '''
        self.appendByte(opcode)
        if data is not None:
            if isinstance(data, list):
                for d in data:
                    self.emit(d)
            else:
                self.emit(data)
        return self

    def emitPush(self, reg: int) -> None:
        '''This method emits a PUSH opcode.

        Args:
            reg (int): register

        Returns:
            None
        '''
        self.emit(Opcode.PUSH)
        self.appendByte(reg)
        return self

    def emitPop(self, reg: int) -> None:
        '''This method emits a POP opcode.

        Args:
            reg (int): register

        Returns:
            None
        '''
        self.emit(Opcode.POP)
        self.appendByte(reg)
        return self

    def emitExtCall(self, method: str, reg: int = 0) -> None:
        '''This method emits a EXTCALL opcode.

        Args:
            method (str): method to be executed.
            reg (int): register

        Returns:
            None
        '''
        self.emitLoad(reg, method)
        self.emit(Opcode.EXTCALL)
        self.appendByte(reg)
        return self

    def rawString(self, value: str) -> list:
        '''This method converts an string to a list of byte characters.

        Args:
            value (str): string to be converted.

        Returns:
            rawString (list): list of byte chars.
        '''
        data = []
        for c in list(value):
            data.append(ord(c))
        return data

    def emitLoad(self, reg: int, obj: Any) -> None:
        '''This method loads data to the script depending of the object type
        Supported object types are: str, bool, int, float and datetime.

        Args:
            reg (int): register
            obj (Any): data

        Returns:
            None
        '''

        # String
        typeLoaded = False
        if (isinstance(obj, str)):
            data = self.rawString(obj)
            self.emitLoadBytes(reg, data, VMType.STRING)
            typeLoaded = True

        # Boolean
        if (isinstance(obj, bool)):
            data = []
            if obj:
                data.append(1)
            else:
                data.append(0)
            self.emitLoadBytes(reg, data, VMType.BOOL)
            typeLoaded = True

        # Number
        if ((isinstance(obj, int)) or (isinstance(obj, float))):
            data = self.rawString(str(obj))
            self.emitLoadBytes(reg, data, VMType.STRING)
            typeLoaded = True

        # Timestamp
        if (isinstance(obj, datetime)):
            data = self.rawString(str(obj))
            self.emitLoadTimestamp(reg, obj)
            typeLoaded = True

        if not typeLoaded:
            raise Exception("Load type " + str(type(obj)) +
                            " is not supported")

        return self

    def emitLoadBytes(self, reg: int, data: bytes = None,
                      typ: VMType = VMType.BYTES) -> None:

        '''This method loads bytes data to the script.

        Args:
            reg (int): register.
            data (bytes): data.
            typ (VMType): data type.

        Returns:
            None
        '''

        if data is None:
            data = []

        if len(data) > 0xffff:
            raise Exception("tried to load too much data")

        self.emit(Opcode.LOAD)
        self.appendByte(reg)
        self.appendByte(typ)
        self.emitVarInt(len(data))
        self.emitBytes(data)
        return self

    def emitLoadEnum(self, reg: int, val: int) -> None:
        '''This method loads enum data to the script.

        Args:
            reg (int): register.
            val (int): data.

        Returns:
            None
        '''

        data = [0, 0, 0, 0]

        for i in range(len(data)):
            data[c] = (val & 0xff)
            val = (val - (val & 0xff)) / 256

        self.emitLoadBytes(reg, data, VMType.ENUM)
        return self

    def emitLoadTimestamp(self, reg: int, obj: datetime) -> None:
        '''This method loads a datetime data to the script as an UTC timestamp.

        Args:
            reg (int): register.
            obj (datetime): data.

        Returns:
            None
        '''

        num = int(obj.replace(tzinfo=timezone.utc).timestamp())

        a = (num & 0xff000000) >> 24
        b = (num & 0x00ff0000) >> 16
        c = (num & 0x0000ff00) >> 8
        d = num & 0x000000ff

        data = [d, c, b, a]
        self.emitLoadBytes(reg, data, VMType.TIMESTAMP)
        return self

    def emitMove(self, src_reg: int, dst_reg: int) -> None:
        '''This method emits a MOVE opcode.

        Args:
            src_reg (int): source register.
            dst_reg (int): destination register.

        Returns:
            None
        '''

        self.emit(Opcode.MOVE)
        self.appendByte(src_reg)
        self.appendByte(dst_reg)
        return self

    def emitCopy(self, src_reg: int, dst_reg: int) -> None:
        '''This method emits a COPY opcode.

        Args:
            src_reg (int): source register.
            dst_reg (int): destination register.

        Returns:
            None
        '''

        self.emit(Opcode.COPY)
        self.appendByte(src_reg)
        self.appendByte(dst_reg)
        return self

    def emitLabel(self, label: str) -> None:
        '''This method loads a label into the script.

        Args:
            label (str): label data.

        Returns:
            None
        '''

        self.emit(Opcode.NOP)
        self._labelLocations[label] = len(self.data)
        return self

    def emitJump(self, opcode: Opcode, label: str, reg: int = 0) -> None:
        '''This method emits a JUMP opcode.

        Args:
            opcode (Opcode): jump opcode to add: Opcode.JMP, Opcode.JMPIF or Opcode.JMPNOT
            label (str): label to jump.
            reg (int): register.

        Returns:
            None
        '''

        if ((opcode == Opcode.JMP) or (
                opcode == Opcode.JMPIF) or (opcode == Opcode.JMPNOT)):
            self.emit(opcode)
        else:
            raise Exception("Invalid jump opcode: " + str(opcode))

        if (opcode != Opcode.JMP):
            self.appendByte(reg)

        ofs = len(self.data)
        self.appendUshort(0)
        self._jumpLocations[ofs] = label
        return self

    def emitCall(self, label: str, regCount: int) -> None:
        '''This method emits a CALL opcode.

        Args:
            label (str): label to call.
            regCount (int): register counter.

        Returns:
            None
        '''

        if ((regCount < 1) or (regCount > MAX_REGISTER_COUNT)):
            raise Exception("Invalid number of registers")

        ofs = (len(self.data)) + 2
        self.emit(Opcode.CALL)
        self.appendUshort(0)
        self._jumpLocations[ofs] = label
        return self

    def emitConditionalJump(
            self,
            opcode: Opcode,
            src_reg: int,
            label: str) -> None:
        '''This method emits a conditional JUMP opcode.

        Args:
            opcode (Opcode): jump opcode to add: only Opcode.JMPIF and Opcode.JMPNOT are valid.
            label (str): label to jump.
            src_reg (int): source register.

        Returns:
            None
        '''

        if ((opcode != Opcode.JMPIF) and (
                opcode != Opcode.JMPNOT)):
            raise Exception("Opcode is not a conditional jump: " + str(opcode))

        ofs = (len(self.data)) + 2
        self.emit(opcode)
        self.appendUshort(0)
        self._jumpLocations[ofs] = label
        return self

    def insertMethodArgs(self, args: list) -> None:
        '''This loads method arguments into the script.

        Args:
            args (list): list of arguments.

        Returns:
            None
        '''

        temp_reg = 0
        for arg in reversed(args):
            self.emitLoad(temp_reg, arg)
            self.emitPush(temp_reg)

    def callInterop(self, method: str, args: list) -> None:
        '''This method loads an external method and its arguments into the script.

        Args:
            method (str): method to be executed.
            args (list): list of arguments.

        Returns:
            None
        '''

        self.insertMethodArgs(args)

        dest_reg = 0
        self.emitLoad(dest_reg, method)
        self.emit(Opcode.EXTCALL, dest_reg)
        return self

    def callContract(self, contractName: str, method: str, args: list) -> None:
        '''This method adds a contract method call with its arguments into the script.

        Args:
            contractName (str): contract name to be executed.
            method (str): method to be executed.
            args (list): list of arguments.

        Returns:
            None
        '''

        self.insertMethodArgs(args)

        temp_reg = 0
        self.emitLoad(temp_reg, method)
        self.emitPush(temp_reg)

        src_reg = 0
        dest_reg = 1
        self.emitLoad(src_reg, contractName)
        self.emit(Opcode.CTX, [src_reg, dest_reg])
        self.emit(Opcode.SWITCH, [dest_reg])
        return self

    def emitVarString(self, text: str) -> None:
        '''This method loads a str variable into the script.

        Args:
            text (str): string data.

        Returns:
            None
        '''

        data = self.rawString(text)
        self.emitVarInt(len(data))
        self.emitBytes(data)
        return self

    def emitVarInt(self, value: int) -> None:
        '''This method loads a int variable into the script.

        Args:
            value (int): int data.

        Returns:
            None
        '''

        if value < 0:
            raise Exception("Negative value invalid")
        if (value < 0xfd):
            self.appendByte(value)
        elif (value <= 0xffff):
            B = (value & 0x0000ff00) >> 8
            A = value & 0x000000ff
            self.appendByte(0xfd)
            self.appendByte(A)
            self.appendByte(B)
        elif (value <= 0xffffffff):
            C = (value & 0x00ff0000) >> 16
            B = (value & 0x0000ff00) >> 8
            A = value & 0x000000ff
            self.appendByte(0xfe)
            self.appendByte(A)
            self.appendByte(B)
            self.appendByte(C)
        else:
            D = (value & 0xff000000) >> 24
            C = (value & 0x00ff0000) >> 16
            B = (value & 0x0000ff00) >> 8
            A = value & 0x000000ff
            self.appendByte(0xff)
            self.appendByte(A)
            self.appendByte(B)
            self.appendByte(C)
            self.appendByte(D)
        return self

    def emitBytes(self, data: bytes) -> None:
        '''This method loads a bytes variable into the script.

        Args:
            data (bytes): bytes data.

        Returns:
            None
        '''

        for i in data:
            self.appendByte(i)

    def byteToHex(self, data: int) -> str:
        '''This method converts a byte into an str hex representation.

        Args:
            data (int): byte data to be converted to hex.

        Returns:
            byteToHex (str): str hex representatiom of the byte.
        '''

        res = format(data, 'x').upper()
        if len(res) == 1:
            res = "0" + res
        return res

    def appendByte(self, data: int) -> None:
        '''This method loads a byte variable into the script.

        Args:
            data (int): int data.

        Returns:
            None
        '''

        self.data = self.data + self.byteToHex(data)

    def appendUshort(self, ushort: int) -> None:
        '''This method loads a unsigned short int variable into the script.

        Args:
            data (int): unsigned short int data.

        Returns:
            None
        '''

        self.data = self.data + \
            (self.byteToHex(ushort & 0xff)) + (this.byteToHex((ushort >> 8) & 0xff))

    def appendHexEncoded(self, data: str) -> None:
        '''This method loads an Hex string variable into the script.

        Args:
            data (str): Hex data to be loaded.

        Returns:
            None
        '''

        self.data = self.data + data
        return self

    # ScriptBuilderExtensions
    def allowGas(
            self,
            frm: str,
            to: str,
            gasPrice: int,
            gasLimit: int) -> None:
        return self.callContract(
            self.Nexus["GasContractName"], "AllowGas", [
                frm, to, gasPrice, gasLimit])
        '''This method is a wrapper that invokes the method AllowGas from the gas contract.

        Args:
            frm (str): Phantasma wallet address where gas fees are going to be spent.
            to (str): Phantasma wallet address where gas fees are going to be sent (NULL Address).
            gasPrice (int): Max gas price to used.
            gasLimit (int): Gas limit to be used.

        Returns:
            None
        '''

    def spendGas(self, address: str) -> None:
        return self.callContract(
            self.Nexus["GasContractName"], "SpendGas", [address])
        '''This method is a wrapper that invokes the method SpendGas from the gas contract.

        Args:
            address (str): Phantasma wallet address where gas fees are going to be spent.

        Returns:
            None
        '''
           
