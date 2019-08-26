#pragma once

#include "../../Security/SecureMemory.h"

#if !defined(PHANTASMA_Ed25519_PublicKeyFromSeed) || !defined(PHANTASMA_Ed25519_PrivateKeyFromSeed) || !defined(PHANTASMA_Ed25519_SignDetached) || !defined(PHANTASMA_Ed25519_ValidateDetached)
#error "You must supply a Ed25519 implementation"
#endif

namespace phantasma {
namespace Ed25519 {

void PublicKeyFromSeed( Byte* output, int outputSize, const Byte* seed, int seedLength )
{
	if(!output || !seed || outputSize != 32 || seedLength != 32 )
	{
		PHANTASMA_EXCEPTION("Invalid arguments");
		return;
	}
	PHANTASMA_Ed25519_PublicKeyFromSeed(output, 32, seed, 32);
}
ByteArray PublicKeyFromSeed( const Byte* seed, int seedLength )
{
	ByteArray publicKey;
	publicKey.resize(32);
	PublicKeyFromSeed(&publicKey.front(), (int)publicKey.size(), seed, seedLength);
	return publicKey;
}

void ExpandedPrivateKeyFromSeed( Byte* output, int outputSize, const Byte* seed, int seedLength )
{
	if(!output || !seed || outputSize != 64 || seedLength != 32 )
	{
		PHANTASMA_EXCEPTION("Invalid arguments");
		return;
	}
	PHANTASMA_Ed25519_PrivateKeyFromSeed(output, 64, seed, 32);
}

ByteArray Sign( const Byte* message, int messageLength, const Byte* expandedPrivateKey, int expandedPrivateKeyLength )
{
	if( !message || !expandedPrivateKey )
		return ByteArray{};
	ByteArray signed_message;
	signed_message.resize(64);
	UInt64 size = PHANTASMA_Ed25519_SignDetached(&signed_message.front(), (int)signed_message.size(), message, messageLength, expandedPrivateKey, expandedPrivateKeyLength);
	signed_message.resize((uint32_t)size);
	return signed_message;
}

bool Verify( const Byte* signature, int signatureLength, const Byte* message, int messageLength, const Byte* publicKey, int publicKeyLength )
{
	if( !signature || !message || !publicKey )
		return false;
	return PHANTASMA_Ed25519_ValidateDetached(signature, signatureLength, message, messageLength, publicKey, publicKeyLength);
}

}}
