#pragma once

#include "../Cryptography/Address.h"
#include "../Numerics/BigInteger.h"
#include "../Numerics/Base16.h"
#include <type_traits>

namespace phantasma {

enum class EventKind
{
	ChainCreate = 0,
	TokenCreate = 1,
	TokenSend = 2,
	TokenReceive = 3,
	TokenMint = 4,
	TokenBurn = 5,
	TokenEscrow = 6,
	TokenStake = 7,
	TokenUnstake = 8,
	TokenClaim = 9,
	MasterDemote = 10,
	MasterPromote = 11,
	AddressRegister = 12,
	AddressAdd = 13,
	AddressRemove = 14,
	GasEscrow = 15,
	GasPayment = 16,
	OrderCreated = 17,
	OrderCancelled = 18,
	OrderFilled = 19,
	OrderClosed = 20,
	AddFriend = 21,
	RemoveFriend = 22,
	FileCreate = 23,
	FileDelete = 24,
	ValidatorAdd = 25,
	ValidatorRemove = 26,
	ValidatorUpdate = 27,
	Metadata = 31,
	Custom = 32,
};

const Char* EventKindToString(EventKind k)
{
	switch(k)
	{
	case EventKind::ChainCreate:	 return PHANTASMA_LITERAL("ChainCreate");
	case EventKind::TokenCreate:	 return PHANTASMA_LITERAL("TokenCreate");
	case EventKind::TokenSend:		 return PHANTASMA_LITERAL("TokenSend");
	case EventKind::TokenReceive:	 return PHANTASMA_LITERAL("TokenReceive");
	case EventKind::TokenMint:		 return PHANTASMA_LITERAL("TokenMint");
	case EventKind::TokenBurn:		 return PHANTASMA_LITERAL("TokenBurn");
	case EventKind::TokenEscrow:	 return PHANTASMA_LITERAL("TokenEscrow");
	case EventKind::TokenStake:		 return PHANTASMA_LITERAL("TokenStake");
	case EventKind::TokenUnstake:	 return PHANTASMA_LITERAL("TokenUnstake");
	case EventKind::TokenClaim:		 return PHANTASMA_LITERAL("TokenClaim");
	case EventKind::MasterDemote:	 return PHANTASMA_LITERAL("MasterDemote");
	case EventKind::MasterPromote:	 return PHANTASMA_LITERAL("MasterPromote");
	case EventKind::AddressRegister: return PHANTASMA_LITERAL("AddressRegister");
	case EventKind::AddressAdd:		 return PHANTASMA_LITERAL("AddressAdd");
	case EventKind::AddressRemove:	 return PHANTASMA_LITERAL("AddressRemove");
	case EventKind::GasEscrow:		 return PHANTASMA_LITERAL("GasEscrow");
	case EventKind::GasPayment:		 return PHANTASMA_LITERAL("GasPayment");
	case EventKind::OrderCreated:	 return PHANTASMA_LITERAL("OrderCreated");
	case EventKind::OrderCancelled:	 return PHANTASMA_LITERAL("OrderCancelled");
	case EventKind::OrderFilled:	 return PHANTASMA_LITERAL("OrderFilled");
	case EventKind::OrderClosed:	 return PHANTASMA_LITERAL("OrderClosed");
	case EventKind::AddFriend:		 return PHANTASMA_LITERAL("AddFriend");
	case EventKind::RemoveFriend:	 return PHANTASMA_LITERAL("RemoveFriend");
	case EventKind::FileCreate:		 return PHANTASMA_LITERAL("FileCreate");
	case EventKind::FileDelete:		 return PHANTASMA_LITERAL("FileDelete");
	case EventKind::ValidatorAdd:	 return PHANTASMA_LITERAL("ValidatorAdd");
	case EventKind::ValidatorRemove: return PHANTASMA_LITERAL("ValidatorRemove");
	case EventKind::ValidatorUpdate: return PHANTASMA_LITERAL("ValidatorUpdate");
	case EventKind::Metadata:		 return PHANTASMA_LITERAL("Metadata");
	default:
	case EventKind::Custom:			 return PHANTASMA_LITERAL("Custom");
	}
}

EventKind StringToEventKind(const String& k)
{
	for( int i=0; i<=(int)EventKind::Custom; ++i )
	{
		if( 0 == k.compare(EventKindToString((EventKind)i)) )
			return (EventKind)i;
	}
	return EventKind::Custom;
}

class Event
{
public:
	const EventKind kind;
	const Address address;
	const ByteArray data;

	Event( EventKind kind, const Address& address, const ByteArray& data )
		: kind( kind )
		, address( address )
		, data( data )
	{
	}

	Event( const String& kind, const String& address, const String& data )
		: kind( StringToEventKind(kind) )
		, address( Address::FromText(address) )
		, data( data.empty() ? ByteArray() : Base16::Decode(data) )
	{
	}

	//String ToString()
	//{
	//	return $"{Kind} @ {Address}: {Base16.Encode(Data)}";
	//}

	template<class T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
	T GetKind()
	{
		return (T)(int)kind;
	}

	template<class T, class BinaryReader>
	T GetContent()
	{
		BinaryReader reader(data);
		return T::Unserialize(reader);
	}
	
	template<class BinaryWriter>
	void Serialize( BinaryWriter& writer )
	{
		int n = (int)kind;
		writer.Write( (Byte)n );
		writer.WriteAddress( address );
		writer.WriteByteArray( data );
	}

	template<class BinaryReader>
	static Event Unserialize( BinaryReader& reader )
	{
		EventKind kind = (EventKind)reader.ReadByte();
		Address address = reader.ReadAddress();
		ByteArray data = reader.ReadByteArray();
		return Event( kind, address, data );
	}
};

template<class T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
inline T DecodeCustomEvent( EventKind kind )
{
	if(kind < EventKind::Custom)
	{
		PHANTASMA_EXCEPTION( "Cannot cast system event" );
	}

	int intVal = ((int)kind - (int)EventKind.Custom);
	return (T)intVal;
}

template<class T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
inline EventKind EncodeCustomEvent( T kind )
{
	return (EventKind)(EventKind::Custom + (int)kind);
}

struct TokenEventData
{
	String symbol;
	BigInteger value;
	Address chainAddress;
};

struct TokenMetadata
{
	String key;
	ByteArray value;
};

struct MetadataEventData
{
	String symbol;
	TokenMetadata metadata;
};

}
