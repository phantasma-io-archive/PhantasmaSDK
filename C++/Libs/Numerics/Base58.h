#pragma once
#include "BigInteger.h"
#include "../Security/SecureVector.h"
#include "../Security/SecureByteArray.h"
#include "../Utils/ByteArrayUtils.h"
#include "../Cryptography/SHA.h"

namespace phantasma {
namespace Base58 {

constexpr Char Alphabet[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
inline int AlphabetIndexOf( Char in )
{
	for( const Char* c = Alphabet; *c; ++c )
		if( *c == in )
			return (int)(c - Alphabet);
	return -1;
}

inline PHANTASMA_VECTOR<Byte> Decode(const String& input)
{
	PHANTASMA_VECTOR<Byte> tmp;
	if(input.empty())
	{
		return tmp;
	}

	BigInteger bi = BigInteger::Zero();
	for (int i = (int)input.length() - 1; i >= 0; i--)
	{
		int index = AlphabetIndexOf(input[i]);
		if(index < 0)
		{
			PHANTASMA_EXCEPTION("invalid character");
			return tmp;
		}

		bi += BigInteger(index) * BigInteger::Pow(58, (int)input.length() - 1 - i);
	}

	PHANTASMA_VECTOR<Byte> bytes = bi.ToByteArray();
	ArrayReverse(bytes);

	int leadingZeros = 0;
	for (int i = 0; i < (int)input.length() && input[i] == Alphabet[0]; i++)
	{
		leadingZeros++;
	}

	tmp.resize(bytes.size() + leadingZeros);
	PHANTASMA_COPY(bytes.begin(), bytes.end(), tmp.begin() + leadingZeros);
	return tmp;
}

inline PHANTASMA_VECTOR<Byte> CheckDecode(const String& input)
{
	PHANTASMA_VECTOR<Byte> buffer = Decode(input);
	if (buffer.size() < 4)
	{
		PHANTASMA_EXCEPTION("Bad format");
		return PHANTASMA_VECTOR<Byte>{};
	}

	Byte expected_checksum[32];
	Byte expected_checksum_first[32];
	SHA256( expected_checksum_first, 32,  &buffer.front(), (int)buffer.size() - 4 );
	SHA256( expected_checksum, 32, expected_checksum_first, 32 );
	
	const Byte* src_checksum = &buffer.front() + buffer.size() - 4;

	if(!PHANTASMA_EQUAL( src_checksum, src_checksum+4, expected_checksum ))
	{
		PHANTASMA_EXCEPTION("WIF checksum failed");
		return PHANTASMA_VECTOR<Byte>{};
	}
	buffer.resize(buffer.size() - 4);
	return buffer;
}

inline int DecodeSecure(Byte* output, int outputSize, const String& input)//todo - secure string
{
	if((!output && outputSize > 0) || outputSize < 0)
	{
		PHANTASMA_EXCEPTION("invalid argument");
		return 0;
	}
	if(input.empty())
		return 0;

	SecureBigInteger bi = SecureBigInteger::Zero();
	for (int i = (int)input.length() - 1; i >= 0; i--)
	{
		int index = AlphabetIndexOf(input[i]);
		if(index < 0)
		{
			PHANTASMA_EXCEPTION("invalid character");
			return 0;
		}

		bi += SecureBigInteger(index) * SecureBigInteger::Pow(58, (int)input.length() - 1 - i);
	}

	int numBytes = bi.ToByteArray(0, 0);
	SecureByteArray byteAllocation(numBytes, 0, false);
	const auto& byteWriter = byteAllocation.Write();
	Byte* bytes = byteWriter.Bytes();

	bi.ToByteArray(bytes, numBytes);

	ArrayReverse(bytes, numBytes);

	int leadingZeros = 0;
	for (int i = 0; i < (int)input.length() && input[i] == Alphabet[0]; i++)
	{
		if( leadingZeros < outputSize )
			output[leadingZeros] = 0;
		leadingZeros++;
	}

	int resultSize = numBytes + leadingZeros;

	int canWrite = PHANTASMA_MIN( outputSize - leadingZeros, numBytes );
	if( canWrite > 0 )
	{
		PHANTASMA_COPY(bytes, bytes + canWrite, output + leadingZeros);
	}

	return resultSize;
}
inline int CheckDecodeSecure(Byte* output, int outputSize, const String& input)//todo - secure string input
{
	if( outputSize < 0 )
	{
		PHANTASMA_EXCEPTION("Invalid argument");
		return 0;
	}
	int bufferSize = outputSize + 4;
	SecureByteArray bufferAlloc(bufferSize, 0, false);
	const auto& bufferAccess = bufferAlloc.Write();
	Byte* buffer = bufferAccess.Bytes();
	int decodedSize = DecodeSecure(buffer, bufferSize, input);
	if (decodedSize < 4)
	{
		PHANTASMA_EXCEPTION("Bad format");
		return 0;
	}
	if(decodedSize > bufferSize)
	{
		PHANTASMA_EXCEPTION("Insufficient buffer size");
		return -decodedSize;
	}

	Byte expected_checksum[32];
	Byte expected_checksum_first[32];
	SHA256( expected_checksum_first, 32, buffer, decodedSize - 4 );
	SHA256( expected_checksum, 32, expected_checksum_first, 32 );

	const Byte* src_checksum = buffer + decodedSize - 4;

	if(!PHANTASMA_EQUAL( src_checksum, src_checksum+4, expected_checksum ))
	{
		PHANTASMA_EXCEPTION("WIF checksum failed");
		return 0;
	}
	int resultSize = decodedSize - 4;
	int canWrite = PHANTASMA_MIN( outputSize, resultSize );
	if( canWrite > 0 )
	{
		PHANTASMA_COPY(buffer, buffer + canWrite, output);
	}
	return resultSize;
}

inline String Encode(const Byte* input, int length)
{
	if( length == 0 )
		return String();

	PHANTASMA_VECTOR<Byte> temp;
	temp.resize(length + 1);
	for (int i=0; i<length; i++)
	{
		temp[i] = input[(length - 1) - i];
	}
	temp[length] = 0;

	BigInteger value(temp);
	PHANTASMA_WIPEMEM(&temp.front(), (int)temp.size());
	PHANTASMA_VECTOR<Char> sb;
	while (value >= 58)
	{
		BigInteger mod = value % 58;
		sb.push_back(Alphabet[(int)mod]);
		value /= 58;
	}
	sb.push_back(Alphabet[(int)value]);

	for(int i=0; i<length; ++i)
	{
		if( input[i] == 0 )
			sb.push_back(Alphabet[0]);
		else
			break;
	}
	ArrayReverse(sb);

	String result(&sb.front(), sb.size());
	PHANTASMA_WIPEMEM(&sb.front(), sb.size());
	return result;
}

}}
