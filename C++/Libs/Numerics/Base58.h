#pragma once
#include "BigInteger.h"
#include "../Security/SecureByteArray.h"
#include "../Security/SecureString.h"
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

inline int Decode(Byte* output, int outputLength, const Char* input, int inputLength)
{
	if(!input || inputLength < 0 || outputLength < 0)
	{
		PHANTASMA_EXCEPTION("Invalid usage");
		return -1;
	}
	if(inputLength == 0)
	{
		inputLength = (int)PHANTASMA_STRLEN(input);
		if( inputLength == 0 )
			return 0;
	}

	BigInteger bi = BigInteger::Zero();
	for (int i = inputLength - 1; i >= 0; i--)
	{
		int index = AlphabetIndexOf(input[i]);
		if(index < 0)
		{
			PHANTASMA_EXCEPTION("invalid character");
			return -1;
		}

		bi += BigInteger(index) * BigInteger::Pow(58, inputLength - 1 - i);
	}

	int leadingZeros = 0;
	for (int i = 0; i < inputLength && input[i] == Alphabet[0]; i++)
	{
		leadingZeros++;
	}

	int bigIntBytes = bi.ToUnsignedByteArray(0, 0);
	int bytesRequired = bigIntBytes + leadingZeros;
	if( !output )
		return bytesRequired;
	if( bytesRequired > outputLength )
		return -1;

	for( int i=0; i<leadingZeros; ++i )
		output[i] = 0;

	bi.ToUnsignedByteArray(output+leadingZeros, outputLength-leadingZeros);
	ArrayReverse(output+leadingZeros, bigIntBytes);
	return bytesRequired;
}

inline ByteArray Decode(const String& input)
{
	ByteArray tmp;
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

	ByteArray bytes = bi.ToUnsignedByteArray();
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

inline ByteArray CheckDecode(const String& input)
{
	ByteArray buffer = Decode(input);
	if (buffer.size() < 4)
	{
		PHANTASMA_EXCEPTION("Bad format");
		return ByteArray{};
	}

	Byte expected_checksum[32];
	Byte expected_checksum_first[32];
	SHA256( expected_checksum_first, 32,  &buffer.front(), (int)buffer.size() - 4 );
	SHA256( expected_checksum, 32, expected_checksum_first, 32 );
	
	const Byte* src_checksum = &buffer.front() + buffer.size() - 4;

	if(!PHANTASMA_EQUAL( src_checksum, src_checksum+4, expected_checksum ))
	{
		PHANTASMA_EXCEPTION("WIF checksum failed");
		return ByteArray{};
	}
	buffer.resize(buffer.size() - 4);
	return buffer;
}

inline int DecodeSecure(Byte* output, int outputSize, const Char* input, int inputLength)
{
	if((!output && outputSize > 0) || outputSize < 0 || inputLength < 0)
	{
		PHANTASMA_EXCEPTION("invalid argument");
		return 0;
	}
	if(!input || input[0] == '\0')
		return 0;

	SecureBigInteger bi = SecureBigInteger::Zero();
	for (int i = inputLength - 1; i >= 0; i--)
	{
		int index = AlphabetIndexOf(input[i]);
		if(index < 0)
		{
			PHANTASMA_EXCEPTION("invalid character");
			return 0;
		}

		bi += SecureBigInteger(index) * SecureBigInteger::Pow(58, inputLength - 1 - i);
	}

	int numBytes = bi.ToUnsignedByteArray(0, 0);
	SecureByteArray byteAllocation(numBytes, 0, false);
	const auto& byteWriter = byteAllocation.Write();
	Byte* bytes = byteWriter.Bytes();

	bi.ToUnsignedByteArray(bytes, numBytes);

	ArrayReverse(bytes, numBytes);

	int leadingZeros = 0;
	for (int i = 0; i < inputLength && input[i] == Alphabet[0]; i++)
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
inline int CheckDecodeSecure(Byte* output, int outputSize, const Char* input, int inputLength)
{
	if( outputSize < 0 || inputLength < 0 )
	{
		PHANTASMA_EXCEPTION("Invalid argument");
		return 0;
	}
	int bufferSize = outputSize + 4;
	SecureByteArray bufferAlloc(bufferSize, 0, false);
	const auto& bufferAccess = bufferAlloc.Write();
	Byte* buffer = bufferAccess.Bytes();
	int decodedSize = DecodeSecure(buffer, bufferSize, input, inputLength);
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

template<class String, class ByteArray, class BigInteger, class CharArray>
String TEncode(const Byte* input, int length)
{
	if( length <= 0 )
		return String();

	ByteArray temp;
	temp.resize(length + 1);
	for (int i=0; i<length; i++)
	{
		temp[i] = input[(length - 1) - i];
	}
	temp[length] = 0;

	BigInteger value(temp);
	CharArray sb;
	sb.reserve(length);
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

	return String{ &sb.front(), (typename String::size_type)sb.size() };
}

inline String Encode(const Byte* input, int length)
{
	return TEncode<String, ByteArray, BigInteger, PHANTASMA_VECTOR<Char>>(input, length);
}

inline SecureString EncodeSecure(const Byte* input, int length)
{
	return TEncode<SecureString, SecureVector<Byte>, SecureBigInteger, SecureVector<Char>>(input, length);
}

template<class String, class ByteArray, class BigInteger, class CharArray>
String TCheckEncode(const Byte* input, int length)
{
	if( length <= 0 )
		return String();
	Byte checksum1[PHANTASMA_SHA256_LENGTH];
	Byte checksum2[PHANTASMA_SHA256_LENGTH];
	SHA256(checksum1, PHANTASMA_SHA256_LENGTH, input, length);
	SHA256(checksum2, PHANTASMA_SHA256_LENGTH, checksum1, PHANTASMA_SHA256_LENGTH);

	ByteArray buffer;
	buffer.resize(length + 4);
	PHANTASMA_COPY(input, input+length, &buffer[0]);
	PHANTASMA_COPY(checksum2, checksum2+4, &buffer[length]);

	return TEncode<String, ByteArray, BigInteger, CharArray>(&buffer.front(), (int)buffer.size());
}

inline String CheckEncode(const Byte* input, int length)
{
	return TCheckEncode<String, ByteArray, BigInteger, PHANTASMA_VECTOR<Char>>(input, length);
}
inline SecureString CheckEncodeSecure(const Byte* input, int length)
{
	return TCheckEncode<SecureString, SecureVector<Byte>, SecureBigInteger, SecureVector<Char>>(input, length);
}

}}
