#pragma once

#include "../utils/Serializable.h"
#include "../utils/TextUtils.h"
#include "../Numerics/Base58.h"
#include "../Security/SecureString.h"
#include "EdDSA/Ed25519.h"

namespace phantasma {

enum class AddressKind
{
	Invalid = 0,
	User = 1,
	System = 2,
	Interop = 3,
};

class Address : public Serializable
{
public:
	static constexpr int TextLength = 47;
	static constexpr int LengthInBytes = 34;
	static constexpr int MaxPlatformNameLength = 10;
	static constexpr Byte NullPublicKey[LengthInBytes] = {};

	const String& Text() const
	{
		if(_text.empty())
		{
			const Char* prefix;
			switch (Kind())
			{
			case AddressKind::User: prefix = PHANTASMA_LITERAL("P"); break;
			case AddressKind::Interop: prefix = PHANTASMA_LITERAL("X"); break;
			default: prefix = PHANTASMA_LITERAL("S"); break;
			}
			_text.append(prefix);
			_text.append(Base58::Encode(_bytes, LengthInBytes));
		}
		return _text;
	}

	Address()
	{
		PHANTASMA_COPY(NullPublicKey, NullPublicKey+LengthInBytes, _bytes);
	}

	Address(const Byte* publicKey, int length)
	{
		if(!publicKey || length != LengthInBytes)
		{
			PHANTASMA_EXCEPTION("Invalid public key length");
			PHANTASMA_COPY(NullPublicKey, NullPublicKey+LengthInBytes, _bytes);
		}
		else
		{
			PHANTASMA_COPY(publicKey, publicKey+length, _bytes);
		}
	}

	Address(const ByteArray& publicKey)
		: Address(&publicKey.front(), (int)publicKey.size())
	{}

	template<class IKeyPair>
	static Address FromKey(const IKeyPair& key)
	{
		return FromKey(key.PublicKeyBytes(), key.PublicKeyLength());
	}
	static Address FromKey(const ByteArray& input)
	{
		if(input.empty())
		{
			PHANTASMA_EXCEPTION("Invalid public key length");
			return {};
		}
		return FromKey(&input.front(), (int)input.size());
	}
	static Address FromKey(const Byte* publicKeyBytes, int publicKeyLength)
	{
		Byte bytes[LengthInBytes] = {};
		bytes[0] = (Byte)AddressKind::User;
		if (publicKeyLength == 32)
		{
			PHANTASMA_COPY(publicKeyBytes, publicKeyBytes+publicKeyLength, bytes+2);
		}
		else if (publicKeyLength == 33)
		{
			PHANTASMA_COPY(publicKeyBytes, publicKeyBytes+publicKeyLength, bytes+1);
		}
		else
		{
			PHANTASMA_EXCEPTION("Invalid public key length");
			return {};
		}

		return Address(bytes, LengthInBytes);
	}

	static Address FromHash(const String& str)
	{
		ByteArray temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( str, temp, numBytes );
		return FromHash(bytes,numBytes);
	}
	
	static Address FromHash(const Byte* input, int inputLength)
	{
		Byte bytes[34];
		SHA256( bytes+2, 32, input, inputLength );
		bytes[0] = (Byte)AddressKind::System;
		bytes[1] = 0;
		return Address(bytes, 34);
	}

	AddressKind Kind() const { return IsNull() ? AddressKind::System : (AddressKind)_bytes[0]; }

	bool IsNull() const { return PHANTASMA_EQUAL(_bytes+1, _bytes + LengthInBytes - 1, NullPublicKey); };
	bool IsSystem() const { return Kind() == AddressKind::System;; }
	bool IsInterop() const { return Kind() == AddressKind::Interop; }
	bool IsUser() const { return Kind() == AddressKind::User; }
	
	bool operator ==( const Address& B ) const { return  PHANTASMA_EQUAL(_bytes, _bytes + LengthInBytes, B._bytes); }
	bool operator !=( const Address& B ) const { return !PHANTASMA_EQUAL(_bytes, _bytes + LengthInBytes, B._bytes); }

	String ToString() const
	{
		if (IsNull())
		{
			return String(PHANTASMA_LITERAL("[Null address]"));
		}
		return Text();
	}

