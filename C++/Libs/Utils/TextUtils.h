#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#if !defined(PHANTASMA_CONVERT_UTF8) && defined _UNICODE
#include <cwchar>
#endif

namespace phantasma {

#ifdef PHANTASMA_CONVERT_UTF8
//User-provided conversion function
template<class ByteArray>
const Byte* GetUTF8Bytes( const String& string, ByteArray& temp, int& out_numBytes )
{
	return PHANTASMA_CONVERT_UTF8(string.c_str(), string.length(), temp, out_numBytes);
}
template<class ByteArray>
const Byte* GetUTF8Bytes( const Char* sz, int length, ByteArray& temp, int& out_numBytes )
{
	return PHANTASMA_CONVERT_UTF8(sz, length, temp, out_numBytes);
}
#elif defined _UNICODE
//Assume that the character encoding is UTF-16
template<class ByteArray>
const Byte* GetUTF8Bytes( const String& string, ByteArray& temp, int& out_numBytes )
{
	if( string.empty() )
		return (Byte*)"";
	const Char* wide = string.c_str();
#ifdef _MSC_VER
	size_t requiredSize=0;
	mbstate_t state = {};
	if( 0 != wcsrtombs_s( &requiredSize, 0, 0, &wide, 0, &state) || requiredSize == 0 )
		return (Byte*)"";
	temp.resize(requiredSize+1);
	Byte* result = &temp.front();
	wcsrtombs_s( &requiredSize, (char*)result, requiredSize, &wide, requiredSize, &state);
#else
	std::mbstate_t state = {};
	size_t requiredSize = 1 + std::wcsrtombs(0, &wide, 0, &state);
	if( !requiredSize )//wcsrtombs returns size_t(-1) on error
		return (Byte*)"";
	temp.resize(requiredSize);
	Byte* result = &temp.front();
	std::wcsrtombs((char*)result, &wide, requiredSize, &state);
#endif
	return result;
}
template<class ByteArray>
const Byte* GetUTF8Bytes( const Char* string, int length, ByteArray& temp, int& out_numBytes )
{
	if( !string )
		return 0;
	PHANTASMA_VECTOR<Char> truncated; 
	if( length > 0 )
	{
		int actualLength = (int)PHANTASMA_STRLEN(string);
		if( length < actualLength )
		{
			truncated.resize(length);
			for( int i=0; i!=length; ++i )
				truncated[i] = string[i];
			string = &truncated.front();
		}
	}
#ifdef _MSC_VER
	size_t requiredSize=0;
	mbstate_t state = {};
	if( 0 != wcsrtombs_s( &requiredSize, 0, 0, &string, 0, &state) || requiredSize == 0 )
		return (Byte*)"";
	temp.resize(requiredSize+1);
	Byte* result = &temp.front();
	wcsrtombs_s( &requiredSize, (char*)result, requiredSize, &string, requiredSize, &state);
	out_numBytes = (int)requiredSize;
#else
	std::mbstate_t state = {};
	size_t requiredSize = 1 + std::wcsrtombs(0, &string, 0, &state);
	if( !requiredSize )//wcsrtombs returns size_t(-1) on error
		return (Byte*)"";
	temp.resize(requiredSize);
	Byte* result = &temp.front();
	std::wcsrtombs((char*)result, &string, requiredSize, &state);
	out_numBytes = (int)requiredSize;
#endif
	return result;
}
#else
//Assume that the character encoding already is UTF-8
template<class ByteArray>
const Byte* GetUTF8Bytes( const String& string, ByteArray& temp, int& out_numBytes )
{
	out_numBytes = string.length();
	return (Byte*)string.c_str();
}
template<class ByteArray>
const Byte* GetUTF8Bytes( const Char* sz, int length, ByteArray& temp, int& out_numBytes )
{
	if( !sz || length < 0 )
		return 0;
	if( length == 0 )
	{
		for( const Char* c = sz; *c != '\0'; ++c )
			++length;
	}
	out_numBytes = length;
	return (Byte*)sz;
}
#endif

}
