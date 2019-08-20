#pragma once

namespace phantasma {

namespace Entropy
{
	void GetRandomBytes( Byte* output, int length )
	{
#ifdef PHANTASMA_RANDOMBYTES
		PHANTASMA_RANDOMBYTES(output, length);
#else
#error "You must supply a PHANTASMA_RANDOMBYTES implementation"
#endif
	}
}

}
