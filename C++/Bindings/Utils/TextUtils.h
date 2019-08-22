#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
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
	//todo - wide char to UTF8 conversion....
}
const Byte* GetUTF8Bytes( const Char* string, ByteBuffer& temp, int& out_numBytes )
{
	//todo - wide char to UTF8 conversion....
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
