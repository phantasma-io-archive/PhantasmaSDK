#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#if !defined(PHANTASMA_SECURE_ALLOC) || !defined(PHANTASMA_SECURE_FREE)
# include <cstdlib>
#endif

#ifndef PHANTASMA_WIPEMEM
#include <cstring>
# define PHANTASMA_WIPEMEM(buffer, size)    memset(buffer, 0, size)
#endif

#ifndef PHANTASMA_LOCKMEM
# define PHANTASMA_LOCKMEM(pointer, size)
#endif

#ifndef PHANTASMA_UNLOCKMEM
# define PHANTASMA_UNLOCKMEM(pointer, size) memset(buffer, 0, size)
#endif

#ifndef PHANTASMA_SECURE_ALLOC
# define PHANTASMA_SECURE_ALLOC(size)       malloc(size)
#endif

#ifndef PHANTASMA_SECURE_FREE
# define PHANTASMA_SECURE_FREE(ptr)         free(ptr)
#endif

#ifndef PHANTASMA_SECURE_NOACCESS
# define PHANTASMA_SECURE_NOACCESS(ptr)
#endif

#ifndef PHANTASMA_SECURE_READONLY
# define PHANTASMA_SECURE_READONLY(ptr)
#endif

#ifndef PHANTASMA_SECURE_READWRITE
# define PHANTASMA_SECURE_READWRITE(ptr)
#endif

namespace phantasma {

// RAII wrapper around the lock/unlock macros
class MemoryPin
{
public:
	MemoryPin( void* pointer, int size )
		: pointer(pointer)
		, size(size)
	{
		PHANTASMA_LOCKMEM(pointer, size); // pins the pages
	}
	~MemoryPin()
	{
		PHANTASMA_UNLOCKMEM(pointer, size); // unpins AND also wipes this range with zero!
	}
private:
	MemoryPin(const MemoryPin&);
	void operator=(const MemoryPin&);
	void* pointer;
	int size;
};

//Fixed size array that uses the lock/unlock macros
template<int N>
class PinnedBytes
{
public:
	PinnedBytes()
		: pin( bytes, N )
	{}
	Byte bytes[N];
	MemoryPin pin;
};

}
