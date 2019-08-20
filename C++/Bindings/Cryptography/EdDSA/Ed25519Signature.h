#pragma once

#include "Ed25519.h"
#include "../SignatureKind.h"
#include "../Address.h"

namespace phantasma {

class Ed25519Signature 
{
public:
	Ed25519Signature()
	{
	}
	Ed25519Signature( PHANTASMA_VECTOR<Byte>&& signature )
		: bytes(signature)
	{
	}
	Ed25519Signature( const PHANTASMA_VECTOR<Byte>& signature )
		: bytes(signature)
	{
	}

	const PHANTASMA_VECTOR<Byte>& Bytes() const { return bytes; }
	
	//public override SignatureKind Kind => SignatureKind::Ed25519;
	
	template<class Enumerable>
	bool Verify(const PHANTASMA_VECTOR<Byte>& message, Enumerable& addresses)
	{
		for(const Address& address : addresses)
		{
			if (Ed25519::Verify(bytes, message, address.PublicKey()))
			{
				return true;
			}
		}
	
		return false;
	}

	template<class BinaryWriter>
	void SerializeData(BinaryWriter& writer)
	{
		writer.WriteByteArray(bytes);
	}

	template<class BinaryReader>
	void UnserializeData(BinaryReader& reader)
	{
		bytes = reader.ReadByteArray();
	}
private:
	//todo - signatures are always 64 bytes
	PHANTASMA_VECTOR<Byte> bytes;
};

}
