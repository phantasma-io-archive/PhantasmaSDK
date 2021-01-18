import re


class EventDecoder():
    '''
    The EventDecoder object implements all the basic methods needed to decode the data related to 
    Phantasma blockchain events.

    Args:
        data (str): encoded event data.

    Attributes:
        data (str): encoded event data.
    '''

    data: str = None

    def __init__(self, data: str) -> None:
        self.data = data

    def readCharPair(self) -> str:
        '''This method reads and returns a pair of characters from the encoded data and updates the object data
        with the remainder characters.

        Args:
            None

        Returns:
            readCharPair (str): char pair data read
        '''
        res = self.data[0:2]
        self.data = self.data[2:]
        return res

    def readByte(self) -> int:
        '''This method reads a byte from the encoded data.

        Args:
            None

        Returns:
            readByte (int): byte data read.
        '''
        b = self.readCharPair()
        return int(b, 16)

    def read(self, numBytes: int) -> str:
        '''This method reads and returns the number of numBytes from encoded data and updates the object data
        with the remainder characters

        Args:
            numBytes (int): number of bytes to be read.

        Returns:
            read (int): string data read.
        '''

        b = (numBytes * 2)
        res = self.data[0:b]
        self.data = self.data[b:]
        return res

    def readString(self) -> str:
        '''This method reads a str value from encoded data.

        Args:
            None

        Returns:
            readString (str): string data read.
        '''

        le = self.readVarInt()
        return self.readStringBytes(le)

    def readStringBytes(self, numBytes: int) -> str:
        '''This method reads a str bytes from encoded data.

        Args:
            numBytes (int): number of bytes to be read.

        Returns:
            readString (str): string data read.
        '''

        res = ""
        for i in range(numBytes):
            res = res + chr(self.readByte())
        return res

    def readVarInt(self) -> int:
        '''This method reads int value from the encoded data.

        Args:
            None

        Returns:
            readVarInt (int): int data read.
        '''

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

    def readBigIntAccurate(self) -> int:
        '''This method reads int value from the encoded data.

        Args:
            None

        Returns:
            readVarInt (int): int data read.
        '''
        le = self.readVarInt()
        res = 0
        stringBytes = self.read(le)
        d = re.findall('.{1,2}', stringBytes)
        for c in reversed(d):
            b = int(c, 16)
            res = (res * 256 + b)
        return res
