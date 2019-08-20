#pragma once

#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#if !defined(PHANTASMA_Ed25519_PublicKeyFromSeed) || !defined(PHANTASMA_Ed25519_PrivateKeyFromSeed) || !defined(PHANTASMA_Ed25519_Sign)
#error "You must supply a Ed25519 implementation"
#endif

namespace phantasma {
namespace Ed25519 {

PHANTASMA_VECTOR<Byte> PublicKeyFromSeed( const Byte* bytes, int length )
{
	PHANTASMA_VECTOR<Byte> publicKey;
	publicKey.resize(32);
	PHANTASMA_Ed25519_PublicKeyFromSeed(&publicKey.front(), publicKey.size(), bytes, length);
	return publicKey;
}

PHANTASMA_VECTOR<Byte> ExpandedPrivateKeyFromSeed( const Byte* bytes, int length )
{
	PHANTASMA_VECTOR<Byte> privateKey;
	privateKey.resize(64);
	PHANTASMA_Ed25519_PrivateKeyFromSeed(&privateKey.front(), privateKey.size(), bytes, length);
	return privateKey;
}
PHANTASMA_VECTOR<Byte> ExpandedPrivateKeyFromSeed( const PHANTASMA_VECTOR<Byte>& bytes )
{
	return ExpandedPrivateKeyFromSeed(&bytes.front(), bytes.size());
}

PHANTASMA_VECTOR<Byte> Sign( const Byte* message, int messageLength, const Byte* expandedPrivateKey, int expandedPrivateKeyLength )
{
	PHANTASMA_VECTOR<Byte> signed_message;
	signed_message.resize(64 + messageLength);
	UInt64 size = PHANTASMA_Ed25519_Sign(&signed_message.front(), signed_message.size(), message, messageLength, expandedPrivateKey, expandedPrivateKeyLength);
	signed_message.resize((uint32_t)size);
	return signed_message;
}
PHANTASMA_VECTOR<Byte> Sign( const PHANTASMA_VECTOR<Byte>& message, const PHANTASMA_VECTOR<Byte>& expandedPrivateKey )
{
	return Sign(&message.front(), message.size(), &expandedPrivateKey.front(), expandedPrivateKey.size());
}

}}
