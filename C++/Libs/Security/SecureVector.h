#pragma once

//--------------------------------------------------------------
// Read the "Secure Memory" disclaimer in SecureMemory.h!
//--------------------------------------------------------------

#include "SecureMemory.h"
#include <type_traits>

namespace phantasma {

// This is a wrapper around the PHANTASMA_VECTOR type which adds pinning/unpinning/wiping behavior to 
//  the memory allocations in order to prevent the data from being swapped out of RAM.
// Data contained in these vectors is also always overwritten with zeros during deallocation.
template<class T>
class SecureVector
{
public:
	SecureVector() 
	{
		static_assert( std::is_trivially_destructible<T>::value, "This class overwrites elements with zero, so they should be POD types" );
	}

	~SecureVector() 
	{
		clear();
	}

	SecureVector( const SecureVector& other )
	{
		*this = other;
	}

	SecureVector& operator=( const SecureVector& other )
	{
		Unlock();
		data = other.data;
		Lock();
		return *this;
	}

	typedef typename PHANTASMA_VECTOR<T>::size_type size_type;

	auto  begin()       { return data.begin(); }
	auto  begin() const { return data.begin(); }
	auto    end()       { return data.end(); }
	auto    end() const { return data.end(); }
	auto   size() const { return data.size(); }
	auto&  back()       { return data.back(); }
	auto&  back() const { return data.back(); }
	auto  empty() const { return data.empty(); }
	auto& front()       { return data.front(); }
	auto& front() const { return data.front(); }
	auto& operator[](size_type i)       { return data[i]; }
	auto& operator[](size_type i) const { return data[i]; }

	void reserve( size_type size ) { data.reserve(size); }
	void resize( size_type size )
	{
		if(data.capacity() >= size) // relying on std C++ vector iterator invalidation rules here
		{
			data.resize(size);
		}
		else
		{
			PHANTASMA_VECTOR<T> clone;
			clone.reserve(size);
			clone = data;
			Unlock();
			clone.resize(size);
			PHANTASMA_SWAP(data, clone);
			Lock();
		}
	}

	void clear()
	{
		Unlock();
		data.clear();
	}

	void push_back( const T& t ) 
	{
		if( data.capacity() > data.size() ) // relying on std C++ vector iterator invalidation rules here
		{
			bool wasEmpty = data.empty();
			data.push_back(t);
			if( wasEmpty )
				PHANTASMA_LOCKMEM( &data.front(), data.capacity() * sizeof(T) );
		}
		else
		{
			PHANTASMA_VECTOR<T> clone;
			clone.reserve(data.size() * 2);
			clone = data;
			Unlock();
			PHANTASMA_SWAP(data, clone);
			Lock();
			data.push_back(t);
		}
	}

	void pop_back() 
	{ 
		bool wasEmpty = data.empty();
		data.pop_back();
		if(!wasEmpty && data.empty())
		{
			PHANTASMA_UNLOCKMEM( &data.front(), data.capacity() * sizeof(T) );
		}
	}
private:
	PHANTASMA_VECTOR<T> data;

	void Lock()
	{
		if( !data.empty() )
			PHANTASMA_LOCKMEM( &data.front(), data.capacity() * sizeof(T) );
	}
	void Unlock()
	{
		if( !data.empty() )
			PHANTASMA_UNLOCKMEM( &data.front(), data.capacity() * sizeof(T) );
	}
};

}
