#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

enum class ProofOfWork
{
	None = 0,
	Minimal = 5,
	Moderate = 15,
	Hard = 19,
	Heavy = 24,
	Extreme = 30
};
