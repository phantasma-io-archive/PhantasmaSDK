#pragma once

#include "Address.h"
#include "Entropy.h"
#include "EdDSA/Ed25519Signature.h"
#include "../Security/SecureMemory.h"

namespace phantasma {

class PrivateKey
{
public:
	constexpr static int Length = 32;

	PrivateKey( const Byte* privateKey, int privateKeyLength )
		: m_data(Length, privateKeyLength == Length ? privateKey : 0)
	{
		if(privateKeyLength != Length)
			PHANTASMA_EXCEPTION("privateKey should have length 32");
	}

	SecureByteReader Read() const { return m_data.Read(); }
private:
	SecureByteArray m_data;
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
		PinnedBytes<PrivateKey::Length> privateKey;
		Entropy::GetRandomBytes( privateKey.bytes, PrivateKey::Length );
		return KeyPair( privateKey.bytes, PrivateKey::Length );
	}

	static KeyPair FromWIF( const String& wif ) // todo - secure memory string
	{
		if( wif.empty() )
		{
			PHANTASMA_EXCEPTION( "WIF required" );
			Byte nullKey[PrivateKey::Length] = {};
			return KeyPair( nullKey, PrivateKey::Length );
		}

		PinnedBytes<34> data;
		int size = Base58::DecodeSecure(data.bytes, 34, wif);
		if( size != 34 || data.bytes[0] != 0x80 || data.bytes[33] != 0x01 )
		{
			PHANTASMA_EXCEPTION( "Invalid WIF format" );
			Byte nullKey[PrivateKey::Length] = {};
			return KeyPair( nullKey, PrivateKey::Length );
		}
		KeyPair kp( &data.bytes[1], 32 );
		return kp;
	}

	String ToWIF() const // todo - secure memory string
	{
		static_assert( PrivateKey::Length == 32, "uh oh" );
		PinnedBytes<34> temp;
		Byte* data = temp.bytes;
		data[0] = 0x80;
		data[33] = 0x01;
		SecureByteReader read = privateKey.Read();
		PHANTASMA_COPY(read.Bytes(), read.Bytes()+32, data+1);
		String wif = Base58::Encode(data, 34);// todo - secure memory string
		return wif;
	}

	Ed25519Signature Sign( const PHANTASMA_VECTOR<Byte>& message ) const
	{
		if(message.empty())
		{
			PHANTASMA_EXCEPTION("Can't sign an empty message");
			return Ed25519Signature();
		}
		PinnedBytes<64> expandedPrivateKey;
		{
			SecureByteReader read = privateKey.Read();
			Ed25519::ExpandedPrivateKeyFromSeed( expandedPrivateKey.bytes, 64, read.Bytes(), PrivateKey::Length );
		}
		return Ed25519Signature( Ed25519::Sign( &message.front(), message.size(), expandedPrivateKey.bytes, 64 ) );
	}
};

}
