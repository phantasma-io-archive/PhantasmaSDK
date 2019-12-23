#pragma once

#include "../Cryptography/Address.h"
#include "../Cryptography/Hash.h"
#include "../Numerics/BigInteger.h"
#include "../Numerics/Base16.h"
#include <type_traits>

namespace phantasma {

enum class EventKind
{
	Unknown = 0,
	ChainCreate = 1,
	TokenCreate = 2,
	TokenSend = 3,
	TokenReceive = 4,
	TokenMint = 5,
	TokenBurn = 6,
	TokenStake = 7,
	TokenClaim = 8,
	AddressRegister = 9,
	AddressLink = 10,
	AddressUnlink = 11,
	OrganizationCreate = 12,
	OrganizationAdd = 13,
	OrganizationRemove = 14,
	GasEscrow = 15,
	GasPayment = 16,
	AddressUnregister = 17,
	OrderCreated = 18,
	OrderCancelled = 19,
	OrderFilled = 20,
	OrderClosed = 21,
	FeedCreate = 22,
	FeedUpdate = 23,
	FileCreate = 24,
	FileDelete = 25,
	ValidatorPropose = 26,
	ValidatorElect = 27,
	ValidatorRemove = 28,
	ValidatorSwitch = 29,
	PackedNFT = 30,
	ValueCreate = 31,
	ValueUpdate = 32,
	PollCreated = 33,
	PollClosed = 34,
	PollVote = 35,
	ChannelCreate = 36,
	ChannelRefill = 37,
	ChannelSettle = 38,
	LeaderboardCreate = 39,
	LeaderboardInsert = 40,
	LeaderboardReset = 41,
	PlatformCreate = 42,
	ChainSwap = 43,
	ContractRegister = 44,
	ContractDeploy = 45,
	AddressMigration = 46,
	ContractUpgrade = 47,
	Log = 48,
	Custom = 64,
};

const Char* EventKindToString(EventKind k)
{
	switch(k)
	{
	case EventKind::Unknown:			 return PHANTASMA_LITERAL("Unknown");
	case EventKind::ChainCreate:		 return PHANTASMA_LITERAL("ChainCreate");
	case EventKind::TokenCreate:		 return PHANTASMA_LITERAL("TokenCreate");
	case EventKind::TokenSend:			 return PHANTASMA_LITERAL("TokenSend");
	case EventKind::TokenReceive:		 return PHANTASMA_LITERAL("TokenReceive");
	case EventKind::TokenMint:			 return PHANTASMA_LITERAL("TokenMint");
	case EventKind::TokenBurn:			 return PHANTASMA_LITERAL("TokenBurn");
	case EventKind::TokenStake:			 return PHANTASMA_LITERAL("TokenStake");
	case EventKind::TokenClaim:			 return PHANTASMA_LITERAL("TokenClaim");
	case EventKind::AddressRegister:	 return PHANTASMA_LITERAL("AddressRegister");
	case EventKind::AddressLink:		 return PHANTASMA_LITERAL("AddressLink");
	case EventKind::AddressUnlink:		 return PHANTASMA_LITERAL("AddressUnlink");
	case EventKind::OrganizationCreate:	 return PHANTASMA_LITERAL("OrganizationCreate");
	case EventKind::OrganizationAdd:	 return PHANTASMA_LITERAL("OrganizationAdd");
	case EventKind::OrganizationRemove:	 return PHANTASMA_LITERAL("OrganizationRemove");
	case EventKind::GasEscrow:			 return PHANTASMA_LITERAL("GasEscrow");
	case EventKind::GasPayment:			 return PHANTASMA_LITERAL("GasPayment");
	case EventKind::AddressUnregister:	 return PHANTASMA_LITERAL("AddressUnregister");
	case EventKind::OrderCreated:		 return PHANTASMA_LITERAL("OrderCreated");
	case EventKind::OrderCancelled:		 return PHANTASMA_LITERAL("OrderCancelled");
	case EventKind::OrderFilled:		 return PHANTASMA_LITERAL("OrderFilled");
	case EventKind::OrderClosed:		 return PHANTASMA_LITERAL("OrderClosed");
	case EventKind::FeedCreate:			 return PHANTASMA_LITERAL("FeedCreate");
	case EventKind::FeedUpdate:			 return PHANTASMA_LITERAL("FeedUpdate");
	case EventKind::FileCreate:			 return PHANTASMA_LITERAL("FileCreate");
	case EventKind::FileDelete:			 return PHANTASMA_LITERAL("FileDelete");
	case EventKind::ValidatorPropose:	 return PHANTASMA_LITERAL("ValidatorPropose");
	case EventKind::ValidatorElect:		 return PHANTASMA_LITERAL("ValidatorElect");
	case EventKind::ValidatorRemove:	 return PHANTASMA_LITERAL("ValidatorRemove");
	case EventKind::ValidatorSwitch:	 return PHANTASMA_LITERAL("ValidatorSwitch");
	case EventKind::PackedNFT:			 return PHANTASMA_LITERAL("PackedNFT");
	case EventKind::ValueCreate:		 return PHANTASMA_LITERAL("ValueCreate");
	case EventKind::ValueUpdate:		 return PHANTASMA_LITERAL("ValueUpdate");
	case EventKind::PollCreated:		 return PHANTASMA_LITERAL("PollCreated");
	case EventKind::PollClosed:			 return PHANTASMA_LITERAL("PollClosed");
	case EventKind::PollVote:			 return PHANTASMA_LITERAL("PollVote");
	case EventKind::ChannelCreate:		 return PHANTASMA_LITERAL("ChannelCreate");
	case EventKind::ChannelRefill:		 return PHANTASMA_LITERAL("ChannelRefill");
	case EventKind::ChannelSettle:		 return PHANTASMA_LITERAL("ChannelSettle");
	case EventKind::LeaderboardCreate:	 return PHANTASMA_LITERAL("LeaderboardCreate");
	case EventKind::LeaderboardInsert:	 return PHANTASMA_LITERAL("LeaderboardInsert");
	case EventKind::LeaderboardReset:	 return PHANTASMA_LITERAL("LeaderboardReset");
	case EventKind::PlatformCreate:		 return PHANTASMA_LITERAL("PlatformCreate");
	case EventKind::ChainSwap:			 return PHANTASMA_LITERAL("ChainSwap");
	case EventKind::ContractRegister:	 return PHANTASMA_LITERAL("ContractRegister");
	case EventKind::ContractDeploy:		 return PHANTASMA_LITERAL("ContractDeploy");
	case EventKind::AddressMigration:	 return PHANTASMA_LITERAL("AddressMigration");
	case EventKind::ContractUpgrade:	 return PHANTASMA_LITERAL("ContractUpgrade");
	case EventKind::Log:				 return PHANTASMA_LITERAL("Log");
	default:
	case EventKind::Custom:				 return PHANTASMA_LITERAL("Custom");
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

class TokenEventData
{
public:
	const String symbol;
	const BigInteger value;
	const String chainName;

	TokenEventData(){}
	TokenEventData(const String& symbol, const BigInteger& value, const String& chainName)
		: symbol(symbol)
		, value(value)
		, chainName(chainName)
	{
	}

	template<class BinaryReader>
	static TokenEventData Unserialize( BinaryReader& reader )
	{
		String symbol;
		BigInteger value;
		String chainName;
		reader.ReadVarString(symbol);
		reader.ReadBigInteger(value);
		reader.ReadVarString(chainName);
		return { symbol, value, chainName };
	}
};

class ChainValueEventData
{
public:
	String name;
	BigInteger value;
};

class TransactionSettleEventData
{
public:
	const Hash hash;
	const String platform;
	const String chain;

	TransactionSettleEventData(const Hash& hash, const String& platform, const String& chain)
		: hash(hash)
		, platform(platform)
		, chain(chain)
	{
	}
};

class GasEventData
{
public:
	const Address address;
	const BigInteger price;
	const BigInteger amount;

	GasEventData(const Address& address, const BigInteger& price, const BigInteger& amount)
		: address(address)
		, price(price)
		, amount(amount)
	{
	}
};

class Event
{
public:
	const EventKind kind;
	const Address address;
	const String contract;
	const ByteArray data;

	Event( EventKind kind, const Address& address, const String& contract, const ByteArray& data )
		: kind( kind )
		, address( address )
		, contract( contract )
		, data( data )
	{
	}

	Event( const String& kind, const String& address, const String& contract, const String& data )
		: kind( StringToEventKind(kind) )
		, address( Address::FromText(address) )
		, contract( contract )
		, data( data.empty() ? ByteArray() : Base16::Decode(data) )
	{
	}

	//String ToString()
	//{
	//	return $"{Kind}/{Contract} @ {Address}: {Base16.Encode(Data)}";
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
		writer.WriteVarString(contract);
		writer.WriteByteArray( data );
	}

	template<class BinaryReader>
	static Event Unserialize( BinaryReader& reader )
	{
		EventKind kind = (EventKind)reader.ReadByte();
		Address address = reader.ReadAddress();
		String contract = reader.ReadVarString();
		ByteArray data = reader.ReadByteArray();
		return Event( kind, address, contract, data );
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

}
