import re

class EventDecoder():
	data = None 

	def __init__(self, data):
		self.data = data

	def readCharPair(self):
		res = self.data[0:2]
		self.data = self.data[2:]
		return res

	def readByte(self):

		b = self.readCharPair()
		return int(b,16)

	def read(self, numBytes):
		b = (numBytes * 2)
		res = self.data[0:b]
		self.data = self.data[b:]
		return res
	
	def readString(self):
		le = self.readVarInt()
		return self.readStringBytes(le)

	def readStringBytes(self, numBytes):
		res = "";
		for i in range(numBytes):
			res = res + chr(self.readByte())
		return res

	def readVarInt(self):
		le = self.readByte()
		res = 0
		if (le == 0xfd):
			d = re.findall('.{1,2}', self.read(2))
			for c in reversed(d):
				res = (res * 256 + int(c, 16))
			return res
		if (le == 0xfe):
			d = re.findall('.{1,2}', self.read(4))
			for c in reversed(d):
				res = (res * 256 + int(c, 16))
			return res
		if (le == 0xff):
			d = re.findall('.{1,2}', self.read(8))
			for c in reversed(d):
				res = (res * 256 + int(c, 16))
			return res
		return le

	def readBigIntAccurate(self):
		le = self.readVarInt()
		res = 0
		stringBytes = self.read(le)
		d = re.findall('.{1,2}', stringBytes)
		for c in reversed(d):
			b = int(c, 16)
			res = (res * 256 + b)
		return res