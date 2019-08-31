#pragma once

namespace phantasma {

// Some templates test if a type has inherited from `Serializable` to tell whether
//  they contain SerializeData/UnserializeData members or not.
class Serializable // concept
{
	//void SerializeData(BinaryWriter& writer) const;
	//void UnserializeData(BinaryReader& reader);
};

}
