#pragma once

//--------------------------------------------------------------
// Read the "Secure Memory" disclaimer in SecureMemory.h!
//--------------------------------------------------------------

#include "SecureVector.h"

namespace phantasma {

// This is a wrapper around the SecureVector type to make it appear similar to the String type.
// Use this when storing passwords, WIF's or other sensitive information in memory.
class SecureString
{
public:
	SecureString() 
	{
	}
	SecureString(const SecureString& o)
		: data(o.data)
	{
	}
	typedef SecureVector<Char>::size_type size_type;
	explicit SecureString(const Char* source, size_type length=0)
	{
		if( !source )
			return;
		if( length <= 0 )
			length = (size_type)PHANTASMA_STRLEN(source);
		if( length > 0 )
		{
			UInt32 size = (UInt32)length + 1;
			data.resize(size);
			PHANTASMA_COPY(source, source+size, &data.front());
			data.back() = '\0';
		}
	}
	explicit SecureString(const String& o)
	{
		const Char* source = o.c_str();
		UInt32 size = (UInt32)(o.length() + 1);
		data.resize(size);
		PHANTASMA_COPY(source, source+size, &data.front());
	}

	bool empty() const { return data.empty() || data.front() == '\0'; }

	Char  operator[](int i) const { return data[i]; }

	const Char* c_str() const
	{
		if( data.empty() )
			return PHANTASMA_LITERAL("");
		return &data.front();
	}

	UInt32 length() const { return data.empty() ? 0 : (UInt32)data.size() - 1; }

	void resize(unsigned len) { data.resize(len+1); data[len] = '\0'; }
	Char* begin() { return data.empty() ? 0 : &data.front(); }

private:
	SecureVector<Char> data;
};

}
