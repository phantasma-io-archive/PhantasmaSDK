#pragma once

#include "Address.h"
#include "Entropy.h"
#include "EdDSA/Ed25519Signature.h"

namespace phantasma {

class PrivateKey
{
public:
	PrivateKey( const Byte* privateKey, int privateKeyLength )
	{
		if(privateKeyLength == Length)
			PHANTASMA_COPY(privateKey, privateKey+Length, bytes);
		else
		{
			PHANTASMA_EXCEPTION("privateKey should have length 32");
			Byte null[Length] = {};
			PHANTASMA_COPY(null, null+Length, bytes);
		}

	}
	const Byte* Bytes() const { return bytes; }

	constexpr static int Length = 32;
private:
	Byte bytes[Length]; // todo - secure memory array
};

class KeyPair
{
public:
	const PrivateKey privateKey;
	const Address address;

	KeyPair( const Byte* privateKey, int privateKeyLength )
		: privateKey(privateKey, privateKeyLength)
		, address( Ed25519::PublicKeyFromSeed( privateKey, privateKeyLength ) )
	{
	}

	String ToString() const
	{
		return address.Text();
	}

	static KeyPair Generate()
	{
		Byte privateKey[PrivateKey::Length];
		Entropy::GetRandomBytes( privateKey, PrivateKey::Length );
		return KeyPair( privateKey, PrivateKey::Length );
	}

	static KeyPair FromWIF( const String& wif ) // todo - secure memory string
	{
		if( wif.empty() )
		{
			PHANTASMA_EXCEPTION( "WIF required" );
			Byte nullKey[PrivateKey::Length] = {};
			return KeyPair( nullKey, PrivateKey::Length );
		}

		PHANTASMA_VECTOR<Byte> data = Base58::Decode(wif); // todo - secure memory vector
		if( data.size() != 34 || data[0] != 0x80 || data[33] != 0x01 )
		{
			PHANTASMA_EXCEPTION( "Invalid WIF format" );
			Byte nullKey[PrivateKey::Length] = {};
			return KeyPair( nullKey, PrivateKey::Length );
		}
		KeyPair kp( &data[1], 32 );
		PHANTASMA_WIPEMEM(&data[0], 34);
		return kp;
	}

	String ToWIF() const // todo - secure memory string
	{
		Byte data[34];// todo - secure memory array
		data[0] = 0x80;
		data[33] = 0x01;
		PHANTASMA_COPY(privateKey.Bytes(), privateKey.Bytes()+32, data);
		String wif = Base58::Encode(data, 34);
		PHANTASMA_WIPEMEM(data, 34);
		return wif;
	}

	Ed25519Signature Sign( const PHANTASMA_VECTOR<Byte>& message ) const
	{
		const auto& sign = Ed25519::Sign( message, Ed25519::ExpandedPrivateKeyFromSeed( privateKey.Bytes(), privateKey.Length ) );
		return Ed25519Signature( sign );
	}
};

}
