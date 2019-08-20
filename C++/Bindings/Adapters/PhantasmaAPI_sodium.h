#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Include crypto API adaptors immediately after including PhantasmaAPI.h"
#endif 
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with crypto features provided by the 
//  libsodium library (https://libsodium.org) 
//------------------------------------------------------------------------------

namespace phantasma { 

static_assert(crypto_sign_ed25519_SEEDBYTES      == 32, "Mismatch with the reference implementation");
static_assert(crypto_sign_ed25519_PUBLICKEYBYTES == 32, "Mismatch with the reference implementation");
static_assert(crypto_sign_ed25519_SECRETKEYBYTES == 64, "Mismatch with the reference implementation");

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

uint64_t Ed25519_Sign( uint8_t* output, int outputLength, const uint8_t* message, int messageLength, const uint8_t* privateKey, int privateKeyLength )
{
	if( ((uint32)outputLength < crypto_sign_ed25519_BYTES + messageLength) ||
		((uint32)privateKeyLength != crypto_sign_ed25519_SECRETKEYBYTES) )
		return 0;
	uint64_t signed_message_len = 0;
	crypto_sign_ed25519(output, &signed_message_len, message, messageLength, privateKey);
	return signed_message_len;
}

#define PHANTASMA_RANDOMBYTES(buffer, size)                                                                randombytes_buf(buffer, size)
#define PHANTASMA_WIPEMEM(buffer, size)                                                                    sodium_memzero(buffer, size)
#define PHANTASMA_Ed25519_PublicKeyFromSeed(output, outputLength, seed, seedLength)                        Ed25519_PublicKeyFromSeed(output, outputLength, seed, seedLength)
#define PHANTASMA_Ed25519_PrivateKeyFromSeed(output, outputLength, seed, seedLength)                       Ed25519_PrivateKeyFromSeed(output, outputLength, seed, seedLength)
#define PHANTASMA_Ed25519_Sign(output, outputLength, message, messageLength, privateKey, privateKeyLength) Ed25519_Sign(output, outputLength, message, messageLength, privateKey, privateKeyLength)

}