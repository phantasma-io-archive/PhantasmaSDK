#pragma once

#include "../Numerics/BigInteger.h"

namespace phantasma {

enum class VMType
{
	None,
	Struct,
	Bytes,
	Number,
	String,
	Timestamp,
	Bool,
	Enum,
	Object
};

}
