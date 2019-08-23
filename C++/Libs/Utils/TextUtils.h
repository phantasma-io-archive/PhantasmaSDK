#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#if !defined(PHANTASMA_CONVERT_UTF8) && defined _UNICODE
#include <cwchar>
#endif

namespace phantasma {

typedef PHANTASMA_VECTOR<Byte> ByteBuffer;

#ifdef PHANTASMA_CONVERT_UTF8
const Byte* GetUTF8Bytes( const String& string, ByteBuffer& temp, int& out_numBytes )
{
	return PHANTASMA_CONVERT_UTF8(string, temp, out_numBytes);
}
#elif defined _UNICODE
const Byte* GetUTF8Bytes( const String& string, ByteBuffer& temp, int& out_numBytes )
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
const Byte* GetUTF8Bytes( const Char* string, ByteBuffer& temp, int& out_numBytes )
{
	if( !string )
		return 0;
#ifdef _MSC_VER
	size_t requiredSize=0;
	mbstate_t state = {};
	if( 0 != wcsrtombs_s( &requiredSize, 0, 0, &string, 0, &state) || requiredSize == 0 )
		return (Byte*)"";
	temp.resize(requiredSize+1);
	Byte* result = &temp.front();
	wcsrtombs_s( &requiredSize, (char*)result, requiredSize, &string, requiredSize, &state);
#else
	std::mbstate_t state = {};
	size_t requiredSize = 1 + std::wcsrtombs(0, &string, 0, &state);
	if( !requiredSize )//wcsrtombs returns size_t(-1) on error
		return (Byte*)"";
	temp.resize(requiredSize);
	Byte* result = &temp.front();
	std::wcsrtombs((char*)result, &string, requiredSize, &state);
#endif
	return result;
}
#else
const Byte* GetUTF8Bytes( const String& string, ByteBuffer& temp, int& out_numBytes )
{
	out_numBytes = string.length();
	return (Byte*)string.c_str();
}
const Byte* GetUTF8Bytes( const Char* sz, ByteBuffer& temp, int& out_numBytes )
{
	int length = 0;
	for( const Char* c = sz; *c != '\0'; ++c )
		++length;
	out_numBytes = length;
	return (Byte*)sz;
}
#endif

}
