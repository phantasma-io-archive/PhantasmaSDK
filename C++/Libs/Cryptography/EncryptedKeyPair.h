#pragma once

#include "KeyPair.h"
#include "Entropy.h"
#include "AuthenticatedEncryption.h"
#include "SHA.h"
#include "../Utils/TextUtils.h"
#include "../Numerics/Base64.h"

namespace phantasma {

//--------------------------------------------------------------
// Private-keys should be kept as private as possible, so it's a
//  good idea to never write them to disk, and even avoid 
//  keeping them in memory longer than necessary.
// This class is a simple way to encrypt a private-key using a 
//  user-supplied password, converting it into a form that is
//  safer to store on disk.
// As long as the user has chosen a *strong* password (and you
//  are compiling with a strong PHANTASMA_AuthenticatedEncrypt
//  implementation) then the user's private key will be safe
//  from digital theives.
//--------------------------------------------------------------
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

	//Encrypt a private key with a password for safe keeping
	EncryptedKeyPair( const Char* password, const Byte* privateKey, int privateKeyLength )
		: m_address( Address::FromKey(Ed25519::PublicKeyFromSeed( privateKey, privateKeyLength )) )
	{
		SecureVector<Byte> temp;
		int passwordLength = 0;
		const char* utf8_password = (char*)GetUTF8Bytes( password, 0, temp, passwordLength );

		Entropy::GetRandomBytes( m_salt, PHANTASMA_PasswordSaltLength );
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

	bool Decrypt( PhantasmaKeys& output, const Char* password, bool* out_tamperWarning=0 ) const 
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

		output = { decrypted.bytes, PrivateKey::Length };
		if( output.Address() != m_address )
		{
			if( out_tamperWarning )
				*out_tamperWarning = true;
			PHANTASMA_EXCEPTION("Invalid EncryptedKeyPair");
			return false;
		}
		return true;
	}

private:
	constexpr static int headerChars = 5;
	constexpr static int addressChars = Address::TextLength;
	constexpr static int saltChars = Base64::RequiredCharacters(PHANTASMA_PasswordSaltLength);
	constexpr static int nonceChars = Base64::RequiredCharacters(PHANTASMA_AuthenticatedNonceLength);
	constexpr static int hashChars = Base64::RequiredCharacters(PHANTASMA_SHA256_LENGTH);
	constexpr static int addressBytes = Address::LengthInBytes;
	constexpr static int saltBytes = PHANTASMA_PasswordSaltLength;
	constexpr static int nonceBytes = PHANTASMA_AuthenticatedNonceLength;
	constexpr static int secretHeaderBytes = 4;
	constexpr static int hashBytes = PHANTASMA_SHA256_LENGTH;
public:

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
		int secretChars = Base64::RequiredCharacters(1);
		int minimumLength = headerChars + addressChars + saltChars + nonceChars + secretChars + hashChars;
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
			int decodeError = Base64::Decode(salt, PHANTASMA_PasswordSaltLength, text, saltChars);
			if( decodeError != 0 )
				break;
			text += saltChars;

			if( *text != 'N' )
				break;
			text += 1;
			Byte nonce[PHANTASMA_AuthenticatedNonceLength];
			decodeError = Base64::Decode(nonce, PHANTASMA_AuthenticatedNonceLength, text, nonceChars);
			if( decodeError != 0 )
				break;
			text += nonceChars;

			if( *text != 'K' )
				break;
			text += 1;

			const char* secretEnd;
			for( secretEnd = text; *secretEnd != '_' && (secretEnd-begin) < textLength; ++secretEnd )
			{}
			secretChars = (int)(secretEnd - text);

			ByteArray secret = Base64::Decode(text, secretChars);
			text += secretChars;

			int inputLengthNoHash = headerChars + addressChars + saltChars + nonceChars + secretChars;
			int inputLength = inputLengthNoHash + hashChars;
			if( textLength < inputLength )
				break;

			if( *text != '_' )
				break;
			text += 1;

			Byte storedHash[PHANTASMA_SHA256_LENGTH];
			Byte computedHash[PHANTASMA_SHA256_LENGTH];
			decodeError = Base64::Decode(storedHash, PHANTASMA_SHA256_LENGTH, text, hashChars);
			if( decodeError != 0 )
				break;

			ByteArray temp;
			int utf8Length = 0;
			const Byte* utf8_result = GetUTF8Bytes( begin, inputLengthNoHash, temp, utf8Length );
			SHA256(computedHash, PHANTASMA_SHA256_LENGTH, utf8_result, utf8Length);

			if(!PHANTASMA_EQUAL( storedHash, storedHash+PHANTASMA_SHA256_LENGTH, computedHash ))
				break;

