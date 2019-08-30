#pragma once

#include "KeyPair.h"
#include "Entropy.h"
#include "AuthenticatedEncryption.h"
#include "SHA.h"
#include "../Utils/TextUtils.h"
#include "../Numerics/Base16.h"

namespace phantasma {

class EncryptedKeyPair
{
	Address   m_address;
	Byte      m_salt[PHANTASMA_PasswordSaltLength];
	Byte      m_nonce[PHANTASMA_AuthenticatedNonceLength];
	ByteArray m_encryptedSecret;
public:
	const Address& GetAddress() const { return m_address; }

	EncryptedKeyPair()
		: m_salt{}
		, m_nonce{}
	{
	}

	EncryptedKeyPair( const Address& address, const Byte* salt, const Byte* nonce, const Byte* secret, int secretSize )
		: m_address( address )
		, m_salt{}
		, m_nonce{}
	{
		if( !salt || !nonce || !secret || secretSize <= 0 )
		{
			PHANTASMA_EXCEPTION("Invalid usage");
			return;
		}
		PHANTASMA_COPY( salt, salt + PHANTASMA_PasswordSaltLength, m_salt );
		PHANTASMA_COPY( nonce, nonce + PHANTASMA_AuthenticatedNonceLength, m_nonce );
		m_encryptedSecret.resize(secretSize);
		PHANTASMA_COPY( secret, secret + secretSize, &m_encryptedSecret.front() );
	}

	EncryptedKeyPair( const Char* password, const Byte* privateKey, int privateKeyLength )
		: m_address( Ed25519::PublicKeyFromSeed( privateKey, privateKeyLength ) )
	{
		SecureVector<Byte> temp;
		int passwordLength = 0;
		const char* utf8_password = (char*)GetUTF8Bytes( password, 0, temp, passwordLength );

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
		SecureVector<Byte> temp;
		int passwordLength = 0;
		const char* utf8_password = (char*)GetUTF8Bytes( password, 0, temp, passwordLength );

		PinnedBytes<PHANTASMA_AuthenticatedKeyLength> key;
		Encryption::PasswordToKey( key.bytes, PHANTASMA_AuthenticatedKeyLength, utf8_password, passwordLength, m_salt, PHANTASMA_PasswordSaltLength );

		PinnedBytes<PrivateKey::Length> decrypted;
		int result = Encryption::Decrypt(decrypted.bytes, PrivateKey::Length, &m_encryptedSecret.front(), m_encryptedSecret.size(), m_nonce, PHANTASMA_AuthenticatedNonceLength, key.bytes, PHANTASMA_AuthenticatedKeyLength);
		if( result != 0 )
			return false;

		output = KeyPair( decrypted.bytes, PrivateKey::Length );
		return true;
	}

