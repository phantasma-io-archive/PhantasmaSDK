#include "pch.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>                       // JSON library
#include <cpprest/ws_client.h>                  // WebSocket client
#include <locale>
#include <codecvt>
#include <string>
#include <nlohmann/json.hpp>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace http;						// Common HTTP functionality
using namespace client;						// HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace json;						// JSON library

#pragma once


struct Balance
{
	std::string chain;//
	std::string amount;//
	std::string symbol;//
	uint32_t decimals;//
	std::vector<std::string> ids;//
};

struct Account
{
	std::string address;//
	std::string name;//
	std::vector<Balance> balances;//
};

struct Chain
{
	std::string name;//
	std::string address;//
	std::string parentAddress;//
	uint32_t height;//
};

struct App
{
	std::string id;//
	std::string title;//
	std::string url;//
	std::string description;//
	std::string icon;//
};

struct Event
{
	std::string address;//
	std::string kind;//
	std::string data;//
};

struct Transaction
{
	std::string hash;//
	std::string chainAddress;//
	uint32_t timestamp;//
	uint32_t blockHeight;//
	std::string script;//
	std::vector<Event> events;//
	std::string result;//
};

struct AccountTransactions
{
	std::string address;//
	std::vector<Transaction> txs;//
};

struct Paginated
{
	uint32_t page;//
	uint32_t pageSize;//
	uint32_t total;//
	uint32_t totalPages;//
	json::value result;//
};

struct Block
{
	std::string hash;//
	std::string previousHash;//
	uint32_t timestamp;//
	uint32_t height;//
	std::string chainAddress;//
	std::string payload;//
	std::vector<Transaction> txs;//
	std::string validatorAddress;//
	std::string reward;//
};

struct TokenMetadata
{
	std::string key;//
	std::string value;//
};

struct Token
{
	std::string symbol;//
	std::string name;//
	uint32_t decimals;//
	std::string currentSupply;//
	std::string maxSupply;//
	std::string ownerAddress;//
	std::vector<TokenMetadata> metadataList;//
	std::string flags;//
};

struct TokenData
{
	std::string ID;//
	std::string chainAddress;//
	std::string ownerAddress;//
	std::string ram;//
	std::string rom;//
};

struct TxConfirmation
{
	std::string hash;//
	std::string chainAddress;//
	uint32_t confirmations;//
	uint32_t height;//
};

struct SendRawTx
{
	std::string hash;//
	std::string error;//
};

struct Auction
{
	std::string creatorAddress;//
	uint32_t startDate;//
	uint32_t endDate;//
	std::string baseSymbol;//
	std::string quoteSymbol;//
	std::string tokenId;//
	std::string price;//
};


class RPCRequest
{
private:
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;	//to convert from std::string to std::wstring
	nlohmann::json requestBody;

public:
	template<typename T>
	void AddParameter(T param);

	template<typename T, typename... Args>
	void AddParameter(T param, Args... args);

	RPCRequest(std::string);

	json::value BuildRequest();
};

class PhantasmaAPI
{
protected:
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;	//to convert from std::string to std::wstring
	
	std::wstring apiHost;

	std::string FieldToString(json::value json, const string_t field);
	uint32_t FieldToNumber(json::value json, const string_t field);

	json::value SendRequest(const json::value& body);

	Balance DeserializeBalance(json::value json);
	Account DeserializeAccount(json::value json);
	Chain DeserializeChain(json::value json);
	App DeserializeApp(json::value json);
	Event DeserializeEvent(json::value json);
	Transaction DeserializeTransaction(json::value json);
	AccountTransactions DeserializeAccountTransactions(json::value json);
	Paginated DeserializePaginated(json::value json);
	Block DeserializeBlock(json::value json);
	TokenMetadata DeserializeTokenMetadata(json::value json);
	Token DeserializeToken(json::value json);
	TokenData DeserializeTokenData(json::value json);
	TxConfirmation DeserializeTxConfirmation(json::value json);
	SendRawTx DeserializeSendRawTx(json::value json);
	Auction DeserializeAuction(json::value json);
	

public:
	PhantasmaAPI(std::string host);
	~PhantasmaAPI();

	//Returns the account name and balance of given address.
	Account GetAccount(std::string addressText);
	
	
	//Returns the height of a chain.
	uint32_t GetBlockHeight(std::string chainInput);
	
	
	//Returns the number of transactions of given block hash or error if given hash is invalid or is not found.
	uint32_t GetBlockTransactionCountByHash(std::string blockHash);
	
	
	//Returns information about a block by hash.
	Block GetBlockByHash(std::string blockHash);
	
	
	//Returns a serialized string, containing information about a block by hash.
	std::string GetRawBlockByHash(std::string blockHash);
	
	
	//Returns information about a block by height and chain.
	Block GetBlockByHeight(std::string chainInput, uint32_t height);
	
	
	//Returns a serialized string, in hex format, containing information about a block by height and chain.
	std::string GetRawBlockByHeight(std::string chainInput, uint32_t height);
	
	
	//Returns the information about a transaction requested by a block hash and transaction index.
	Transaction GetTransactionByBlockHashAndIndex(std::string blockHash, uint32_t index);
	
	
	//Returns last X transactions of given address.
	//paginated call
	AccountTransactions GetAddressTransactions(std::string addressText, uint32_t page, uint32_t pageSize);
	
	
	//Get number of transactions in a specific address and chain
	uint32_t GetAddressTransactionCount(std::string addressText, std::string chainInput);
	
	
	//Returns the number of confirmations of given transaction hash and other useful info.
	uint32_t GetConfirmations(std::string hashText);
	
	
	//Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually.
	std::string SendRawTransaction(std::string txData);
	
	
	//Returns information about a transaction by hash.
	Transaction GetTransaction(std::string hashText);
	
	
	//Removes a pending transaction from the mempool.
	std::string CancelTransaction(std::string hashText);
	
	
	//Returns an array of all chains deployed in Phantasma.
	std::vector<Chain> GetChains();
	
	
	//Returns an array of tokens deployed in Phantasma.
	std::vector<Token> GetTokens();
	
	
	//Returns info about a specific token deployed in Phantasma.
	Token GetToken(std::string symbol);
	
	
	//Returns data of a non-fungible token, in hexadecimal format.
	TokenData GetTokenData(std::string symbol, std::string IDtext);
	
	
	//Returns an array of apps deployed in Phantasma.
	std::vector<App> GetApps();
	
	
	//Returns last X transactions of given token.
	//paginated call
	std::vector<Transaction> GetTokenTransfers(std::string tokenSymbol, uint32_t page, uint32_t pageSize);
	
	
	//Returns the number of transaction of a given token.
	uint32_t GetTokenTransferCount(std::string tokenSymbol);
	
	
	//Returns the balance for a specific token and chain, given an address.
	Balance GetTokenBalance(std::string addressText, std::string tokenSymbol, std::string chainInput);
	
	
	//Returns the number of active auctions.
	uint32_t GetAuctionsCount(std::string symbol);
	
	
	//Returns the auctions available in the market.
	//paginated call
	std::vector<Auction> GetAuctions(std::string symbol, uint32_t page, uint32_t pageSize);
	
	
	
};

