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
		, m_isNull(true)
	{
	}
	PrivateKey( const Byte* privateKey, int privateKeyLength )
		: m_data(Length, privateKeyLength == Length ? privateKey : 0)
	{
		m_isNull = true;
		if(privateKeyLength != Length)
		{
			PHANTASMA_EXCEPTION("privateKey should have length 32");
			return;
		}

		for(int i=0; i!= Length; ++i)
		{
			if(privateKey[i] != 0)
			{
				m_isNull = false;
				break;
			}
		}
	}

	SecureByteReader Read() const { return m_data.Read(); }
	bool IsNull() const { return m_isNull; }
private:
	SecureByteArray m_data;
	bool m_isNull;
};

class PhantasmaKeys
{
	PrivateKey privateKey;
	ByteArray  publicKey;
	Address    address;
public:
	const PrivateKey& PrivateKey() const { return privateKey; }
	const ByteArray&  PublicKey()  const { return publicKey; }
	const Address&    Address()    const { return address; }

	const Byte*  PublicKeyBytes()  const { return publicKey.size() ? &publicKey.front() : 0; }
	int          PublicKeyLength() const { return (int)publicKey.size(); }

	PhantasmaKeys()
		: privateKey()
		, address()
	{
	}
	PhantasmaKeys( const Byte* privateKey, int privateKeyLength )
		: privateKey(privateKey, privateKeyLength)
		, publicKey( Ed25519::PublicKeyFromSeed( privateKey, privateKeyLength ) )
		, address( Address::FromKey(*this) )
	{
	}

	PhantasmaKeys& operator=( const PhantasmaKeys& other )
	{
		privateKey = other.privateKey;
		publicKey = other.publicKey;
		address = other.address;
		return *this;
	}

	String ToString() const
	{
		return address.Text();
	}

	bool IsNull() const { return privateKey.IsNull(); }

	static PhantasmaKeys Generate()
	{
		PinnedBytes<PrivateKey::Length> privateKey;
		Entropy::GetRandomBytes( privateKey.bytes, PrivateKey::Length );
		return { privateKey.bytes, PrivateKey::Length };
	}

	static PhantasmaKeys FromWIF(const SecureString& wif)
	{
		return FromWIF(wif.c_str(), wif.length());
	}
	static PhantasmaKeys FromWIF(const Char* wif, int wifStringLength=0)
	{
		if( wifStringLength == 0 )
		{
			wifStringLength = (int)PHANTASMA_STRLEN(wif);
		}

		if( !wif || wif[0] == '\0' || wifStringLength <= 0 )
		{
			PHANTASMA_EXCEPTION( "WIF required" );
			Byte nullKey[PrivateKey::Length] = {};
			return PhantasmaKeys( nullKey, PrivateKey::Length );
		}

		PinnedBytes<34> data;
		int size = Base58::CheckDecodeSecure(data.bytes, 34, wif, wifStringLength);
		if( size != 34 || data.bytes[0] != 0x80 || data.bytes[33] != 0x01 )
		{
			PHANTASMA_EXCEPTION( "Invalid WIF format" );
			Byte nullKey[PrivateKey::Length] = {};
			return PhantasmaKeys( nullKey, PrivateKey::Length );
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
		return Ed25519Signature::Generate(*this, message);
	}
};


inline Address Address::FromWIF(const Char* wif, int wifStringLength)
{
	return PhantasmaKeys::FromWIF(wif, wifStringLength).Address();
}

}
