#pragma once

#include "../Cryptography/SHA.h"
#include "../Cryptography/Hash.h"
#include "../Cryptography/Signature.h"
#include "../Cryptography/KeyPair.h"
#include "../Utils/Timestamp.h"
#include "../utils/Serializable.h"
#include "../utils/BinaryWriter.h"

namespace phantasma
{

class Transaction : public Serializable
{
	Timestamp m_expiration;
	ByteArray m_script;
	ByteArray m_payload;
	String m_nexusName;
	String m_chainName;
	PHANTASMA_VECTOR<Signature> m_signatures;
	Hash m_hash;
	struct StringToByteHelper
	{
		ByteArray buffer;
		int numBytes;
	};
public:
	const ByteArray& Script() const { return m_script; }
	const String     NexusName() const { return m_nexusName; }
	const String     ChainName() const { return m_chainName; }
	const Timestamp  Expiration() const { return m_expiration; }
	const ByteArray& Payload() const { return m_payload; }
	const Hash       GetHash() const { return m_hash; }
	const PHANTASMA_VECTOR<Signature> Signatures() const { return m_signatures; }

	template<class BinaryReader>
	static Transaction Unserialize( Byte* bytes, int numBytes )
	{
		BinaryReader reader( bytes, numBytes );
		return Unserialize( reader );
	}

	template<class BinaryReader>
	static Transaction Unserialize( BinaryReader& reader )
	{
		Transaction tx;
		tx.UnserializeData( reader );
		return tx;
	}

	template<class BinaryWriter>
	void Serialize( BinaryWriter& writer, bool withSignature ) const
	{
		writer.WriteVarString( m_nexusName );
		writer.WriteVarString( m_chainName );
		writer.WriteByteArray( m_script );
		writer.Write( m_expiration.Value );
		writer.WriteByteArray( m_payload );

		if( withSignature )
		{
			writer.WriteVarInt( m_signatures.size() );
			for( const auto& signature : m_signatures )
			{
				writer.WriteSignature( signature );
			}
		}
	}

	//String ToString()
	//{
	//	return String("{Hash}");//todo
	//}

	Transaction()
	{
	}

	Transaction( const Char* nexusName, const Char* chainName, const ByteArray& script, Timestamp expiration, const String& payload, StringToByteHelper temp={} )
		: Transaction(nexusName, chainName, script, expiration, GetUTF8Bytes(payload, temp.buffer, temp.numBytes), temp.numBytes)
	{
	}
	
    // transactions are always created unsigned, call Sign() to generate signatures
	Transaction( const Char* nexusName, const Char* chainName, const ByteArray& script, Timestamp expiration, const Byte* payload=0, int payloadLength=0 )
		: m_nexusName(nexusName)
		, m_chainName(chainName)
		, m_script(script)
		, m_expiration(expiration)
	{
		if(script.empty())
		{
			PHANTASMA_EXCEPTION("null script in transaction");
			return;
		}
		if(payload && payloadLength > 0)
		{
			m_payload.resize(payloadLength);
			PHANTASMA_COPY( payload, payload+payloadLength, m_payload.begin() );
		}
		UpdateHash();
	}

	ByteArray ToByteArray( bool withSignature ) const
	{
		BinaryWriter writer;
		Serialize( writer, withSignature );
		return writer.ToArray();
	}

//	String ToRawTransaction() const
//	{
//		return Base16::Encode(ToByteArray(true));
//

	bool HasSignatures() const
	{
		return !m_signatures.empty();
	}

	template<class IKeyPair>
	void Sign( const IKeyPair& keypair )
	{
		auto msg = ToByteArray( false );

		//m_signatures.clear();
		m_signatures.push_back( Signature{keypair.Sign( msg )} );
	}

	bool IsSignedBy( Address address )
	{
		return IsSignedBy( &address, 1 );
	}

	bool IsSignedBy( const Address* addresses, int numAddresses )
	{
		if( !HasSignatures() )
		{
			return false;
		}

		auto msg = ToByteArray( false );

		for(const auto& signature : m_signatures)
		{
			if(signature.Verify( &msg.front(), (int)msg.size(), addresses, numAddresses ))
			{
				return true;
			}
		}

		return false;
	}

	template<class Chain>
	bool IsValid(const Chain& chain) const
	{
		return (chain.Name() == m_chainName && chain.Nexus().Name() == m_nexusName);
	}

private:
	void UpdateHash()
	{
		auto data = ToByteArray( false );
		auto hash = SHA256( data );
		m_hash = Hash( hash );
	}
public:

	void SerializeData( BinaryWriter& writer ) const
	{
		Serialize( writer, true );
	}

	template<class BinaryReader>
	void UnserializeData( BinaryReader& reader )
	{
		m_nexusName = reader.ReadVarString();
		m_chainName = reader.ReadVarString();
		reader.ReadByteArray(m_script);
		m_expiration = reader.ReadUInt32();
		reader.ReadByteArray(m_payload);

		// check if we have some signatures attached
		PHANTASMA_TRY
		{
			int signatureCount = (int)reader.ReadVarInt();
			m_signatures.resize(signatureCount);
			for(int i = 0; i < signatureCount; i++)
			{
				m_signatures[i] = reader.ReadSignature();
			}
		}
		PHANTASMA_CATCH(...)
		{
			m_signatures.clear();
		}

		UpdateHash();
	}

	template<class ProofOfWork>
	void Mine( ProofOfWork targetDifficulty )
	{
		Mine( (int)targetDifficulty );
	}

	bool Mine( int targetDifficulty )
	{
		if( targetDifficulty < 0 || targetDifficulty > 256 )
		{
			PHANTASMA_EXCEPTION( "invalid difficulty" );
			return false;
		}
		if( m_signatures.size() > 0 )
		{
			PHANTASMA_EXCEPTION( "cannot be signed" );
			return false;
		}

		if(targetDifficulty == 0)
		{
			return true; // no mining necessary 
		}

		UInt32 nonce = 0;

		while(true)
		{
			if(GetHash().GetDifficulty() >= targetDifficulty)
			{
				return true;
			}

			if(nonce == 0)
			{
				m_payload = ByteArray(4);
			}

			nonce++;
			if(nonce == 0)
			{
				PHANTASMA_EXCEPTION( "Transaction mining failed" );
				return false;
			}

			m_payload[0] = (Byte)((nonce >> 0) & 0xFF);
			m_payload[1] = (Byte)((nonce >> 8) & 0xFF);
			m_payload[2] = (Byte)((nonce >> 16) & 0xFF);
			m_payload[3] = (Byte)((nonce >> 24) & 0xFF);
			UpdateHash();
		}
	}
};

}
