#pragma once

#ifndef PHANTASMA_SHA256
#error "You must supply a SHA256 implementation"
#endif

namespace phantasma {

#define PHANTASMA_SHA256_LENGTH 32

void SHA256( Byte* output, int outputSize, const Byte* input, int inputSize )
{
	if(!output || !input || outputSize != PHANTASMA_SHA256_LENGTH || inputSize < 0 )
	{
		PHANTASMA_EXCEPTION("Invalid arguments");
		return;
	}
	PHANTASMA_SHA256(output, outputSize, input, inputSize);
}

ByteArray SHA256( const ByteArray& input )
{
	ByteArray result;
	result.resize(PHANTASMA_SHA256_LENGTH);
	SHA256(&result.front(), PHANTASMA_SHA256_LENGTH, input.empty() ? 0 : &input.front(), (int)input.size());
	return result;
}

}
