#pragma once

#include "../Numerics/Base16.h"
#include "../Numerics/BigInteger.h"
#include "../utils/Serializable.h"
#include "SHA.h"

namespace phantasma
{

class Hash : public Serializable
{
	Byte m_data[32];
public:
	constexpr static int Length = 32;

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
		String result = String(PHANTASMA_LITERAL("0x"));
		ByteArray temp;
		temp.resize(Length);
		PHANTASMA_COPY(m_data, m_data+Length, &temp.front());
		ArrayReverse(temp);
		result.append(Base16::Encode(temp));
		return result;
	}


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
		if(s.length() < 2 )
		{
			PHANTASMA_EXCEPTION("string cannot be empty");
			return Hash();
		}

		auto bytes = Base16::Decode(s);
		if(bytes.size() != Length)
		{
			PHANTASMA_EXCEPTION("length of string must be 64 hex chars");
			return Hash();
		}

		ArrayReverse(bytes);
		return Hash(bytes);
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
		if (sLength > 2 && s[0] == '0' && s[1] == 'x')
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
			auto data = Base16::Decode(s);
			ArrayReverse(data);
			result = Hash(data);
			return true;
		}
		PHANTASMA_CATCH(...)
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
		auto src = val.ToSignedByteArray();
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
		return BigInteger(ToByteArray(), Length);
	}

	static Hash MerkleCombine(const Hash& A, const Hash& B)
	{
		Byte bytes[Length * 2];
		PHANTASMA_COPY(A.m_data, A.m_data+Length, bytes);
		PHANTASMA_COPY(B.m_data, B.m_data+Length, bytes+Length);
		return FromBytes(bytes, Length * 2);
	}
};

}
