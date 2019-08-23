#pragma once

#include "../utils/Serializable.h"
#include "../Numerics/Base58.h"
#include "EdDSA/Ed25519.h"

namespace phantasma {

class Address : public Serializable
{
public:
	static constexpr int PublicKeyLength = 32;
	static constexpr Byte NullKey[PublicKeyLength] = {};

	const Byte* PublicKey() const
	{
		return _publicKey;
	}

	const String& Text() const
	{
		if(_text.empty())
			_text = Base58::Encode(&_opcode, PublicKeyLength+1);
		return _text;
	}

	Address()
	{
		PHANTASMA_COPY(NullKey, NullKey+PublicKeyLength, _publicKey);
	}

	Address(const Byte* publicKey, int length)
	{
		if(!publicKey || length != PublicKeyLength)
		{
			PHANTASMA_EXCEPTION("Invalid public key length");
			PHANTASMA_COPY(NullKey, NullKey+PublicKeyLength, _publicKey);
		}
		else
			PHANTASMA_COPY(publicKey, publicKey+length, _publicKey);
	}

	Address(const PHANTASMA_VECTOR<Byte>& publicKey)
		: Address(&publicKey.front(), (int)publicKey.size())
	{}

	bool IsNull() const { return  PHANTASMA_EQUAL(_publicKey, _publicKey + PublicKeyLength, NullKey); };
	
	bool operator ==( const Address& B ) const { return  PHANTASMA_EQUAL(_publicKey, _publicKey + PublicKeyLength, B._publicKey); }
	bool operator !=( const Address& B ) const { return !PHANTASMA_EQUAL(_publicKey, _publicKey + PublicKeyLength, B._publicKey); }

	String ToString() const
	{
		if (PHANTASMA_EQUAL(_publicKey, _publicKey + PublicKeyLength, NullKey))
		{
			return String(PHANTASMA_LITERAL("[Null address]"));
		}
		return Text();
	}

	static Address FromWIF(const String& wif)//todo - secure memory string
	{
		if( wif.empty() )
		{
			PHANTASMA_EXCEPTION( "WIF required" );
			return Address();
		}
		Byte publicKey[32];
		{
			PinnedBytes<34> data;
			int size = Base58::DecodeSecure(data.bytes, 34, wif);
			if( size != 34 || data.bytes[0] != 0x80 || data.bytes[33] != 0x01 )
			{
				PHANTASMA_EXCEPTION( "Invalid WIF format" );
				return Address();
			}
			Ed25519::PublicKeyFromSeed( publicKey, 32, &data.bytes[1], 32 );
		}
		return Address( publicKey, 32 );
	}

	static Address FromText(const String& text, bool* out_error=0)
	{
		if(text.length() != 45)
		{
			PHANTASMA_EXCEPTION("Invalid address length");
			if( out_error )
				*out_error = true;
			return Address();
		}

		auto bytes = Base58::Decode(text);
		Byte opcode = bytes[0];

		if(opcode != 74)
		{
			PHANTASMA_EXCEPTION("Invalid address opcode");
			if( out_error )
				*out_error = true;
			return Address();
		}

		return Address(&bytes.front()+1, (int)bytes.size()-1);
	}

	template<class ScriptBytes>//TODO (tricking compiler into accepting this code...)
	static Address FromScript(const ScriptBytes& script)
	{
		auto hash = script.SHA256();
		return Address(hash);
	}

	int GetSize() const
	{
		return PublicKeyLength;
	}

	static bool IsValidAddress(const String& text)
	{
		PHANTASMA_TRY
		{
			bool error = false;
			Address addr = Address::FromText(text, &error);
			return !error;
		}
		PHANTASMA_CATCH(...)
		{
			return false;
		}
	}

	template<class BinaryWriter>//TODO (tricking compiler into accepting this code...)
	void SerializeData(BinaryWriter& writer) const
	{
		writer.WriteByteArray(_publicKey);
	}

	template<class BinaryReader>//TODO (tricking compiler into accepting this code...)
	void UnserializeData(BinaryReader& reader)
	{
		reader.ReadByteArray(_publicKey);
		_text = "";
	}

private:
	Byte _opcode = 74;
	Byte _publicKey[PublicKeyLength];
	mutable String _text;
};

}
