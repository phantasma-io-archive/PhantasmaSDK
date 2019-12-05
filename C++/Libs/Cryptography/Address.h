#pragma once

#include "../utils/Serializable.h"
#include "../Numerics/Base58.h"
#include "../Security/SecureString.h"
#include "EdDSA/Ed25519.h"

namespace phantasma {

class Address : public Serializable
{
public:
	static constexpr int TextLength = 45;
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
		{
			PHANTASMA_COPY(publicKey, publicKey+length, _publicKey);
			_opcode = (Byte)(IsInterop() ? 102 : 74);
		}
	}

	Address(const ByteArray& publicKey)
		: Address(&publicKey.front(), (int)publicKey.size())
	{}

	bool IsNull() const { return  PHANTASMA_EQUAL(_publicKey, _publicKey + PublicKeyLength, NullKey); };

	// NOTE currently we only support interop chain names with 3 chars, but this could be expanded to support up to 10 chars
	bool IsInterop() const
	{
		return !IsNull() && _publicKey[0] == (Byte)'*' && _publicKey[4] == (Byte)'*';
	}
	
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

	static Address FromWIF(const SecureString& wif)
	{
		return FromWIF(wif.c_str(), wif.length());
	}
	static Address FromWIF(const Char* wif, int wifStringLength)
	{
		if( !wif || wif[0] == '\0' || wifStringLength <= 0 )
		{
			PHANTASMA_EXCEPTION( "WIF required" );
			return Address();
		}
		Byte publicKey[32];
		{
			PinnedBytes<34> data;
			int size = Base58::CheckDecodeSecure(data.bytes, 34, wif, wifStringLength);
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
		return FromText(text.c_str(), (int)text.length(), out_error);
	}
	static Address FromText(const Char* text, int textLength=0, bool* out_error=0)
	{
		if(textLength == 0)
		{
			textLength = (int)PHANTASMA_STRLEN(text);
		}

		Byte bytes[PublicKeyLength+1];
		int decoded = 1;
		bool error = false;
		if(textLength != TextLength)
		{
			PHANTASMA_EXCEPTION("Invalid address length");
			error = true;
		}
		else
		{
			decoded = Base58::Decode(bytes, PublicKeyLength+1, text, textLength);
			if( decoded != PublicKeyLength+1 )
			{
				PHANTASMA_EXCEPTION("Invalid address encoding");
				error = true;
			}
			Byte opcode = bytes[0];
			if(opcode != 74 && opcode != 102)
			{
				PHANTASMA_EXCEPTION("Invalid address opcode");
				error = true;
			}
		}
		if( error )
		{
			if( out_error )
				*out_error = true;
			return Address();
		}
		return Address(bytes+1, decoded-1);
	}

	static Address FromScript(const ByteArray& script)
	{
		return Address(SHA256(script));
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

	template<class BinaryWriter>
	void SerializeData(BinaryWriter& writer) const
	{
		writer.WriteByteArray(_publicKey);
	}

	template<class BinaryReader>
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
