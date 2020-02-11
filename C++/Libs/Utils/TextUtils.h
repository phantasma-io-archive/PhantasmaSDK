#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#if !defined(PHANTASMA_CONVERT_TO_UTF8) && defined _UNICODE
#include <cwchar>
#endif

namespace phantasma {

bool StringStartsWith(const String& str, const Char* prefix, int prefixLength=-1)
{
	if( prefixLength == -1 )
		prefixLength = (int)PHANTASMA_STRLEN(prefix);
	if( (int)str.length() < prefixLength )
		return false;
	return PHANTASMA_EQUAL(prefix, prefix+prefixLength, str.c_str());
}

#ifdef PHANTASMA_CONVERT_TO_UTF8
//User-provided conversion function
template<class ByteArray>
const Byte* GetUTF8Bytes( const String& string, ByteArray& temp, int& out_numBytes )
{
	return PHANTASMA_CONVERT_TO_UTF8(string.c_str(), string.length(), temp, out_numBytes);
}
template<class ByteArray>
const Byte* GetUTF8Bytes( const Char* sz, int length, ByteArray& temp, int& out_numBytes )
{
	return PHANTASMA_CONVERT_TO_UTF8(sz, length, temp, out_numBytes);
}
inline String FromUTF8Bytes( const ByteArray& bytes )
{
	return PHANTASMA_CONVERT_FROM_UTF8(bytes);
}
inline String FromUTF8( const char* bytes )
{
	return PHANTASMA_CONVERT_FROM_UTF8(bytes);
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

inline String FromUTF8( const char* src )
{
	if( !src || *src == '\0' )
		return String{};
#ifdef _MSC_VER
	size_t requiredSize=0;
	mbstate_t state = {};
	if( 0 != mbsrtowcs_s( &requiredSize, 0, 0, &src, 0, &state) || requiredSize == 0 )
		return String{};
	ByteArray temp(requiredSize*2+2);
	wchar_t* dst = (wchar_t*)&temp.front();
	mbsrtowcs_s( &requiredSize, dst, requiredSize, &src, requiredSize, &state);
	temp[requiredSize*2-1] = 0;
	temp[requiredSize*2-2] = 0;
#else
	std::mbstate_t state = {};
	size_t requiredSize = 1 + std::mbsrtowcs( 0, &src, 0, &state);
	if( !requiredSize )//mbsrtowcs returns size_t(-1) on error
		return {};
	ByteArray temp(requiredSize*2);
	wchar_t* dst = (wchar_t*)&temp.front();
	std::mbsrtowcs( dst, &src, requiredSize, &state);
	temp[requiredSize*2-1] = 0;
	temp[requiredSize*2-2] = 0;
#endif
	return String{ dst };
}
inline String FromUTF8Bytes( const ByteArray& bytes )
{
	if( bytes.empty() )
		return {};
	if(bytes.back() != '\0')
	{
		ByteArray copy = bytes;
		copy.push_back((Byte)'\0');
		return FromUTF8Bytes(copy);
	}
	const char* src = (char*)&bytes.front();
	return FromUTF8(src);
}
#else
//Assume that the character encoding already is UTF-8
template<class ByteArray>
const Byte* GetUTF8Bytes( const String& string, ByteArray& temp, int& out_numBytes )
{
	out_numBytes = (int)string.length();
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

inline String FromUTF8( const char* bytes )
{
	if( !bytes || *bytes == '\0' )
		return String{};
	return String{bytes};
}
inline String FromUTF8Bytes( const ByteArray& bytes )
{
	if( bytes.empty() )
		return String{};
	if(bytes.back() == '\0')
		return String{(Char*)&bytes.front()};
	ByteArray copy = bytes;
	copy.push_back((Byte)'\0');
	return String{(Char*)&copy.front()};
}
#endif

}
