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

struct Chain {};//todo 

class Transaction : public Serializable
{
	Timestamp m_expiration;
	ByteArray m_script;
	String m_nexusName;
	String m_chainName;
	PHANTASMA_VECTOR<Signature> m_signatures;
	Hash m_hash;
public:
	const Timestamp Expiration() const { return m_expiration; }
	const ByteArray Script() const { return m_script; }

	const String NexusName() const { return m_nexusName; }
	const String ChainName() const { return m_chainName; }

	const PHANTASMA_VECTOR<Signature> Signatures() const { return m_signatures; }
	const Hash GetHash() const { return m_hash; }

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

private:
	// TODO should run the script and return true if sucess or false if exception
	bool Validate( Chain chain, BigInteger& fee )
	{
		fee = BigInteger::Zero();
		return true;
	}

	//bool Execute( Chain chain, Epoch epoch, Block block, StorageChangeSetContext changeSet, Action<Hash, Event> onNotify, OracleReaderDelegate oracleReader, ByteArray& result )
	//{
	//	result = ByteArray{};
	//
	//	var runtime = new RuntimeVM( this.Script, chain, epoch, block, this, changeSet, false );
	//	runtime.ThrowOnFault = true;
	//	runtime.OracleReader = oracleReader;
	//
	//	var state = runtime.Execute();
	//
	//	if(state != ExecutionState.Halt)
	//	{
	//		return false;
	//	}
	//
	//	var cost = runtime.UsedGas;
	//
	//	foreach ( var evt in runtime.Events )
	//	{
	//		onNotify( this.Hash, evt );
	//	}
	//
	//	if(runtime.Stack.Count > 0)
	//	{
	//		var obj = runtime.Stack.Pop();
	//		result = Serialization.Serialize( obj );
	//	}
	//
	//	return true;
	//}

public:
	Transaction()
	{
	}

	Transaction( const Char* nexusName, const Char* chainName, const ByteArray& script, Timestamp expiration, const PHANTASMA_VECTOR<Signature>* signatures = 0 )
		: m_nexusName(nexusName)
		, m_chainName(chainName)
		, m_script(script)
		, m_expiration(expiration)
		, m_signatures(signatures ? *signatures : PHANTASMA_VECTOR<Signature>{})
	{
		if(script.empty())
		{
			PHANTASMA_EXCEPTION("null script in transaction");
			return;
		}
		UpdateHash();
	}

	ByteArray ToByteArray( bool withSignature ) const
	{
		BinaryWriter writer;
		Serialize( writer, withSignature );
		return writer.ToArray();
	}

	String ToRawTransaction() const
	{
		return Base16::Encode(ToByteArray(true));
	}

	bool HasSignatures() const
	{
		return !m_signatures.empty();
	}

	void Sign( const Signature& signature )
	{
		for(const auto& existing : m_signatures)
		{
			if( existing == signature )
				return;
		}
		m_signatures.push_back( signature );
	}

	void Sign( const KeyPair& owner )
	{
		auto msg = ToByteArray( false );

		m_signatures.clear();
		m_signatures.push_back( Signature{owner.Sign( msg )} );
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

	bool IsValid( Chain chain )
	{
		//todo
		//if(chain.Name != this.ChainName)
		//{
		//	return false;
		//}
		//
		//if(chain.Nexus.Name != this.NexusName)
		//{
		//	return false;
		//}

		// TODO unsigned tx should be supported too
		/* if (!IsSigned)
			{
				return false;
			}

			var data = ToArray(false);
			if (!this.Signature.Verify(data, this.SourceAddress))
			{
				return false;
			}*/

		BigInteger cost;
		bool validation = Validate( chain, cost );
		if(!validation)
		{
			return false;
		}

		/*if (chain.NativeTokenAddress != null)
		{
			if (this.Fee < cost)
			{
				return false;
			}

			var balance = chain.GetTokenBalance(chain.NativeTokenAddress, this.SourceAddress);

			if (balance < this.Fee)
			{
				return false;
			}
		}*/

		return true;
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
};

}
