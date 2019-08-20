#pragma once

namespace phantasma {

namespace Entropy
{
#ifndef PHANTASMA_WIPEMEM
# define PHANTASMA_WIPEMEM(buffer, size) memset(buffer, 0, size)
#endif

	void GetRandomBytes( Byte* output, int length )
	{
#ifdef PHANTASMA_RANDOMBYTES
		PHANTASMA_RANDOMBYTES(output, length);
#else
# error "You must supply a PHANTASMA_RANDOMBYTES implementation"
#endif
	}
}

}
