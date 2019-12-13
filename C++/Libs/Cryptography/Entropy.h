#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

namespace phantasma {

namespace Entropy
{
	void GetRandomBytes( Byte* output, int length )
	{
		if(!output || length <= 0)
		{
			PHANTASMA_EXCEPTION("Invalid usage");
			return;
		}
#ifdef PHANTASMA_RANDOMBYTES
		PHANTASMA_RANDOMBYTES(output, length);
#else
# error "You must supply a PHANTASMA_RANDOMBYTES implementation"
#endif
	}
}

}
