#pragma once
#include "BinaryReader.h"

namespace phantasma {

// Some templates test if a type has inherited from `Serializable` to tell whether
//  they contain SerializeData/UnserializeData members or not.
class Serializable // concept
{
	//void SerializeData(BinaryWriter& writer) const;
	//void UnserializeData(BinaryReader& reader);
};

template<class T>
struct Serialization
{
	static T Unserialize(const ByteArray& bytes)
	{
		if (bytes.size() != 0)
		{
			BinaryReader reader(bytes);
			return T::Unserialize(reader);
		}
		return T{};
	}
};

template<>
struct Serialization<String>
{
	static String Unserialize(const ByteArray& bytes)
	{
		if (bytes.size() != 0)
		{
			BinaryReader reader(bytes);
			String s;
			reader.ReadVarString(s);
			return s;
		}
		return String{};
	}
};

}
