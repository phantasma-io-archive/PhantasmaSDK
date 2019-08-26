#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

namespace phantasma {

template<class T>
void ArrayReverse(T& vec)
{
	int last = (int)vec.size()-1;
	for( int i=0, end=(int)vec.size()/2; i!=end; ++i )
		PHANTASMA_SWAP( vec[i], vec[last-i] );
}
template<class T>
void ArrayReverse(T* vec, int size)
{
	int last = size-1;
	for( int i=0, end=size/2; i!=end; ++i )
		PHANTASMA_SWAP( vec[i], vec[last-i] );
}

}