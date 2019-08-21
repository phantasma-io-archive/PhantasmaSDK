#pragma once

#include "SecureMemory.h"
#include <type_traits>

namespace phantasma {

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
	}

	auto  begin()       { return data.begin(); }
	auto  begin() const { return data.begin(); }
	auto    end()       { return data.end(); }
	auto    end() const { return data.end(); }
	auto   size() const { return data.size(); }
	auto&  back()        { return data.back(); }
	auto&  back() const { return data.back(); }
	auto  empty() const { return data.empty(); }
	auto& front()       { return data.front(); }
	auto& front() const { return data.front(); }
	void pop_back()     { data.pop_back(); }
	auto& operator[](int i)       { return data[i]; }
	auto& operator[](int i) const { return data[i]; }
	
	void resize( UInt32 size )
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
			data.push_back(t);
		}
		else
		{
			PHANTASMA_VECTOR<T> clone;
			clone.reserve(data.size() * 2);
			clone = data;
			Unlock();
			PHANTASMA_SWAP(data, clone);
			Lock();
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
