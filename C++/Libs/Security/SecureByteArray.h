#pragma once
#include "SecureMemory.h"

//--------------------------------------------------------------
// Read the "Secure Memory" disclaimer in SecureMemory.h!
//--------------------------------------------------------------

namespace phantasma {

class SecureByteReader;
class SecureByteWriter;

// This is a simple array of bytes, designed to store secret/private data that should never be leaked.
// If the data is intended to only be accessed rarely, then the 'protectAccess' option (defaults to true)
//  will mark the pages as non-readable to prevent accidental (or malicious) copying of the data.
// The Read and Write methods will temporarilly transition the array to a readable or writable state, and 
//  then automatically transition it back to a non-readable state when the user completes their read/write
//  operations.
class SecureByteArray
{
public:
	SecureByteArray() {}
	SecureByteArray(int size, const Byte* data=0, bool protectAccess=true)
		: m_size(size)
		, m_protectAccess(protectAccess)
	{
		if(size < 0)
		{
			PHANTASMA_EXCEPTION("Invalid size");
			return;
		}
		m_data = (Byte*)PHANTASMA_SECURE_ALLOC(size);
		if( data )
		{
			PHANTASMA_COPY(data, data+size, m_data);
		}
		else
		{
			PHANTASMA_WIPEMEM(m_data, size);
		}

		if( protectAccess )
		{
			PHANTASMA_SECURE_NOACCESS(m_data);
		}
	}
	SecureByteArray(SecureByteArray&& other)
		: m_data(other.m_data) 
		, m_size(other.m_size)
	{
		other.m_data = 0;
	}
	SecureByteArray(const SecureByteArray& other)
	{
		*this = other;
	}
	~SecureByteArray()
	{
		if( m_data )
		{
			PHANTASMA_SECURE_FREE(m_data);
		}
		if( m_readers != 0 || m_writers != 0 )
		{
			//todo - You can't throw in destructors!! -- need PHANTASMA_ERROR / PHANTASMA_ASSERT macro?
			//PHANTASMA_EXCEPTION("Dangling pointer error");//make sure all read/write handles are destroyed before destorying the array!
		}
	}
	SecureByteArray& operator=( SecureByteArray&& other )
	{
		if( m_data )
		{
			PHANTASMA_SECURE_FREE(m_data);
		}
		m_data = other.m_data;
		m_size = other.m_size;
		other.m_data = 0;
	}
	SecureByteArray& operator=( const SecureByteArray& other );

	UInt32           Size() const { return (UInt32)m_size; }
	SecureByteReader Read() const;
	SecureByteWriter Write();
private:
	friend class SecureByteReader;
	friend class SecureByteWriter;
	void UnlockReader(const Byte* data) const
	{
		if( data != m_data )
		{
			PHANTASMA_EXCEPTION("Internal secure memory error");
		}
		--m_readers;
		if( m_protectAccess && m_readers == 0 && m_writers == 0 )
		{
			PHANTASMA_SECURE_NOACCESS(m_data);
		}
		if(m_readers < 0)
		{
			PHANTASMA_EXCEPTION("Internal secure memory error");
		}
	}
	void UnlockWriter(Byte* data)
	{
		if( data != m_data )
		{
			PHANTASMA_EXCEPTION("Internal secure memory error");
		}
		--m_writers;
		if( m_protectAccess && m_readers == 0 && m_writers == 0 )
		{
			PHANTASMA_SECURE_NOACCESS(m_data);
		}
		else if( m_protectAccess && m_writers == 0 )
		{
			PHANTASMA_SECURE_READONLY(m_data);
		}
		if(m_writers < 0)
		{
			PHANTASMA_EXCEPTION("Internal secure memory error");
		}
	}

	Byte* m_data = 0;
	int   m_size = 0;
	mutable int m_readers = 0;
	int m_writers = 0;
	bool m_protectAccess = true;
};

class SecureByteReader
{
public:
	SecureByteReader( SecureByteReader&& other )
		: data( other.data )
		, owner( other.owner )
		, size( other.size )
	{
		other.data = 0;
		other.owner = 0;
	}
	~SecureByteReader()
	{
		if( owner )
			owner->UnlockReader(data);
	}

	int         Size()  const { return size; }
	const Byte* Bytes() const { return data; }
private:
	friend class SecureByteArray;
	SecureByteReader( const Byte* data, const SecureByteArray* owner, int size )
		: data( data ), owner( owner ), size( size ) {}
	void operator=(const SecureByteReader&);
	const Byte* data;
	const SecureByteArray* owner;
	int size;
};

class SecureByteWriter
{
public:
	SecureByteWriter( SecureByteWriter& other )
		: data( other.data )
		, owner( other.owner )
		, size( other.size )
	{
		other.data = 0;
		other.owner = 0;
	}
	~SecureByteWriter()
	{
		if( owner )
			owner->UnlockWriter(data);
	}

	int   Size()  const { return size; }
	Byte* Bytes() const { return data; }
private:
	friend class SecureByteArray;
	SecureByteWriter( Byte* data, SecureByteArray* owner, int size )
		: data( data ), owner( owner ), size( size ) {}
	void operator=(const SecureByteWriter&);
	Byte* data;
	SecureByteArray* owner;
	int size;
};

inline SecureByteArray& SecureByteArray::operator=( const SecureByteArray& other )
{
	if( m_data )
	{
		PHANTASMA_SECURE_FREE(m_data);
	}

	int size = other.m_size;
	bool protectAccess = other.m_protectAccess;
	m_protectAccess = protectAccess;
	m_size = size;
	m_data = (Byte*)PHANTASMA_SECURE_ALLOC(size);
	if( other.m_data )
	{
		const auto& read = other.Read();
		PHANTASMA_COPY(read.Bytes(), read.Bytes()+size, m_data);
	}
	else
	{
		PHANTASMA_WIPEMEM(m_data, size);
	}
	if( protectAccess )
	{
		PHANTASMA_SECURE_NOACCESS(m_data);
	}
	return *this;
}

inline SecureByteReader SecureByteArray::Read() const
{
	if( m_protectAccess && m_readers == 0 && m_writers == 0 )
	{
		PHANTASMA_SECURE_READONLY(m_data);
	}
	++m_readers;
	return { m_data, this, m_size };
}

inline SecureByteWriter SecureByteArray::Write()
{
	if( m_protectAccess && m_writers == 0 )
	{
		PHANTASMA_SECURE_READWRITE(m_data);
	}
	++m_writers;
	return { m_data, this, m_size };
}

}
