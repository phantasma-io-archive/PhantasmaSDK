#pragma once

#include "TextUtils.h"
#include "Serializable.h"
#include "../Numerics/BigInteger.h"

namespace phantasma {
class Hash;
class Address;
class Signature;

class BinaryWriter
{
	ByteArray stream;
public:
	BinaryWriter(UInt32 sizeHint = 4096)
	{
		stream.reserve(sizeHint);
	}

	UInt32 Position() const { return (UInt32)stream.size(); }

	const ByteArray& ToArray() { return stream; }

	void Write(uint8_t b) 
	{
		stream.push_back((Byte)b);
	}
	void Write( int8_t b)
	{
		stream.push_back((Byte)b);
	}
	void Write(uint16_t b)
	{
		stream.push_back((Byte)( b       & 0xFF));
		stream.push_back((Byte)((b >> 8) & 0xFF));
	}
	void Write( int16_t b)
	{
		Write((uint16_t)b);
	}
	void Write(uint32_t b)
	{
		stream.push_back((Byte)( b        & 0xFF));
		stream.push_back((Byte)((b >>  8) & 0xFF));
		stream.push_back((Byte)((b >> 16) & 0xFF));
		stream.push_back((Byte)((b >> 24) & 0xFF));
	}
	void Write( int32_t b)
	{
		Write((uint32_t)b);
	}
	void Write(uint64_t b)
	{
		Write((uint32_t)( b        & 0xFFFFFFFFU));
		Write((uint32_t)((b >> 32) & 0xFFFFFFFFU));
	}
	void Write( int64_t b)
	{
		Write((uint64_t)b);
	}

	void Write(const Byte* b, int size)
	{
		for( int i=0; i<size; ++i )
			stream.push_back(b[i]);
	}
	void Write( const ByteArray& bytes )
	{
		if(!bytes.empty()) 
			Write(&bytes.front(), (int)bytes.size());
	}

	void WriteVarInt(Int64 value)
	{
		if (value < 0)
		{
			PHANTASMA_EXCEPTION("argument out of range");
			return;
		}
		if (value < 0xFD)
		{
			Write((Byte)value);
		}
		else if (value <= 0xFFFF)
		{
			Write((Byte)0xFD);
			Write((uint16_t)value);
		}
		else if (value <= 0xFFFFFFFF)
		{
			Write((Byte)0xFE);
			Write((UInt32)value);
		}
		else
		{
			Write((Byte)0xFF);
			Write(value);
		}
	}

	void WriteBigInteger(const BigInteger& n)
	{
		auto bytes = n.ToSignedByteArray();
		Write((Byte)bytes.size());
		Write(bytes);
	}

	void WriteByteArray(const Byte* bytes, int numBytes)
	{
		if (bytes == 0 || numBytes == 0)
		{
			WriteVarInt(0);
			return;
		}
		WriteVarInt(numBytes);
		Write(bytes, numBytes);
	}
	void WriteByteArray(const ByteArray& bytes) 
	{
		WriteByteArray( bytes.empty()?0:&bytes.front(), (int)bytes.size() );
	}
	template<int N> 
	void WriteByteArray( const Byte(&bytes)[N] ) 
	{
		WriteByteArray( bytes, N );
	}

	void WriteVarString(const String& text)
	{
		if (text.empty())
		{
			Write((Byte)0);
			return;
		}

		ByteArray temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( text, temp, numBytes );

		WriteVarInt(numBytes);
		Write(bytes, numBytes);
	}
	void WriteVarString(const Char* text)
	{
		if (!text || text[0] == '\0')
		{
			Write((Byte)0);
			return;
		}

		ByteArray temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( text, 0, temp, numBytes );

		WriteVarInt(numBytes);
		Write(bytes, numBytes);
	}


	template<class T, typename std::enable_if<std::is_base_of<Serializable, T>::value>::type* = nullptr>
	void WriteSerializable(const T& s)
	{
		s.SerializeData(*this);
	}
	template<class Address>
	void WriteAddress(const Address& address)
	{
		WriteSerializable(address);
	}
	template<class Hash>
	void WriteHash(const Hash& hash)
	{
		WriteSerializable(hash);
	}
	template<class Signature>
	void WriteSignature(const Signature& hash)
	{
		WriteSerializable(hash);
	}
};

}