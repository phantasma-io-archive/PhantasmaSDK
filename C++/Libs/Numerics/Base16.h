#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#include <ctype.h>
#include "../Utils/ByteArrayUtils.h"

namespace phantasma {
namespace Base16 {

constexpr Char Alphabet[] = "0123456789ABCDEF";
inline int AlphabetIndexOf( Char in )
{
	for( const Char* c = Alphabet; *c; ++c )
		if( *c == in )
			return (int)(c - Alphabet);
	return -1;
}

inline PHANTASMA_VECTOR<Byte> Decode(const String& input)
{
	if( input.empty() )
	{
		PHANTASMA_EXCEPTION("string cannot be empty");
		return PHANTASMA_VECTOR<Byte>{};
	}

	auto length = input.length();
	const Char* sz = input.c_str();

	if (length >= 2 && sz[0] == '0' && sz[1] == 'x')
	{
		sz += 2;
		length -= 2;
		if( length == 0 )
		{
			PHANTASMA_EXCEPTION("string cannot be empty");
			return PHANTASMA_VECTOR<Byte>{};
		}
	}

	if( length % 2 == 1 )
	{
		PHANTASMA_EXCEPTION("string length must be even");
		return PHANTASMA_VECTOR<Byte>{};
	}

	length /= 2;
	PHANTASMA_VECTOR<Byte> result;
	result.resize(length);
	for (int i = 0; i < length; i++)
	{
		int A = AlphabetIndexOf(toupper(input[i * 2 + 0]));
		int B = AlphabetIndexOf(toupper(input[i * 2 + 1]));

		if(A < 0 || B < 0)
		{
			PHANTASMA_EXCEPTION("invalid character");
			return PHANTASMA_VECTOR<Byte>{};
		}

		result[i] = (Byte)(A * 16 + B);
	}

	return result;
}

inline String Encode(const Byte* input, int length)
{
	if(!input || length <= 0)
	{
		PHANTASMA_EXCEPTION("invalid argument");
		return String();
	}

	PHANTASMA_VECTOR<Char> c;
	c.resize(length * 2 + 1);
	int b;
	for(int i = 0; i < length; i++)
	{
		b = input[i] >> 4;
		c[i * 2] = (Char)(55 + b + (((b - 10) >> 31) & -7));
		b = input[i] & 0xF;
		c[i * 2 + 1] = (Char)(55 + b + (((b - 10) >> 31) & -7));
	}
	c[c.size()-1] = '\0';

	return String(&c.front());
}

inline String Encode(const PHANTASMA_VECTOR<Byte>& input)
{
	if(input.empty())
	{
		PHANTASMA_EXCEPTION("invalid argument");
		return String();
	}
	return Encode(&input.front(), (int)input.size());
}

}}