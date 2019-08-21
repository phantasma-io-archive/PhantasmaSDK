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
	
	bool Verify(const PHANTASMA_VECTOR<Byte>& message, const Address* addresses, int numAddresses)
	{
		if( bytes.empty() || message.empty() )
			return false;

		for(int i=0; i<numAddresses; ++i)
		{
			const Address& address = addresses[i];
			if (Ed25519::Verify(&bytes.front(), bytes.size(), &message.front(), message.size(), address.PublicKey(), Address::PublicKeyLength))
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
