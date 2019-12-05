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

	PrivateKey()
		: m_data(Length, 0)
	{
	}
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
	PrivateKey privateKey;
	Address    address;
public:
	const PrivateKey& PrivateKey() const { return privateKey; }
	const Address&    Address()    const { return address; }

	KeyPair()
		: privateKey()
		, address()
	{
	}
	KeyPair( const Byte* privateKey, int privateKeyLength )
		: privateKey(privateKey, privateKeyLength)
		, address( Ed25519::PublicKeyFromSeed( privateKey, privateKeyLength ) )
	{
	}

	KeyPair& operator=( const KeyPair& other )
	{
		privateKey = other.privateKey;
		address = other.address;
		return *this;
	}

	String ToString() const
	{
		return address.Text();
	}

	static KeyPair Generate()
	{
		do
		{
			PinnedBytes<PrivateKey::Length> privateKey;
			Entropy::GetRandomBytes( privateKey.bytes, PrivateKey::Length );
			KeyPair pair( privateKey.bytes, PrivateKey::Length );
			if (!pair.Address().IsInterop())
			{
				return pair;
			}
		} while (true);
	}

	static KeyPair FromWIF(const SecureString& wif)
	{
		return FromWIF(wif.c_str(), wif.length());
	}
	static KeyPair FromWIF(const Char* wif, int wifStringLength)
	{
		if( !wif || wif[0] == '\0' || wifStringLength <= 0 )
		{
			PHANTASMA_EXCEPTION( "WIF required" );
			Byte nullKey[PrivateKey::Length] = {};
			return KeyPair( nullKey, PrivateKey::Length );
		}

		PinnedBytes<34> data;
		int size = Base58::CheckDecodeSecure(data.bytes, 34, wif, wifStringLength);
		if( size != 34 || data.bytes[0] != 0x80 || data.bytes[33] != 0x01 )
		{
			PHANTASMA_EXCEPTION( "Invalid WIF format" );
			Byte nullKey[PrivateKey::Length] = {};
			return KeyPair( nullKey, PrivateKey::Length );
		}
		return { &data.bytes[1], 32 };
	}

	SecureString ToWIF() const
	{
		static_assert( PrivateKey::Length == 32, "uh oh" );
		PinnedBytes<34> temp;
		Byte* data = temp.bytes;
		data[0] = 0x80;
		data[33] = 0x01;
		SecureByteReader read = privateKey.Read();
		PHANTASMA_COPY(read.Bytes(), read.Bytes()+32, data+1);
		return Base58::CheckEncodeSecure(data, 34);
	}

	Ed25519Signature Sign( const ByteArray& message ) const
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
		return Ed25519Signature( Ed25519::Sign( &message.front(), (int)message.size(), expandedPrivateKey.bytes, 64 ) );
	}
};

}
