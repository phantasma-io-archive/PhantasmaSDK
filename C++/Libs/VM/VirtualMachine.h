#pragma once

namespace phantasma {

class VirtualMachine
{
public:
	static constexpr int DefaultRegisterCount = 32; // TODO temp hack, this should be 4
	static constexpr int MaxRegisterCount = 32;
};

}