			return EncryptedKeyPair(address, salt, nonce, &secret.front(), secret.size());
		}
		if( out_error )
			*out_error = true;
		PHANTASMA_EXCEPTION("Invalid EncryptedKeyPair string");
		return EncryptedKeyPair();
	}
	static EncryptedKeyPair FromBytes( const Byte* begin, int dataSize, bool* out_error=0 )
	{
		int secretBytes = 1;
		if( dataSize < addressBytes + saltBytes + nonceBytes + secretHeaderBytes + secretBytes + hashBytes )
		{
			if( out_error )
				*out_error = true;
			PHANTASMA_EXCEPTION("Invalid EncryptedKeyPair bytes");
			return EncryptedKeyPair();
		}

		const Byte* data = begin;
		data += Address::LengthInBytes;
		const Byte* salt = data;
		data += saltBytes;
		const Byte* nonce = data;
		data += nonceBytes;
		const Byte* secretHeader = data;
		data += secretHeaderBytes;
		secretBytes = *secretHeader;
		const Byte* secret = data;
		data += secretBytes;
		const Byte* hash = data;
		data += hashBytes;

		if( dataSize < addressBytes + saltBytes + nonceBytes + secretHeaderBytes + secretBytes + hashBytes )
		{
			if( out_error )
				*out_error = true;
			PHANTASMA_EXCEPTION("Invalid EncryptedKeyPair bytes");
			return EncryptedKeyPair();
		}

		Byte rehash[hashBytes];
		SHA256(rehash, hashBytes,  begin, addressBytes + saltBytes + nonceBytes + secretHeaderBytes + secretBytes);
		if(!PHANTASMA_EQUAL( hash, hash + hashBytes, rehash ))
		{
			if( out_error )
				*out_error = true;
			PHANTASMA_EXCEPTION("Invalid EncryptedKeyPair bytes");
			return EncryptedKeyPair();
		}

		return EncryptedKeyPair(Address(begin, Address::LengthInBytes), salt, nonce, secret, secretBytes);
	}
	static EncryptedKeyPair FromBytes( const ByteArray& bytes, bool* out_error=0 )
	{
		return FromBytes( bytes.empty()?0:&bytes.front(), bytes.size(), out_error );
	}

	String ToText() const
	{
		if(m_encryptedSecret.empty())
		{
			PHANTASMA_EXCEPTION("Empty EncryptedKeyPair");
			return String();
		}
		const String& address = m_address.Text();

		Char salt[saltChars+1];
		Char nonce[nonceChars+1];
		Base64::Encode(salt, saltChars+1, m_salt, PHANTASMA_PasswordSaltLength);
		Base64::Encode(nonce, nonceChars+1, m_nonce, PHANTASMA_AuthenticatedNonceLength);
		String secret = Base64::Encode(m_encryptedSecret);

		StringBuilder builder;
		builder << "A" << address;
		builder << "S" << salt;
		builder << "N" << nonce;
		builder << "K" << secret;
		builder << "_";

		const String& result = builder.str();

		ByteArray temp;
		int utf8Length = 0;
		const Byte* utf8_result = GetUTF8Bytes( result, temp, utf8Length );
		Byte hash[PHANTASMA_SHA256_LENGTH];
		SHA256(hash, PHANTASMA_SHA256_LENGTH, utf8_result, utf8Length);
		Char hash64[hashChars+1];
		Base64::Encode(hash64, hashChars+1, hash, PHANTASMA_SHA256_LENGTH);
		builder << hash64;

		return builder.str();
	}

	ByteArray ToBytes() const
	{
		if(m_encryptedSecret.empty())
		{
			PHANTASMA_EXCEPTION("Empty EncryptedKeyPair");
			return ByteArray();
		}

		Int32 secretBytes = m_encryptedSecret.size();

		ByteArray result;
		result.resize(addressBytes + saltBytes + nonceBytes + secretHeaderBytes + secretBytes + hashBytes);
		Byte* output = &result.front();

		const Byte* address = m_address.ToByteArray();
		PHANTASMA_COPY(address, address + addressBytes, output);
		output += addressBytes;

		PHANTASMA_COPY(m_salt, m_salt + saltBytes, output);
		output += saltBytes;

		PHANTASMA_COPY(m_nonce, m_nonce + nonceBytes, output);
		output += nonceBytes;

		Byte secretHeader[secretHeaderBytes];
		memcpy(secretHeader, &secretBytes, secretHeaderBytes);
		PHANTASMA_COPY(secretHeader, secretHeader+secretHeaderBytes, output);
		output += secretHeaderBytes;

		PHANTASMA_COPY(m_encryptedSecret.begin(), m_encryptedSecret.end(), output);
		output += m_encryptedSecret.size();

		int size = (int)(output - &result.front());

		Byte hash[hashBytes];
		SHA256(hash, hashBytes,  &result.front(), size);
		PHANTASMA_COPY(hash, hash + hashBytes, output);
		output += hashBytes;

		if( result.size() != (int)(output - &result.front()) )
		{
			PHANTASMA_EXCEPTION("Internal error");
		}
		return result;
	}
	
};

}
