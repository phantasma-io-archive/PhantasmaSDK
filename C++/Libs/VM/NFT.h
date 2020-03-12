#pragma once

#include "../Cryptography/Hash.h"

namespace phantasma {

inline BigInteger NftIdFromNftRom(const ByteArray& rom)
{
	return BigInteger(Hash::FromBytes(rom).ToByteArray(), Hash::Length);
}

}
