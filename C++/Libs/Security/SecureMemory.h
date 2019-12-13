#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

//--------------------------------------------------------------
// "Secure Memory" is a bit of a misnomer, because if an 
//  attacker has access to the user's account, there likely will
//  be a way for them to peek into the user's RAM...
//
// The "Secure Memory" systems implemented within do NOT 
//  guarantee that attackers cannot read the data stored within
//  them. These systems only try to minimize the chance that
//  the contents of these memory regions are accidentally 
//  disclosed via programmer or system errors, and provide basic
//  mitigation against some attacks.
//
// Also, these systems do ALMOST NOTHING by default! You need to
//  provide a secure memory implementation at compile time.
// The `PhantasmaAPI_sodium.h` adaptor implements these macros 
//  to use the implementation provided by libSodium.
//--------------------------------------------------------------

#if !defined(PHANTASMA_SECURE_ALLOC) || !defined(PHANTASMA_SECURE_FREE)
# include <cstdlib>
#endif

//Erase a region of memory in a way that won't be omitted by the optimizer
#ifndef PHANTASMA_WIPEMEM
#include <cstring>
# define PHANTASMA_WIPEMEM(buffer, size)    memset(buffer, 0, size)
#endif

//Pin a region to avoid swapping to disk.
#ifndef PHANTASMA_LOCKMEM
# define PHANTASMA_LOCKMEM(pointer, size)
#endif

//Un-pin a region and also erase it as `PHANTASMA_WIPEMEM` would do.
#ifndef PHANTASMA_UNLOCKMEM
# define PHANTASMA_UNLOCKMEM(pointer, size) memset(pointer, 0, size)
#endif

//Allocate a region of memory that can have its access permissions modified
#ifndef PHANTASMA_SECURE_ALLOC
# define PHANTASMA_SECURE_ALLOC(size)       malloc(size)
#endif

//Release memory allocated by `PHANTASMA_SECURE_ALLOC`
#ifndef PHANTASMA_SECURE_FREE
# define PHANTASMA_SECURE_FREE(ptr)         free(ptr)
#endif

//Used only with allocations from `PHANTASMA_SECURE_ALLOC`. Mark the allocation as non-read / non-write.
#ifndef PHANTASMA_SECURE_NOACCESS
# define PHANTASMA_SECURE_NOACCESS(ptr)
#endif

//Used only with allocations from `PHANTASMA_SECURE_ALLOC`. Mark the allocation as read-only.
#ifndef PHANTASMA_SECURE_READONLY
# define PHANTASMA_SECURE_READONLY(ptr)
#endif

//Used only with allocations from `PHANTASMA_SECURE_ALLOC`. Mark the allocation as read/write.
#ifndef PHANTASMA_SECURE_READWRITE
# define PHANTASMA_SECURE_READWRITE(ptr)
#endif

namespace phantasma {

// RAII wrapper around the lock/unlock macros.
// Use to mark temporary memory regions as containing sensitive data. They will be erased at the end of the current scope.
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

//Fixed size array that uses the lock/unlock macros.
//Used for temporary byte arrays that store sensitive information.
//Erased upon destruction.
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