	static Address FromWIF(const SecureString& wif)
	{
		return FromWIF(wif.c_str(), wif.length());
	}
	static Address FromWIF(const Char* wif, int wifStringLength);

	static Address FromText(const String& text, bool* out_error=0)
	{
		return FromText(text.c_str(), (int)text.length(), out_error);
	}
	static Address FromText(const Char* text, int textLength=0, bool* out_error=0)
	{
		if(textLength == 0)
		{
			textLength = (int)PHANTASMA_STRLEN(text);
			if(textLength == 0)
			{
				PHANTASMA_EXCEPTION("Invalid address data");
				if( out_error )
					*out_error = true;
				return {};
			}
		}

		Char prefix = text[0];
		Byte bytes[LengthInBytes+1];
		int decoded = Base58::Decode(bytes, LengthInBytes+1, text+1, textLength-1);

		if( decoded != LengthInBytes )
		{
			PHANTASMA_EXCEPTION("Invalid address data");
			if( out_error )
				*out_error = true;
			return {};
		}

		Address addr(bytes, decoded);
		switch (prefix)
		{
		case 'P':
			if(addr.Kind() != AddressKind::User)
			{
				PHANTASMA_EXCEPTION("address should be user");
				if( out_error )
					*out_error = true;
				return {};
			}
			break;

		case 'S':
			if(addr.Kind() != AddressKind::System)
			{
				PHANTASMA_EXCEPTION("address should be system");
				if( out_error )
					*out_error = true;
				return {};
			}
			break;

		case 'X':
			if(addr.Kind() < AddressKind::Interop)
			{
				PHANTASMA_EXCEPTION("address should be interop");
				if( out_error )
					*out_error = true;
				return {};
			}
			break;
		default:
			{
				StringBuilder sb;
				sb << "invalid address prefix: ";
				sb << prefix;
				PHANTASMA_EXCEPTION_MESSAGE("invalid address prefix", sb.str());
				if( out_error )
					*out_error = true;
				return {};
			}
			break;
		}
		return addr;
	}

	static bool IsValidAddress(const String& text)
	{
		PHANTASMA_TRY
		{
			bool error = false;
			Address addr = Address::FromText(text, &error);
			return !error;
		}
		PHANTASMA_CATCH_ALL()
		{
			return false;
		}
	}

	template<class BinaryWriter>
	void SerializeData(BinaryWriter& writer) const
	{
		writer.WriteByteArray(_bytes);
	}

	template<class BinaryReader>
	void UnserializeData(BinaryReader& reader)
	{
		reader.ReadByteArray(_bytes);
		_text = PHANTASMA_LITERAL("");
	}
	
	int DecodeInterop(Byte& out_platformID, Byte* out_publicKey, int publicKeyLength)
	{
		out_platformID = (Byte)(1 + _bytes[0] - (int)AddressKind::Interop);
		if(!out_publicKey || publicKeyLength < LengthInBytes - 1)
		{
			PHANTASMA_EXCEPTION("insufficient output space");
			return 0;
		}
		PHANTASMA_COPY(_bytes+1, _bytes+LengthInBytes, out_publicKey);
		return LengthInBytes - 1;
	}

	static Address FromInterop(Byte platformID, const Byte* publicKey, int publicKeyLength)
	{
		if(!publicKey || publicKeyLength != 33)
		{
			PHANTASMA_EXCEPTION("public key is invalid");
			return {};
		}
		if(platformID < 1)
		{
			PHANTASMA_EXCEPTION("invalid platform id");
			return {};
		}
	
		Byte bytes[LengthInBytes];
		bytes[0] = (Byte)((int)AddressKind::Interop+platformID-1);
		PHANTASMA_COPY(publicKey, publicKey+33, bytes+1);
		return Address(bytes, LengthInBytes);
	}

	const Byte* ToByteArray() const
	{
		return _bytes;
	}

	int GetSize() const
	{
		return LengthInBytes;
	}

private:
	Byte _bytes[LengthInBytes];
	mutable String _text;
};

inline void BinaryReader::ReadAddress(Address& address)
{
	ReadSerializable(address);
}

}
