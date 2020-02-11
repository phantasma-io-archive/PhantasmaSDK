#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Include crypto API adaptors immediately after including PhantasmaAPI.h"
#endif 
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with crypto features provided by the 
//  libsodium library (https://libsodium.org) 
//
// Make sure you call sodium_init in your own application before using Phantasma!
//------------------------------------------------------------------------------
#include "sodium/core.h"
#include "sodium/utils.h"
#include "sodium/randombytes.h"
#include "sodium/crypto_hash_sha256.h"
#include "sodium/crypto_sign_ed25519.h"
#include "sodium/crypto_secretbox.h"
#include "sodium/crypto_pwhash.h"

namespace phantasma { 

static_assert(crypto_sign_ed25519_SEEDBYTES      == 32, "Mismatch with the reference implementation");
static_assert(crypto_sign_ed25519_PUBLICKEYBYTES == 32, "Mismatch with the reference implementation");
static_assert(crypto_sign_ed25519_SECRETKEYBYTES == 64, "Mismatch with the reference implementation");
static_assert(crypto_sign_ed25519_BYTES          == 64, "Mismatch with the reference implementation");

void Ed25519_PublicKeyFromSeed(uint8_t* output, int outputLength, const uint8_t* seed, int seedLength)
{
	if( (outputLength != crypto_sign_ed25519_PUBLICKEYBYTES) ||
		(seedLength != crypto_sign_ed25519_SEEDBYTES) )
		return;
	uint8_t secret[crypto_sign_ed25519_SECRETKEYBYTES];
	crypto_sign_ed25519_seed_keypair(output, secret, seed);
	sodium_memzero(secret, crypto_sign_ed25519_SECRETKEYBYTES);
}

void Ed25519_PrivateKeyFromSeed(uint8_t* output, int outputLength, const uint8_t* seed, int seedLength)
{
	if( (outputLength != crypto_sign_ed25519_SECRETKEYBYTES) ||
		(seedLength != crypto_sign_ed25519_SEEDBYTES) )
		return;
	uint8_t publik[crypto_sign_ed25519_PUBLICKEYBYTES];
	crypto_sign_ed25519_seed_keypair(publik, output, seed);
}

uint64_t Ed25519_SignAttached( uint8_t* output, int outputLength, const uint8_t* message, int messageLength, const uint8_t* privateKey, int privateKeyLength )
{
	if( ((UInt32)outputLength < crypto_sign_ed25519_BYTES + messageLength) ||
		((UInt32)privateKeyLength != crypto_sign_ed25519_SECRETKEYBYTES) ||
		messageLength < 0 )
		return 0;
	uint64_t signed_message_len = 0;
	crypto_sign_ed25519(output, &signed_message_len, message, messageLength, privateKey);
	return signed_message_len;
}

uint64_t Ed25519_SignDetached( uint8_t* output, int outputLength, const uint8_t* message, int messageLength, const uint8_t* privateKey, int privateKeyLength )
{
	if( ((UInt32)outputLength < crypto_sign_ed25519_BYTES) ||
		((UInt32)privateKeyLength != crypto_sign_ed25519_SECRETKEYBYTES) ||
		messageLength < 0)
		return 0;
	uint64_t signed_message_len = 0;
	crypto_sign_ed25519_detached(output, &signed_message_len, message, messageLength, privateKey);
	return signed_message_len;
}

bool Ed25519_ValidateAttached( const uint8_t* message, int messageLength, const uint8_t* publicKey, int publicKeyLength )
{
	if( ((UInt32)messageLength < crypto_sign_ed25519_BYTES) ||
		((UInt32)publicKeyLength != crypto_sign_ed25519_PUBLICKEYBYTES) )
		return false;
	return 0 == crypto_sign_ed25519_open(0, 0, message, messageLength, publicKey);
}

bool Ed25519_ValidateDetached( const uint8_t* signature, int signatureLength, const uint8_t* message, int messageLength, const uint8_t* publicKey, int publicKeyLength )
{
	if( ((UInt32)signatureLength != crypto_sign_ed25519_BYTES) ||
		((UInt32)publicKeyLength != crypto_sign_ed25519_PUBLICKEYBYTES) ||
		messageLength < 0 )
		return false;
	return 0 == crypto_sign_ed25519_verify_detached(signature, message, messageLength, publicKey);
}

int Phantasma_Encrypt(Byte* output, int outputLength, const Byte* message, int messageLength, const Byte* nonce, const Byte* key)
{
	if( outputLength < 0 || messageLength < 0 || !message )
		return -1;
	if( !output || outputLength < messageLength + (int)crypto_secretbox_MACBYTES )
		return messageLength + (int)crypto_secretbox_MACBYTES;
	crypto_secretbox_easy( output, message, messageLength, nonce, key );
	return 0;
}

int Phantasma_Decrypt(Byte* output, int outputLength, const Byte* encrypted, int encryptedLength, const Byte* nonce, const Byte* key)
{
	if( outputLength < 0 || encryptedLength < (int)crypto_secretbox_MACBYTES || !encrypted )
		return -1;
	if( !output || outputLength < encryptedLength - (int)crypto_secretbox_MACBYTES )
		return encryptedLength - (int)crypto_secretbox_MACBYTES;
	if( 0==crypto_secretbox_open_easy(output, encrypted, encryptedLength, nonce, key) )
		return 0;
	return INT_MAX;
}

bool Phantasma_PasswordToKey( Byte* output, const char* password, int passwordLength, const Byte* salt )
{
	static_assert( crypto_secretbox_KEYBYTES >= crypto_pwhash_BYTES_MIN, "" );
	static_assert( crypto_secretbox_KEYBYTES <= crypto_pwhash_BYTES_MAX, "" );
	if( !output || !password || passwordLength <= 0 )
		return false;
	crypto_pwhash(output, crypto_secretbox_KEYBYTES,
		password,
		passwordLength,
		salt,
		crypto_pwhash_OPSLIMIT_MODERATE,
		crypto_pwhash_argon2id_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT);
	return true;
}

#define PHANTASMA_RANDOMBYTES(buffer, size) randombytes_buf(buffer, size)
#define PHANTASMA_WIPEMEM(buffer, size)     sodium_memzero(buffer, size)

#define PHANTASMA_LOCKMEM(  pointer, size)  sodium_mlock(  pointer, size)
#define PHANTASMA_UNLOCKMEM(pointer, size)  sodium_munlock(pointer, size)

#define PHANTASMA_SECURE_ALLOC(size)        sodium_malloc(size)
#define PHANTASMA_SECURE_FREE(ptr)          sodium_free(ptr)
#define PHANTASMA_SECURE_NOACCESS(ptr)      sodium_mprotect_noaccess(ptr)
#define PHANTASMA_SECURE_READONLY(ptr)      sodium_mprotect_readonly(ptr)
#define PHANTASMA_SECURE_READWRITE(ptr)     sodium_mprotect_readwrite(ptr)

#define PHANTASMA_Ed25519_PublicKeyFromSeed(output, outputLength, seed, seedLength)                                        \
                  Ed25519_PublicKeyFromSeed(output, outputLength, seed, seedLength)
#define PHANTASMA_Ed25519_PrivateKeyFromSeed(output, outputLength, seed, seedLength)                                       \
                  Ed25519_PrivateKeyFromSeed(output, outputLength, seed, seedLength)
#define PHANTASMA_Ed25519_SignAttached(output, outputLength, message, messageLength, privateKey, privateKeyLength)         \
                  Ed25519_SignAttached(output, outputLength, message, messageLength, privateKey, privateKeyLength)
#define PHANTASMA_Ed25519_SignDetached(output, outputLength, message, messageLength, privateKey, privateKeyLength)         \
                  Ed25519_SignDetached(output, outputLength, message, messageLength, privateKey, privateKeyLength)
#define PHANTASMA_Ed25519_ValidateAttached(message, messageLength, publicKey, publicKeyLength)                             \
                  Ed25519_ValidateAttached(message, messageLength, publicKey, publicKeyLength)
#define PHANTASMA_Ed25519_ValidateDetached(signature, signatureLength, message, messageLength, publicKey, publicKeyLength) \
                  Ed25519_ValidateDetached(signature, signatureLength, message, messageLength, publicKey, publicKeyLength)

#define PHANTASMA_SHA256(output, outputSize, input, inputSize) crypto_hash_sha256(output, input, inputSize)

#define PHANTASMA_AuthenticatedEncrypt        Phantasma_Encrypt
#define PHANTASMA_AuthenticatedDecrypt        Phantasma_Decrypt
#define PHANTASMA_AuthenticatedNonceLength    crypto_secretbox_NONCEBYTES
#define PHANTASMA_AuthenticatedKeyLength      crypto_secretbox_KEYBYTES
#define PHANTASMA_PasswordToKey               Phantasma_PasswordToKey
#define PHANTASMA_PasswordSaltLength          crypto_pwhash_SALTBYTES


}
