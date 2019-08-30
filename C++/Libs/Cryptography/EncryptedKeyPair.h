#pragma once

#include "KeyPair.h"
#include "Entropy.h"
#include "AuthenticatedEncryption.h"
#include "../Utils/TextUtils.h"

namespace phantasma {

class EncryptedKeyPair
{
	Address   m_address;
	Byte      m_salt[PHANTASMA_PasswordSaltLength];
	Byte      m_nonce[PHANTASMA_AuthenticatedNonceLength];
	ByteArray m_encryptedSecret;
public:
	const Address& Address() const { return m_address; }

	EncryptedKeyPair()
		: m_salt{}
		, m_nonce{}
	{
	}

	EncryptedKeyPair( const Char* password, const Byte* privateKey, int privateKeyLength )
		: m_address( Ed25519::PublicKeyFromSeed( privateKey, privateKeyLength ) )
	{
		ByteArray temp;
		int passwordLength = 0;
		const char* utf8_password = (char*)GetUTF8Bytes( password, temp, passwordLength );//todo SecureByteArray version of this!

		Entropy::GetRandomBytes( m_salt, PHANTASMA_AuthenticatedKeyLength );
		Entropy::GetRandomBytes( m_nonce, PHANTASMA_AuthenticatedNonceLength );

		PinnedBytes<PHANTASMA_AuthenticatedKeyLength> key;
		Encryption::PasswordToKey( key.bytes, PHANTASMA_AuthenticatedKeyLength, utf8_password, passwordLength, m_salt, PHANTASMA_PasswordSaltLength );

		int encryptedSize = Encryption::Encrypt(0, 0, privateKey, privateKeyLength, m_nonce, PHANTASMA_AuthenticatedNonceLength, key.bytes, PHANTASMA_AuthenticatedKeyLength);
		if(encryptedSize <= 0)
		{
			PHANTASMA_EXCEPTION("Internal error");
		}
		m_encryptedSecret.resize(encryptedSize);
		int result = Encryption::Encrypt(&m_encryptedSecret.front(), encryptedSize, privateKey, privateKeyLength, m_nonce, PHANTASMA_AuthenticatedNonceLength, key.bytes, PHANTASMA_AuthenticatedKeyLength);
		if(result != 0)
		{
			PHANTASMA_EXCEPTION("Internal error");
		}
	}

	bool Decrypt( KeyPair& output, const Char* password ) const 
	{
		if( m_encryptedSecret.empty() )
			return false;
		ByteArray temp;
		int passwordLength = 0;
		const char* utf8_password = (char*)GetUTF8Bytes( password, temp, passwordLength );//todo SecureByteArray version of this!

		PinnedBytes<PHANTASMA_AuthenticatedKeyLength> key;
		Encryption::PasswordToKey( key.bytes, PHANTASMA_AuthenticatedKeyLength, utf8_password, passwordLength, m_salt, PHANTASMA_PasswordSaltLength );

		PinnedBytes<PrivateKey::Length> decrypted;
		int result = Encryption::Decrypt(decrypted.bytes, PrivateKey::Length, &m_encryptedSecret.front(), m_encryptedSecret.size(), m_nonce, PHANTASMA_AuthenticatedNonceLength, key.bytes, PHANTASMA_AuthenticatedKeyLength);
		if( result != 0 )
			return false;

		output = KeyPair( decrypted.bytes, PrivateKey::Length );
		return true;
	}

	static EncryptedKeyPair FromText( const Char* )
	{
		PHANTASMA_EXCEPTION("TODO");//todo
		return EncryptedKeyPair();
	}

	String ToText() const
	{
		PHANTASMA_EXCEPTION("TODO");//todo
		return String();
	}
	
};

}
