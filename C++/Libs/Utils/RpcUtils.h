#pragma once

#include "../Domain/Event.h"

namespace phantasma {

const static BigInteger MinimumGasFee = 100000;
constexpr static int PaginationMaxResults = 50;

// Did an address receive a particular token type from this transaction?
// Who sent it, how many tokens were received?
inline bool GetTxTokensReceived(
	BigInteger& out_value, 
	String& out_addressFrom, 
	const rpc::Transaction& tx, 
	const PHANTASMA_VECTOR<rpc::Chain>& phantasmaChains, 
	const PHANTASMA_VECTOR<rpc::Token>& phantasmaTokens, 
	const String& addressTo, 
	const Char* tokenSymbol = PHANTASMA_LITERAL("SOUL"),
	const Char* chainName = PHANTASMA_LITERAL("main")
	)
{
	if(!tokenSymbol || tokenSymbol[0] == '\0')
	{
		PHANTASMA_EXCEPTION("Invalid usage - must provide a token");
		return false;
	}
	BigInteger value;
	String receivedChain;
	for (const auto& evt : tx.events)
	{
		if( evt.address != addressTo )
			continue;
		EventKind eventKind = StringToEventKind(evt.kind);
		if(eventKind == EventKind::TokenReceive)
		{
			TokenEventData data = Serialization<TokenEventData>::Unserialize(Base16::Decode(evt.data));
			if( 0!=data.symbol.compare(tokenSymbol) )
				continue;
			if( chainName && 0!=data.chainName.compare(chainName) )
				continue;
			if( data.value.IsZero() )
				continue;
			receivedChain = data.chainName;
			value = data.value;
			break;
		}
	}
	if( value.IsZero() )
		return false;
	for (const auto& evt : tx.events)
	{
		EventKind eventKind = StringToEventKind(evt.kind);
		if(eventKind == EventKind::TokenSend)
		{
			TokenEventData data = Serialization<TokenEventData>::Unserialize(Base16::Decode(evt.data));
			if( 0!=data.symbol.compare(tokenSymbol) )
				continue;
			if( data.chainName != receivedChain )
				continue;
			if( value != data.value )
				continue;
			out_value = value;
			out_addressFrom = evt.address;
			return true;
		}
	}
	return false;
}

inline String GetTxDescription(
	const rpc::Transaction& tx, 
	const PHANTASMA_VECTOR<rpc::Chain>& phantasmaChains, 
	const PHANTASMA_VECTOR<rpc::Token>& phantasmaTokens, 
	const String& addressFrom = String{})
{
	StringBuilder sb;
	String description;

	String senderToken;
	String senderChain;
	for(const auto& x : phantasmaChains)
	{
		if( x.address == tx.chainAddress )
		{
			senderChain = x.name;
			break;
		}
	}
	Address senderAddress;

	String receiverToken;
	String receiverChain;
	Address receiverAddress;

	BigInteger amount;

	for(const auto& evt : tx.events)
	{
		auto eventKind = StringToEventKind(evt.kind);
		switch (eventKind)
		{

		case EventKind::TokenClaim:
		{
			TokenEventData data = Serialization<TokenEventData>::Unserialize(Base16::Decode(evt.data));
			if (0==data.symbol.compare(PHANTASMA_LITERAL("SOUL")))
			{
				return String{PHANTASMA_LITERAL("Custom transaction")};
			}
		}
		break;


		case EventKind::TokenStake:
		{
			TokenEventData data = Serialization<TokenEventData>::Unserialize(Base16::Decode(evt.data));
			amount = data.value;
			if (amount >= 1000000000)
			{
				if (0!=data.symbol.compare(PHANTASMA_LITERAL("KCAL")) &&
					0!=data.symbol.compare(PHANTASMA_LITERAL("NEO")) &&
					0!=data.symbol.compare(PHANTASMA_LITERAL("GAS")) )
				{
					//return String{PHANTASMA_LITERAL("Stake transaction")};
					return String{PHANTASMA_LITERAL("Custom transaction")};
				}
			}
		}
		break;

		case EventKind::TokenMint:
		{
			return String{PHANTASMA_LITERAL("Claim transaction")};
		}
		break;

		case EventKind::AddressRegister:
		{
			auto name = Serialization<String>::Unserialize(Base16::Decode(evt.data));
			sb << PHANTASMA_LITERAL("Register transaction: name '");
			sb << name;
			sb << PHANTASMA_LITERAL("' registered");
			description = sb.str();
		}
		break;

		case EventKind::TokenSend:
		{
			TokenEventData data = Serialization<TokenEventData>::Unserialize(Base16::Decode(evt.data));
			amount = data.value;
			senderAddress = Address::FromText(evt.address);
			senderToken = data.symbol;
		}
		break;

		case EventKind::TokenReceive:
		{
			TokenEventData data = Serialization<TokenEventData>::Unserialize(Base16::Decode(evt.data));
			amount = data.value;
			receiverAddress = Address::FromText(evt.address);
			receiverChain = data.chainName;
			receiverToken = data.symbol;
		}
		break;

		}
	}

	if (description.empty())
	{
		if (amount > 0 && !senderAddress.IsNull() && !receiverAddress.IsNull() &&
			!senderToken.empty() && senderToken == receiverToken)
		{
			//auto amountDecimal = UnitConversion.ToDecimal(amount, phantasmaTokens.Single(p => p.Symbol == senderToken).Decimals);
			if(addressFrom.empty())
			{
				sb << PHANTASMA_LITERAL("Receive transaction: from ");
				sb << senderAddress.ToString();
				sb << PHANTASMA_LITERAL(" to ");
				sb << receiverAddress.ToString();
				description = sb.str();
			}
			else if (addressFrom == senderAddress.ToString())
			{
				sb << PHANTASMA_LITERAL("Send transaction: to ");
				sb << receiverAddress.ToString();
				description = sb.str();
			}
			else
			{
				sb << PHANTASMA_LITERAL("Receive transaction: from ");
				sb << senderAddress.ToString();
				description = sb.str();
			}

		}
		else if (amount > 0 && !receiverAddress.IsNull() && !receiverToken.empty())
		{
			//Int32 decimals = -1;
			//for(const auto& p : phantasmaTokens)
			//{
			//	if(p.symbol == receiverToken)
			//	{
			//		decimals = p.decimals;
			//		break;
			//	}
			//}
			//if(decimals < 0)
			//{
			//	PHANTASMA_EXCEPTION("Unknown token encountered");
			//}
			//var amountDecimal = UnitConversion.ToDecimal(amount, phantasmaTokens.Single(p => p.Symbol == receiverToken).Decimals);

			sb << PHANTASMA_LITERAL("Send transaction: to ");
			sb << receiverAddress.Text();
			sb << PHANTASMA_LITERAL(" ");
			description = sb.str();
		}
		else
		{
			description = PHANTASMA_LITERAL("Custom transaction");
		}

	}

	return description;
}

}