	static EncryptedKeyPair FromText( const String& text, bool* out_error=0 )
	{
		return FromText( text.c_str(), text.length(), out_error );
	}
	static EncryptedKeyPair FromText( const Char* begin, int textLength=0, bool* out_error=0 )
	{
		if(!begin || textLength < 0)
		{
			PHANTASMA_EXCEPTION("Invalid Usage");
			if( out_error )
				*out_error = true;
			return EncryptedKeyPair();
		}
		if( textLength == 0 )
		{
			textLength = (int)PHANTASMA_STRLEN(begin);
		}
		int minimumLength = 5 + Address::TextLength + PHANTASMA_PasswordSaltLength*2 + PHANTASMA_AuthenticatedNonceLength*2 + 2 + PHANTASMA_SHA256_LENGTH*2;
		while( textLength >= minimumLength )//just an if, but i'm using 'break' as "goto error" here:
		{
			const char* text = begin;
			if( *text != 'A' )
				break;
			text += 1;
			bool error = false;
			Address address = Address::FromText( text, Address::TextLength, &error );
			if( error )
				break;
			text += Address::TextLength;

			if( *text != 'S' )
				break;
			text += 1;
			Byte salt[PHANTASMA_PasswordSaltLength];
			int decoded = Base16::Decode(salt, PHANTASMA_PasswordSaltLength, text, PHANTASMA_PasswordSaltLength*2);
			if( decoded != PHANTASMA_PasswordSaltLength )
				break;
			text += PHANTASMA_PasswordSaltLength*2;

			if( *text != 'N' )
				break;
			text += 1;
			Byte nonce[PHANTASMA_AuthenticatedNonceLength];
			decoded = Base16::Decode(nonce, PHANTASMA_AuthenticatedNonceLength, text, PHANTASMA_AuthenticatedNonceLength*2);
			if( decoded != PHANTASMA_AuthenticatedNonceLength )
				break;
			text += PHANTASMA_AuthenticatedNonceLength*2;

			if( *text != 'K' )
				break;
			text += 1;

			const char* secretEnd;
			for( secretEnd = text; *secretEnd != 'H' && (secretEnd-begin) < textLength; ++secretEnd )
			{}
			int secretChars = (int)(secretEnd - text);

			ByteArray secret;
			secret.resize(secretChars/2);
			decoded = Base16::Decode(&secret.front(), secretChars/2, text, secretChars);
			text += secretChars;

			int inputLengthNoHash = 5 + Address::TextLength + PHANTASMA_PasswordSaltLength*2 + PHANTASMA_AuthenticatedNonceLength*2 + secretChars;
			int inputLength = inputLengthNoHash + PHANTASMA_SHA256_LENGTH*2;
			if( textLength < inputLength )
				break;

			if( *text != 'H' )
				break;
			text += 1;

			Byte storedHash[PHANTASMA_SHA256_LENGTH];
			Byte computedHash[PHANTASMA_SHA256_LENGTH];
			decoded = Base16::Decode(storedHash, PHANTASMA_SHA256_LENGTH, text, PHANTASMA_SHA256_LENGTH*2);
			if( decoded != PHANTASMA_SHA256_LENGTH )
				break;

			ByteArray temp;
			int utf8Length = 0;
			const Byte* utf8_result = GetUTF8Bytes( begin, inputLengthNoHash, temp, utf8Length );
			SHA256(computedHash, PHANTASMA_SHA256_LENGTH, utf8_result, utf8Length);

			if(!PHANTASMA_EQUAL( storedHash, storedHash+PHANTASMA_SHA256_LENGTH, computedHash ))
				break;

			return EncryptedKeyPair(address, salt, nonce, &secret.front(), secret.size());
		}
		PHANTASMA_EXCEPTION("Invalid EncryptedKeyPair string");
		if( out_error )
			*out_error = true;
		return EncryptedKeyPair();
	}

	String ToText() const
	{
		if(m_encryptedSecret.empty())
		{
			PHANTASMA_EXCEPTION("Empty EncryptedKeyPair");
			return String();
		}
		const String& address = m_address.Text();
		String salt = Base16::Encode(m_salt, PHANTASMA_PasswordSaltLength);
		String nonce = Base16::Encode(m_nonce, PHANTASMA_AuthenticatedNonceLength);
		String secret = Base16::Encode(&m_encryptedSecret.front(), m_encryptedSecret.size());

		StringBuilder builder;
		builder << "A";
		builder << address;
		builder << "S";
		builder << salt;
		builder << "N";
		builder << nonce;
		builder << "K";
		builder << secret;
		builder << "H";

		const String& result = builder.str();

		ByteArray temp;
		int utf8Length = 0;
		const Byte* utf8_result = GetUTF8Bytes( result, temp, utf8Length );
		Byte hash[PHANTASMA_SHA256_LENGTH];
		SHA256(hash, PHANTASMA_SHA256_LENGTH, utf8_result, utf8Length);
		builder << Base16::Encode(hash, PHANTASMA_SHA256_LENGTH);

		return builder.str();
	}
	
};

}
