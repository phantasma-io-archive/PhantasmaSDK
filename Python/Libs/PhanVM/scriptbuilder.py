from datetime import datetime

class ScriptBuilder():

	MAX_REGISTER_COUNT = 32

	Nexus = dict(
		GasContractName='gas',
		BlockContractName = 'block',
		StakeContractName = 'stake',
		SwapContractName = 'swap',
		AccountContractName = 'account',
		ConsensusContractName='consensus',
		GovernanceContractName = 'governance',
		StorageContractName = 'storage',
		ValidatorContractName = 'validator',
		InteropContractName = 'interop',
		ExchangeContractName='exchange',
		PrivacyContractName = 'privacy',
		RelayContractName = 'relay',
		RankingContractName = 'ranking'
	)

	VMType = dict(
	  NONE=0,
	  STRUCT=1,
	  BYTES=2,
	  NUMBER=3,
	  STRING=4,
	  TIMESTAMP=5,
	  BOOL=6,
	  ENUM=7,
	  OBJECT=8
	)

	Opcode = dict(
		NOP=0,
		#Register
		MOVE=1,
		COPY=2,
		PUSH=3,
		POP=4,
		SWAP=5,
		#Flow
		CALL=6,
		EXTCALL=7,
		JMP=8,
		JMPIF=9,
		JMPNOT=10,
		RET=11,
		THROW=12,
		#Data
		LOAD=13,
		CAST=14,
		CAT=15,
		SUBSTR=16,
		LEFT=17,
		RIGHT=18,
		SIZE=19,
		COUNT=20,
		#Logical
		NOT=21,
		AND=22,
		OR=23,
		XOR=24,
		EQUAL=25,
		LT=26,
		GT=27,
		LTE=28,
		GTE=29,
		#Numeric
		INC=30,
		DEC=31,
		SIGN=32,
		NEGATE=33,
		ABS=34,
		ADD=35,
		SUB=36,
		MUL=37,
		DIV=38,
		MOD=39,
		SHL=40,
		SHR=41,
		MIN=42,
		MAX=43,
		#Context
		THIS=44,
		CTX=45,
		SWITCH=46,
		#Array
		PUT=47,
		GET=48
	)

	data = None 
	_labelLocations = []
	_jumpLocations = []

	nullAddress = "S1111111111111111111111111111111111"

	def __init__(self):
		self.data = ""
		self._labelLocations = []
		self._jumpLocations = []

	def beginScript(self):
		self.data = ""
		self._labelLocations = []
		self._jumpLocations = []
		
	def getScript(self):
		return self.data			

	def endScript(self):
		self.emit(self.Opcode["RET"])
		return self.data

	def emit(self, opcode, data = None):
		self.appendByte(opcode)
		if data is not None:
			if type(data) is list:
				for d in data:
					self.emit(d)
			else:
				self.emit(data)
		return self

	def emitPush(self, reg):
		self.emit(self.Opcode["PUSH"])
		self.appendByte(reg)
		return self

	def emitPop(self, reg):
		self.emit(Opcode["POP"])
		self.appendByte(reg)
		return self

	def emitExtCall(self, method, reg = 0):
		self.emitLoad(reg, method)
		self.emit(self.Opcode["EXTCALL"])
		self.appendByte(reg)
		return self

	def rawString(self, value):
		data = []
		for c in list(value):
			data.append(ord(c))
		return data

	def emitLoad(self, reg, obj):
		#String
		typeLoaded = False
		if (isinstance(obj,str)):
			data = self.rawString(obj)
			self.emitLoadBytes(reg, data, self.VMType["STRING"])
			typeLoaded = True

		#Boolean
		if (isinstance(obj,bool)):
			data = []
			if obj:
				data.append(1)
			else:
				data.append(0)	
			self.emitLoadBytes(reg, data, self.VMType["BOOL"])
			typeLoaded = True

		#Number
		if ((isinstance(obj,int)) or (isinstance(obj,float))):
			data = self.rawString(str(obj))
			self.emitLoadBytes(reg, data, self.VMType["STRING"])
			typeLoaded = True

		#Timestamp
		if (isinstance(obj,datetime)):
			data = self.rawString(str(obj))
			self.emitLoadTimestamp(reg, obj)
			typeLoaded = True

		if typeLoaded == False:
			raise Exception("Load type " + str(type(obj)) + " is not supported")

		return self

	def emitLoadBytes(self, reg, data=None, typ=None):

		if typ is None:
			type=self.VMType["BYTES"]

		if data is None:
			data =[]

		if len(data) > 0xffff:
			raise Exception("tried to load too much data")

		self.emit(self.Opcode["LOAD"])
		self.appendByte(reg)
		self.appendByte(typ)
		self.emitVarInt(len(data))
		self.emitBytes(data)
		return self

	def emitLoadEnum(self, reg, val):
		data = [0,0,0,0]

		for i in range(len(data)):
			data[c] = (val & 0xff)
			val = (val - (val & 0xff)) / 256

		self.emitLoadBytes(reg, data, self.VMType.ENUM)
		return self

	def emitLoadTimestamp(self, reg, obj):
		num = int(obj.replace(tzinfo=timezone.utc).timestamp())

		a = (num & 0xff000000) >> 24;
		b = (num & 0x00ff0000) >> 16;
		c = (num & 0x0000ff00) >> 8;
		d = num & 0x000000ff;

		data = [d, c, b, a];
		self.emitLoadBytes(reg, data, self.VMType.TIMESTAMP);
		return self;

	def emitMove(self, src_reg, dst_reg):
		self.emit(Opcode["MOVE"])
		self.appendByte(src_reg)
		self.appendByte(dst_reg)
		return self

	def emitCopy(self, src_reg, dst_reg):
		self.emit(Opcode["COPY"])
		self.appendByte(src_reg)
		self.appendByte(dst_reg)
		return self

	def emitLabel(self, label):
		self.emit(Opcode["NOP"])
		self._labelLocations[label] =len(self.data)
		return self

	def emitJump(self, opcode, label, reg = 0):
		
		if ((opcode == self.Opcode["JMP"]) or (opcode == self.Opcode["JMPIF"]) or (opcode == self.Opcode["JMPNOT"])):
			self.emit(opcode)
		else:	
			raise Exception("Invalid jump opcode: " + str(opcode))

		if (opcode != self.Opcode["JMP"]):
			self.appendByte(reg)

		ofs = len(self.data)
		self.appendUshort(0)
		self._jumpLocations[ofs] = label
		return self

	def emitCall(self, label, regCount):
		
		if ((regCount < 1) or (regCount > MAX_REGISTER_COUNT)):
			raise Exception("Invalid number of registers")
		
		ofs = (len(self.data)) + 2
		self.emit(self.Opcode["CALL"])
		self.appendUshort(0)
		self._jumpLocations[ofs] = label
		return self

	def emitConditionalJump(self, opcode, src_reg, label):
		
		if ((opcode != self.Opcode["JMPIF"]) and (opcode != self.Opcode["JMPNOT"])):
			raise Exception("Opcode is not a conditional jump: " + str(opcode))
		
		ofs = (len(self.data)) + 2
		self.emit(opcode)
		self.appendUshort(0)
		self._jumpLocations[ofs] = label
		return self

	def insertMethodArgs(self, args):
		temp_reg = 0
		for arg in reversed(args):
			self.emitLoad(temp_reg, arg)
			self.emitPush(temp_reg)

	def callInterop(self, method, args):
		self.insertMethodArgs(args)

		dest_reg = 0
		self.emitLoad(dest_reg, method)
		self.emit(self.Opcode["EXTCALL"], dest_reg)
		return self

	def callContract(self, contractName, method, args):
		self.insertMethodArgs(args)

		temp_reg = 0
		self.emitLoad(temp_reg, method)
		self.emitPush(temp_reg)

		src_reg = 0;
		dest_reg = 1;
		self.emitLoad(src_reg, contractName)
		self.emit(self.Opcode["CTX"], [src_reg, dest_reg])
		self.emit(self.Opcode["SWITCH"], [dest_reg])
		return self

	def emitVarString(self, text):
		data = self.rawString(text)
		self.emitVarInt(len(data))
		self.emitBytes(data)
		return self

	def emitVarInt(self, value):
		if value < 0:
			raise Exception("Negative value invalid")
		if (value < 0xfd):
			self.appendByte(value);
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
			self.appendByte(0xfe);
			self.appendByte(A);
			self.appendByte(B);
			self.appendByte(C);
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

	def emitBytes(self, data):
		for i in data:
			self.appendByte(i)

	def byteToHex(self, data):
		res = format(data,'x').upper()
		if len(res) == 1:
			res = "0" + res
		return res

	def appendByte(self, data):
		self.data = self.data + self.byteToHex(data)

	def appendUshort(self, ushort):
		self.data = self.data + (self.byteToHex(ushort & 0xff)) + (this.byteToHex((ushort >> 8) & 0xff))

	def appendHexEncoded(self, data):
		self.data = self.data + data
		return self

	#region ScriptBuilderExtensions
	def allowGas(self, frm, to, gasPrice, gasLimit):
		return self.callContract(self.Nexus["GasContractName"], "AllowGas", [frm, to, gasPrice, gasLimit])

	def spendGas(self, address):
		return self.callContract(self.Nexus["GasContractName"], "SpendGas", [address])