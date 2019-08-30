#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#if !defined(PHANTASMA_AuthenticatedEncrypt) || !defined(PHANTASMA_AuthenticatedDecrypt) || !defined(PHANTASMA_AuthenticatedNonceLength) || !defined(PHANTASMA_AuthenticatedKeyLength) || !defined(PHANTASMA_PasswordToKey) || !defined(PHANTASMA_PasswordSaltLength)
#error "You must supply a secret key encryption implementation"
#endif

namespace phantasma {
namespace Encryption {

// if output == nullptr, returns required outputLength
// if invalid arguments (negative sizes, incorrect salt/key sizes), returns -1 (or throws if exceptions enabled)
// returns 0 on success
int Encrypt(phantasma::Byte* output, int outputLength, const phantasma::Byte* message, int messageLength, const phantasma::Byte* nonce, int nonceLength, const phantasma::Byte* key, int keyLength)
{
	int result = -1;
	if( !output || (nonceLength == PHANTASMA_AuthenticatedNonceLength && keyLength == PHANTASMA_AuthenticatedKeyLength) )
		result = PHANTASMA_AuthenticatedEncrypt(output, outputLength, message, messageLength, nonce, key);
	if(result < 0)
	{
		PHANTASMA_EXCEPTION("Invalid usage");
	}
	return result;
}

// if output == nullptr, returns required outputLength
// if invalid arguments (negative sizes, incorrect salt/key sizes), returns -1 (or throws if exceptions enabled)
// returns 0 on success
// return INT_MAX on decryption failure (bad key/nonce)
int Decrypt( phantasma::Byte* output, int outputLength, const phantasma::Byte* encrypted, int encryptedLength, const phantasma::Byte* nonce, int nonceLength, const phantasma::Byte* key, int keyLength )
{
	int result = -1;
	if( !output || (nonceLength == PHANTASMA_AuthenticatedNonceLength && keyLength == PHANTASMA_AuthenticatedKeyLength) )
		result = PHANTASMA_AuthenticatedDecrypt(output, outputLength, encrypted, encryptedLength, nonce, key);
	if(result < 0)
	{
		PHANTASMA_EXCEPTION("Invalid usage");
	}
	return result;
}


bool PasswordToKey( phantasma::Byte* output, int outputLength, const char* password, int passwordLength, const Byte* salt, int saltLength )
{
	if( !output || !salt || !password || passwordLength <= 0 || outputLength != PHANTASMA_AuthenticatedKeyLength || saltLength != PHANTASMA_PasswordSaltLength ||
		!PHANTASMA_PasswordToKey(output, password, passwordLength, salt) )
	{
		PHANTASMA_EXCEPTION("Invalid usage");
		return false;
	}
	return true;
}

}}
