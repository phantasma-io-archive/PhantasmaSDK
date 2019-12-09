#pragma once

#include "TextUtils.h"
#include "../Numerics/BigInteger.h"

namespace phantasma {
class Hash;
class Address;
class Signature;

class BinaryReader
{
	const ByteArray& stream;
	UInt32 cursor;
public:
	BinaryReader(const ByteArray& stream, int cursor = 0)
		: stream(stream)
		, cursor((UInt32)cursor)
	{
	}

	UInt32 Position() const { return cursor; }

	const ByteArray& ToArray() { return stream; }

	Byte ReadByte() 
	{
		Byte b = 0;
		Read(b);
		return b;
	}
	void Read(uint8_t& b) 
	{
		if(cursor >= stream.size())
			PHANTASMA_EXCEPTION("stream error");
		else
			b = (uint8_t)stream[cursor++];
	}
	void Read( int8_t& b)
	{
		if(cursor >= stream.size())
			PHANTASMA_EXCEPTION("stream error");
		else
			b = (int8_t)stream[cursor++];
	}
	void Read(uint16_t& b)
	{
		uint8_t b0, b1;
		Read(b0);
		Read(b1);
		b = ((uint16_t)b0) | (((uint16_t)b1) << 8);
	}
	void Read( int16_t& b)
	{
		Read((uint16_t&)b);
	}
	void Read(uint32_t& b)
	{
		uint8_t b0, b1, b2, b3;
		Read(b0);
		Read(b1);
		Read(b2);
		Read(b3);
		b =  ((uint32_t)b0) | 
			(((uint32_t)b1) <<  8) |
			(((uint32_t)b2) << 16) |
			(((uint32_t)b3) << 24);
	}
	void Read( int32_t& b)
	{
		Read((uint32_t&)b);
	}
	void Read(uint64_t& b)
	{
		uint32_t i0, i1;
		Read(i0);
		Read(i1);
		b = ((uint64_t)i0) | (((uint64_t)i1) << 32);
	}
	void Read( int64_t& b)
	{
		Read((uint64_t&)b);
	}
	
	void Read(Byte* b, int size)
	{
		UInt32 end = (UInt32)stream.size();
		for( int i=0; i<size; ++i )
		{
			if(cursor >= end)
			{
				PHANTASMA_EXCEPTION("stream error");
				break;
			}
			b[i] = stream[cursor++];
		}
	}
	
	void Read( ByteArray& bytes, int size )
	{
		bytes.resize(size);
		if(size) 
			Read(&bytes.front(), size);
	}
	
	void ReadVarInt(Int64& output)
	{
		Byte header;
		Read(header);
		switch(header)
		{
			case 0xFD:
			{
				uint16_t value;
				Read(value);
				output = value;
				return;
			}
			case 0xFE:
			{
				UInt32 value;
				Read(value);
				output = value;
				return;
			}
			case 0xFF:
			{
				Int64 value;
				Read(value);
				output = value;
				return;
			}
			default:
			{
				output = header;
				return;
			}
		}
	}

	void ReadBigInteger(BigInteger& n)
	{
		Byte size = 0;
		ByteArray bytes;
		Read(size);
		Read(bytes, size);
		n = BigInteger::FromSignedArray(bytes);
	}
	
	void ReadByteArray(ByteArray& bytes) 
	{
		Int64 numBytes = 0;
		ReadVarInt(numBytes);
		if (numBytes == 0)
			bytes.resize(0);
		else
			Read(bytes, (int)numBytes);
	}
	int ReadByteArray(Byte* bytes, int maxToRead)
	{
		Int64 numBytes = 0;
		ReadVarInt(numBytes);
		if( numBytes )
		{
			if(numBytes > maxToRead)
			{
				PHANTASMA_EXCEPTION("Unexpected byte array size");
				//todo - error signal for non-exception builds
			}
			Read(bytes, PHANTASMA_MIN((int)numBytes, maxToRead));
		}
		return (int)numBytes;
	}
	template<int N> 
	void ReadByteArray( Byte(&bytes)[N] ) 
	{
		int read = ReadByteArray( bytes, N );
		if(read != N)
		{
			PHANTASMA_EXCEPTION("Unexpected byte array size");
			//todo - error signal for non-exception builds
		}
	}
	
	void ReadVarString(String& text)
	{
		Int64 numBytes = 0;
		ByteArray bytes;
		ReadVarInt(numBytes);
		if (numBytes== 0)
		{
			text = String{};
			return;
		}
		Read(bytes, (int)numBytes);
		text = FromUTF8Bytes(bytes);
	}

	void ReadAddress(Address& address)
	{
		ReadSerializable(address);
	}
	void ReadHash(Hash& hash)
	{
		ReadSerializable(hash);
	}
	void ReadSignature(Signature& hash)
	{
		ReadSerializable(hash);
	}
	template<class T, typename std::enable_if<std::is_base_of<Serializable, T>::value>::type* = nullptr>
	void ReadSerializable(T& s)
	{
		s.UnserializeData(*this);
	}
};

}