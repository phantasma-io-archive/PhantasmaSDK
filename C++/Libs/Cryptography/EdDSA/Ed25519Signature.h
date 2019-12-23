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
		for( int i=0; i!=Length; ++i )
			bytes[i] = 0;
	}
	Ed25519Signature( const Byte* signature, int signatureLength )
		: Ed25519Signature()
	{
		if( signatureLength == Length )
		{
			PHANTASMA_COPY(signature, signature+Length, bytes);
		}
		else if(signatureLength != 0)
		{
			PHANTASMA_EXCEPTION( "Invalid Ed25519 signature length, should be 64 bytes" );
		}
	}
	Ed25519Signature( const ByteArray& signature )
			: Ed25519Signature(signature.empty() ? 0 : &signature.front(), (int)signature.size())
	{
	}

	constexpr static int Length = 64;
	constexpr static SignatureKind Kind = SignatureKind::Ed25519;

	const Byte* Bytes() const { return bytes; }
	
	bool Verify(const Byte* message, int messageLength, const Address* addresses, int numAddresses) const
	{
		if( messageLength <= 0 )
			return false;

		for(int i=0; i<numAddresses; ++i)
		{
			const Address& address = addresses[i];
			if( !address.IsUser() )
				continue;

			const Byte* pubKey = address.ToByteArray() + 2;
			int pubKeyLength = address.GetSize() - 2;
			if (Ed25519::Verify(bytes, Length, message, messageLength, pubKey, pubKeyLength))
			{
				return true;
			}
		}
	
		return false;
	}

	bool operator==( const Ed25519Signature& o ) const
	{
		return PHANTASMA_EQUAL( bytes, bytes+Length, o.bytes );
	}

	template<class BinaryWriter>
	void SerializeData(BinaryWriter& writer) const
	{
		writer.WriteByteArray(bytes, Length);
	}

	template<class BinaryReader>
	void UnserializeData(BinaryReader& reader)
	{
		reader.ReadByteArray(bytes, Length);
	}
	
	template<class IKeyPair>
	static Ed25519Signature Generate(const IKeyPair& keypair, const ByteArray& message)
	{
		if(message.empty())
		{
			PHANTASMA_EXCEPTION("Can't sign an empty message");
			return Ed25519Signature();
		}
		PinnedBytes<64> expandedPrivateKey;
		{
			SecureByteReader read = keypair.PrivateKey().Read();
			Ed25519::ExpandedPrivateKeyFromSeed( expandedPrivateKey.bytes, 64, read.Bytes(), PrivateKey::Length );
		}
		ByteArray sign = Ed25519::Sign( &message.front(), (int)message.size(), expandedPrivateKey.bytes, 64 );
		return Ed25519Signature(sign);
	}
private:
	Byte bytes[Length];
};

}
