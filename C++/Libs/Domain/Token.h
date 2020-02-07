#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

namespace phantasma { 

enum class TokenFlags : UInt32
{
	None         = 0,
	Transferable = 1 << 0,
	Fungible     = 1 << 1,
	Finite       = 1 << 2,
	Divisible    = 1 << 3,
	Fuel         = 1 << 4,
	Stakable     = 1 << 5,
	Fiat         = 1 << 6,
	Foreign      = 1 << 7,
	Burnable     = 1 << 8,
};

inline constexpr TokenFlags operator  | (TokenFlags a, TokenFlags b) { return  TokenFlags  ( ((UInt32 )a) |  ((UInt32)b)); } 
inline           TokenFlags &operator |=(TokenFlags&a, TokenFlags b) { return (TokenFlags&)( ((UInt32&)a) |= ((UInt32)b)); } 
inline constexpr TokenFlags operator  & (TokenFlags a, TokenFlags b) { return  TokenFlags  ( ((UInt32 )a) &  ((UInt32)b)); } 
inline           TokenFlags &operator &=(TokenFlags&a, TokenFlags b) { return (TokenFlags&)( ((UInt32&)a) &= ((UInt32)b)); } 
inline constexpr TokenFlags operator  ~ (TokenFlags a              ) { return  TokenFlags  (~((UInt32 )a)               ); }
inline constexpr TokenFlags operator  ^ (TokenFlags a, TokenFlags b) { return  TokenFlags  ( ((UInt32 )a) ^  ((UInt32)b)); } 
inline           TokenFlags &operator ^=(TokenFlags&a, TokenFlags b) { return (TokenFlags&)( ((UInt32&)a) ^= ((UInt32)b)); } 

}
