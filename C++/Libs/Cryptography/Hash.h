#pragma once

#include "../Numerics/Base16.h"
#include "../Numerics/BigInteger.h"
#include "../utils/Serializable.h"
#include "../utils/TextUtils.h"
#include "SHA.h"

namespace phantasma
{

class Hash : public Serializable
{
	Byte m_data[32];
public:
	constexpr static int Length = 32;

	bool IsNull() const
	{
		for (int i=0; i<Length; i++)
		{
			if (m_data[i] != 0)
			{
				return false;
			}
		}
		return true;
	}

	bool operator==(const Hash& other) const
	{
		if( this == &other )
			return true;
		return PHANTASMA_EQUAL(m_data, m_data+32, other.m_data);
	}

	const Byte* ToByteArray() const
	{
		return m_data;
	}

	String ToString() const
	{
		ByteArray temp;
		temp.resize(Length);
		PHANTASMA_COPY(m_data, m_data+Length, &temp.front());
		ArrayReverse(temp);
		return Base16::Encode(temp);
	}

	static Hash Zero() { return Hash(); }

	Hash()
	{
		for( int i=0; i!=Length; ++i )
			m_data[i] = 0;
	}

	Hash(const Byte* value, int valueLength)
	{
		if(!value)
		{
			PHANTASMA_EXCEPTION("value cannot be null");
			for( int i=0; i!=Length; ++i )
				m_data[i] = 0;
		}
		else if(valueLength != Length)
		{
			PHANTASMA_EXCEPTION("value must have length 32");
			for( int i=0; i!=Length; ++i )
				m_data[i] = 0;
		}
		else
			PHANTASMA_COPY(value, value+Length, m_data);
	}
	Hash(const ByteArray& value)
		: Hash(value.empty()?0:&value.front(), (int)value.size())
	{
	}

	int CompareTo(const Hash& other) const
	{
		const Byte* x = m_data;
		const Byte* y = other.m_data;
		for(int i = Length - 1; i >= 0; i--)
		{
			if (x[i] > y[i])
				return 1;
			if (x[i] < y[i])
				return -1;
		}
		return 0;
	}

	static Hash Parse(const String& s)
	{
		return Parse(s.c_str(), (int)s.length());
	}
	static Hash Parse(const Char* s, int sLength=-1)
	{
		if(sLength < 0)
		{
			sLength = (int)PHANTASMA_STRLEN(s);
		}
		if(sLength == 0 )
		{
			PHANTASMA_EXCEPTION("string cannot be empty");
			return Hash();
		}
		if(sLength < 64 )
		{
			PHANTASMA_EXCEPTION("string too short");
			return Hash();
		}

		Char ch = s[1];
		if (ch == 'X' || ch == 'x')
		{
			if(s[0] != '0')
			{
				PHANTASMA_EXCEPTION("invalid hexdecimal prefix");
				return Hash();
			}
			return Parse(s+2, sLength-2);
		}

		constexpr int expectedLength = Length * 2;
		if(sLength != expectedLength)
		{
			PHANTASMA_EXCEPTION("length of string must be 64 hex chars");
			return Hash();
		}

		Byte decoded[Length];
		int decodedLength = Base16::Decode( decoded, Length, s, sLength );
		if(decodedLength != Length)
		{
			PHANTASMA_EXCEPTION("base16 decoding error");
			return Hash();
		}
		ArrayReverse(decoded, Length);
		return Hash(decoded, Length);
	}

	static bool TryParse(const String& s, Hash& result)
	{
		if (s.empty())
		{
			result = Hash();
			return false;
		}

		auto sLength = s.length();
		int expectedLength = Length*2;
		if (sLength > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		{
			expectedLength += 2;
		}

		if (sLength != expectedLength)
		{
			result = Hash();
			return false;
		}

		PHANTASMA_TRY
		{
			Byte decoded[Length];
			int decodedLength = Base16::Decode( decoded, Length, s.c_str(), (int)sLength );
			if(decodedLength != Length)
			{
				result = Hash();
				return false;
			}
			ArrayReverse(decoded, Length);
			result = Hash(decoded, Length);
			return true;
		}
		PHANTASMA_CATCH_ALL()
		{
			result = Hash();
			return false;
		}
	}

	bool operator !=(const Hash& right) const
	{
		return !(*this == right);
	}


	bool operator >(const Hash& right) const
	{
		return CompareTo(right) > 0;
	}

	bool operator >=(const Hash& right) const
	{
		return CompareTo(right) >= 0;
	}

	bool operator <(const Hash& right) const
	{
		return CompareTo(right) < 0;
	}

	bool operator <=(const Hash& right) const
	{
		return CompareTo(right) <= 0;
	}

	// If necessary pads the number to 32 bytes with zeros 
	Hash(const BigInteger& val)
	{
		//auto src = val.ToSignedByteArray();
		auto src = val.ToUnsignedByteArray();
		if(src.size() > Length)
		{
			PHANTASMA_EXCEPTION("number is too large");
			*this = Hash();
		}
		else
			*this = FromBytes(src);
	}

	static Hash FromBytes(const ByteArray& input)
	{
		if( input.empty() )
			return Hash();
		return FromBytes(&input.front(), (int)input.size());
	}
	static Hash FromBytes(const Byte* input, int inputLength)
	{
		if (inputLength != Length) // NOTE this is actually problematic, better to separate into 2 methods
		{
			Byte temp[32];
			SHA256(temp, 32, input, inputLength);
			return FromBytes( temp, 32 );
		}
		else
			return Hash(input, inputLength);
	}

	template<class BinaryWriter>
	void SerializeData(BinaryWriter& writer) const
	{
		writer.WriteByteArray(m_data, Length);
	}

	template<class BinaryReader>
	void UnserializeData(BinaryReader& reader)
	{
		reader.ReadByteArray(m_data, Length);
	}

	operator BigInteger() const
	{
		return BigInteger::FromUnsignedArray(ToByteArray(), true);
	}

	static Hash MerkleCombine(const Hash& A, const Hash& B)
	{
		Byte bytes[Length * 2];
		PHANTASMA_COPY(A.m_data, A.m_data+Length, bytes);
		PHANTASMA_COPY(B.m_data, B.m_data+Length, bytes+Length);
		return FromBytes(bytes, Length * 2);
	}

	static Hash FromString(const String& str)
	{
		ByteArray temp;
		int utf8Length = 0;
		const Byte* utf8 = GetUTF8Bytes(str, temp, utf8Length );
		Byte bytes[PHANTASMA_SHA256_LENGTH];
		SHA256( bytes, PHANTASMA_SHA256_LENGTH, utf8, utf8Length );
		return Hash(bytes, PHANTASMA_SHA256_LENGTH);
	}

	static Hash FromUnpaddedHex(const String& hash)
	{
		const Char* szHash = hash.c_str();
		if (hash.length() >= 2 && szHash[0] == '0' && (szHash[1] == 'x' || szHash[1] == 'X'))
		{
			szHash += 2;
		}

		StringBuilder sb;
		sb << szHash;
		while (sb.str().length() < 64)
		{
			sb << '0';
			sb << '0';
		}

		return Hash::Parse(sb.str());
	}

	//public static class PoWUtils
	int GetDifficulty() const
	{
		int result = 0;
		for (int i=0; i<Length; i++)
		{
			Byte n = m_data[i];

			for (int j=0; j<8; j++)
			{
				if ((n & (1 << j)) != 0)
				{
					result = 1 + (i << 3) + j;
				}
			}
		}

		return 256 - result;
	}
};

inline void BinaryReader::ReadHash(Hash& hash)
{
	ReadSerializable(hash);
}

}
