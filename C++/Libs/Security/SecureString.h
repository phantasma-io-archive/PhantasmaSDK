#pragma once

#include "SecureVector.h"

namespace phantasma {

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
	SecureString(const Char* source, size_type length)
	{
		if( source && length > 0 )
		{
			UInt32 size = (UInt32)length + 1;
			data.resize(size);
			PHANTASMA_COPY(source, source+size, &data.front());
			data.back() = '\0';
		}
	}
	SecureString(const String& o)
	{
		const Char* source = o.c_str();
		UInt32 size = (UInt32)(o.length() + 1);
		data.resize(size);
		PHANTASMA_COPY(source, source+size, &data.front());
	}

	bool empty() const { return data.empty() || data.front() == '\0'; }

	Char operator[](int i) const { return data[i]; }

	const Char* c_str() const
	{
		if( data.empty() )
			return PHANTASMA_LITERAL("");
		return &data.front();
	}

	UInt32 length() const { return data.empty() ? 0 : (UInt32)data.size() - 1; }

private:
	SecureVector<Char> data;
};

}
