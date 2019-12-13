#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#include <ctype.h>

namespace phantasma {
namespace Base64 {

constexpr Char Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline int AlphabetIndexOf( Char in )
{
	for(const Char* c = Alphabet; *c; ++c)
		if(*c == in)
			return (int)(c - Alphabet);
	return -1;
}

constexpr inline int RequiredCharacters( int numBytes )//does not include a null terminator
{
	int numTriBytes = (numBytes + 2) / 3;
	return numTriBytes * 4;
}

inline int DecodeTriByte( Byte* output, const Char* input )//read 4 Chars, write 1-3 bytes
{
	int padding = 0;
	UInt32 triByte = 0;
	for( int i = 0; i != 4; ++i )
	{
		triByte <<= 6;
		if( input[i] == '=' )
			++padding;
		else 
		{
			int index = AlphabetIndexOf( input[i] );
			if( index < 0 )
				return 0;//invalid input
			triByte += index;
		}
	}
	if( padding < 1 ) output[2] = (Byte)(  triByte        & 0xFFU );
	if( padding < 2 ) output[1] = (Byte)( (triByte >> 8 ) & 0xFFU );
	                  output[0] = (Byte)( (triByte >> 16) & 0xFFU );
	return 3 - padding;
}

inline int Decode( Byte* output, int outputLength, const Char* input, int inputLength = 0 )
{
	if( inputLength == 0 )
	{
		inputLength = (int)PHANTASMA_STRLEN(input);
	}

	if( inputLength == 0 || (inputLength % 4) != 0 )
	{
		PHANTASMA_EXCEPTION("Invalid input");
		return -1;
	}

	int length, padding;
	for(length=0; length < inputLength && input[length] != '='; ++length)
	{}
	for( padding = 0; length < inputLength && input[length + padding] == '='; ++padding )
	{}

	if( padding > 2 || length + padding != inputLength )
	{
		PHANTASMA_EXCEPTION("Invalid input");
		return -1;
	}

	int numTriBytes = inputLength / 4;
	int requiredLength = (numTriBytes * 3) - padding;

	if( !output )
	{
		return requiredLength;
	}
	if( outputLength < requiredLength )
	{
		PHANTASMA_EXCEPTION("Invalid usage");
		return -1;
	}

	for( int i = 0, cursor = 0; i < numTriBytes; ++i )
	{
		int bytesDecoded = DecodeTriByte( output + cursor, input + i*4 );
		if(bytesDecoded <= 0) 
		{
			PHANTASMA_EXCEPTION("Invalid input");
			return -1;
		}
		cursor += bytesDecoded;
	}
	return 0;
}

inline ByteArray Decode( const Char* input, int inputLength = 0 )
{
	if( inputLength == 0 )
	{
		inputLength = (int)PHANTASMA_STRLEN(input);
	}
	int outputLength = Decode( 0, 0, input, inputLength );
	ByteArray result;
	if( outputLength > 0 )
	{
		result.resize(outputLength);
		Decode( &result.front(), outputLength, input, inputLength );
	}
	return result;
}

inline ByteArray Decode( const String& input )
{
	return Decode( input.c_str(), input.length() );
}

inline int Encode( Char* output, int outputSize, const Byte* input, int inputLength )
{
	int requiredChars = RequiredCharacters( inputLength );
	if(!output)
		return requiredChars+1;
	if(!input || inputLength <= 0 || outputSize < requiredChars+1)
	{
		PHANTASMA_EXCEPTION( "invalid argument" );
		return -1;
	}

	Char* out = output;
	for(int i = 0; i < inputLength; )
	{
		UInt32 b0 =                   input[i++];
		UInt32 b1 = i < inputLength ? input[i++] : 0;
		UInt32 b2 = i < inputLength ? input[i++] : 0;
		UInt32 triByte = (b0 << 16) | (b1 << 8) | b2;
		*out++ = Alphabet[(triByte >> 3 * 6) & 0x3F];
		*out++ = Alphabet[(triByte >> 2 * 6) & 0x3F];
		*out++ = Alphabet[(triByte >> 1 * 6) & 0x3F];
		*out++ = Alphabet[(triByte >> 0 * 6) & 0x3F];
	}

	int padding = (3 - ((inputLength + 2) % 3)) - 1;
	for(int i = 0; i < padding; i++)
		output[(requiredChars - 1) - i] = '=';

	output[requiredChars] = '\0';
	return 0;
}

inline String Encode( const Byte* input, int length )
{
	if(!input || length <= 0)
	{
		PHANTASMA_EXCEPTION( "invalid argument" );
		return String();
	}

	int requiredBuffer = Encode( 0, 0, input, length );
	PHANTASMA_VECTOR<Char> c;
	c.resize( requiredBuffer );
	Encode( &c.front(), requiredBuffer, input, length );
	return String( &c.front() );
}

inline String Encode( const ByteArray& input )
{
	return Encode( input.empty() ? 0 : &input.front(), (int)input.size() );
}

}
}