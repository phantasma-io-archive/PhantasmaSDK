#pragma once
//------------------------------------------------------------------------------
// Low-level API
//------------------------------------------------------------------------------
//  The PhantasmaJsonAPI namespace can construct JSON requests and parse JSON responses,
//   but you are responsible for sending/receiving these messages via HTTP on your own.
//   You can call PhantasmaJsonAPI::Uri() to determine where to send them.
//
// void PhantasmaJsonAPI::Make{Message}Request(JSONBuilder&, {Parameters});
// bool PhantasmaJsonAPI::Parse{Message}Response(const JSONValue&, {Output});
//
//------------------------------------------------------------------------------
// High-level API
//------------------------------------------------------------------------------
//  If you have defined PHANTASMA_HTTPCLIENT, then you can construct a PhantasmaAPI object,
//   which provides a simplified API that hides the internal JSON messaging.
//
// PhantasmaAPI phantasmaAPI(httpClient);
// {Output} phantasmaAPI->{Message}({Parameters});
//
//------------------------------------------------------------------------------
// API configuration
//------------------------------------------------------------------------------
// As different C++ projects may use different primitive types, you can use the 
//  following #defines (BEFORE including phantasma.h) to override the default types.
//
// #define               | typedef                | Default           | Notes
// PHANTASMA_INT32       | phantasma::Int32       | int32_t           |
// PHANTASMA_UINT32      | phantasma::UInt32      | uint32_t          |
// PHANTASMA_CHAR        | phantasma::Char        | char              | See Unicode section
// PHANTASMA_STRING      | phantasma::String      | std::string       | Must support construction from `const phantasma::Char*`
// PHANTASMA_VECTOR      |                        | std::vector       | Must support `push_back` and `size` members
// PHANTASMA_JSONVALUE   | phantasma::JSONValue   | std::string_view  | See JSON section
// PHANTASMA_JSONARRAY   | phantasma::JSONArray   | JSONValue         | See JSON section
// PHANTASMA_JSONDOCUMENT| phantasma::JSONDocument| std::string       | See JSON section
// PHANTASMA_JSONBUILDER | phantasma::JSONBuilder | std::stringstream*| See JSON section
// PHANTASMA_HTTPCLIENT  | phantasma::HttpClient  |                   | See HTTP section
//
// The behavior of this header can further be modified by using the following 
//  #defines (BEFORE including phantasma.h)
// 
// #define                                      | Notes                   
// PHANTASMA_EXCEPTION(message)                 | See Exceptions section
// PHANTASMA_EXCEPTION_MESSAGE(message, String) | See Exceptions section
// PHANTASMA_LITERAL(x)                         | See Unicode section
// PHANTASMA_FUNCTION                           | See Integration section         
// PHANTASMA_IMPLEMENTATION                     | See Integration section
//
//------------------------------------------------------------------------------
// Integration
//------------------------------------------------------------------------------
// This API is provided in the "single header" style to support simple and flexible
//  integration into your project (see https://github.com/nothings/single_file_libs).
//  The implementation of function bodies will be excluded unless you define
//  PHANTASMA_IMPLEMENTATION before including phantasma.h.
//
// Typical linking:
//  In one CPP file, before including phantasma.h:
//   #define PHANTASMA_IMPLEMENTATION
// 
// Inline linking:
//  In every CPP file that uses the API, before including phantasma.h:
//   #define PHANTASMA_IMPLEMENTATION
//   #define PHANTASMA_FUNCTION inline
//
// Aside from PHANTASMA_IMPLEMENTATION / PHANTASMA_FUNCTION, you should take care
//  to ensure that every other PHANTASMA_* macro is defined to the same value in
//  all of yoru CPP files that use the phantasma API.
//
//------------------------------------------------------------------------------
// Exceptions
//------------------------------------------------------------------------------
// Support for C++ exceptions is opt-in. Define the following (or an alternative
//  based on your own exception classes) before including phantasma.h:
// #define PHANTASMA_EXCEPTION(message)                 throw std::runtime_error(message)
// #define PHANTASMA_EXCEPTION_MESSAGE(message, string) throw std::runtime_error(string)
//
//------------------------------------------------------------------------------
// Unicode
//------------------------------------------------------------------------------
// To build a wide-character version of the API, define the following before
//  including phantasma.h:
// #define PHANTASMA_CHAR       wchar_t
// #define PHANTASMA_LITERAL(x) L ## x
// #define PHANTASMA_STRING     std::wstring
//
// You should also provide a JSON and HTTP library with wide-character support.
//
//------------------------------------------------------------------------------
// JSON
//------------------------------------------------------------------------------
// This header contains JSON parsing and building code, but it is written to be
//  as simple as possible (approx 200 lines of code) and is not high-performance
//  or highly robust.
//
// It is recommended that you supply another JSON-parsing API, by defining the
//  following macros before including phantasma.h:
// #define PHANTASMA_JSONVALUE    Your_Json_Value_Type
// #define PHANTASMA_JSONARRAY    Your_Json_Array_Type
// #define PHANTASMA_JSONDOCUMENT Your_JSON_Document_Type
// #define PHANTASMA_JSONBUILDER  Your_Json_Serializer_Type
//
// Also, this header uses the following procedural API to interact with these types.
// If you have supplied your own JSON types, you must implement the following functions:
//
// namespace phantasma { namespace json {
//
//    JSONValue Parse(const JSONDocument&);
//
//    Int32     LookupInt32(  const JSONValue&, const Char* field, bool& out_error);
//    UInt32    LookupUInt32( const JSONValue&, const Char* field, bool& out_error);
//    String    LookupString( const JSONValue&, const Char* field, bool& out_error);
//    JSONValue LookupValue(  const JSONValue&, const Char* field, bool& out_error);
//    JSONArray LookupArray(  const JSONValue&, const Char* field, bool& out_error);
//    bool      HasField(     const JSONValue&, const Char* field, bool& out_error);
//    bool      HasArrayField(const JSONValue&, const Char* field, bool& out_error);
//
//    Int32     AsInt32(      const JSONValue&,                    bool& out_error);
//    UInt32    AsUInt32(     const JSONValue&,                    bool& out_error);
//    String    AsString(     const JSONValue&,                    bool& out_error);
//    JSONArray AsArray(      const JSONValue&,                    bool& out_error);
//    bool      IsArray(      const JSONValue&,                    bool& out_error);
//    bool      IsObject(     const JSONValue&,                    bool& out_error);
//    
//    int       ArraySize(    const JSONArray&,                    bool& out_error);
//    JSONValue IndexArray(   const JSONArray&, int index,         bool& out_error);
//
//                           void BeginObject(JSONBuilder&);
//                           void AddString  (JSONBuilder&, const Char* key, const Char* value);
//   template<class... Args> void AddArray   (JSONBuilder&, const Char* key, Args...);
//                           void EndObject  (JSONBuilder&);
// }}
//
//------------------------------------------------------------------------------
// HTTP
//------------------------------------------------------------------------------
// This header does not contain a HTTP client, nor a dependency on any specific
//  HTTP client library. If you do not supply a HTTP client library, then only
//  the Low-level phantasma API (PhantasmaJsonAPI) is available.
//
// To enable the PhantasmaAPI class, defining the following macro before 
//  including phantasma.h:
// #define PHANTASMA_HTTPCLIENT   Your_HTTP_Client_Type
//
// Also, this header uses the following procedural API to interact with this type.
// If you have defined PHANTASMA_HTTPCLIENT, you must implement the following,
//  function, which should perform a HTTP POST request and return the result:
//
// namespace phantasma {
//  JSONDocument HttpPost(HttpClient&, const Char* uri, const JSONBuilder&);
// }
//
//------------------------------------------------------------------------------

#if !defined(PHANTASMA_STRING) || !defined(PHANTASMA_JSONDOCUMENT) || !defined(PHANTASMA_JSONVALUE)
#include <string>
#endif

#if !defined(PHANTASMA_JSONVALUE) && __cplusplus > 201402L
#include <string_view>
#endif

#if !defined(PHANTASMA_JSONBUILDER)
#include <sstream>
#endif

#if !defined(PHANTASMA_VECTOR)
#define PHANTASMA_VECTOR std::vector
#include <vector>
#endif

#if !defined(PHANTASMA_S32) || !defined(PHANTASMA_U32)
#include <cstdint>
#endif

#if !defined(PHANTASMA_EXCEPTION)
#define PHANTASMA_EXCEPTION(literal)
#define PHANTASMA_EXCEPTION_MESSAGE(literal, string)
#endif

#if !defined(PHANTASMA_LITERAL)
#define PHANTASMA_LITERAL(x) x
#endif

#if !defined(PHANTASMA_FUNCTION)
#define PHANTASMA_FUNCTION
#endif

namespace phantasma
{
#ifdef PHANTASMA_CHAR
typedef PHANTASMA_CHAR Char;
#else
typedef char Char;
#endif

#ifdef PHANTASMA_INT32
typedef PHANTASMA_INT32 Int32;
#else
typedef int32_t Int32;
#endif

#ifdef PHANTASMA_UINT32
typedef PHANTASMA_UINT32 UInt32;
#else
typedef uint32_t UInt32;
#endif

#ifdef PHANTASMA_STRING
typedef PHANTASMA_STRING String;
#else
typedef std::string String;
#endif

#ifdef PHANTASMA_JSONVALUE
typedef PHANTASMA_JSONVALUE JSONValue;
#elif __cplusplus > 201402L
typedef std::string_view JSONValue;
#else
typedef std::string JSONValue;
#endif

#ifdef PHANTASMA_JSONARRAY
typedef PHANTASMA_JSONARRAY JSONArray;
#else
typedef JSONValue JSONArray;
#endif

#ifdef PHANTASMA_JSONDOCUMENT
typedef PHANTASMA_JSONDOCUMENT JSONDocument;
#else
typedef std::string JSONDocument;
#endif


#ifdef PHANTASMA_JSONBUILDER
typedef PHANTASMA_JSONBUILDER JSONBuilder;
#else
struct JSONBuilder // A VERY simple json string builder. Highly recommended that you provide a real JSON library instead!
{
	std::stringstream s;
	bool empty = true;
	operator std::stringstream&() { return s; }
	void AddKey(const Char* key) { if(!empty) { s << ", "; } empty = false; s << '"' << key << "\": "; }
	void AddValues() {}
	void AddValues(const char* arg) { s << '"' << arg << '"'; }
	template<class T> void AddValues(T arg) { s << arg; }
	template<class T, class... Args> void AddValues(T arg0, Args... args) { AddValues(arg0); s << ", "; AddValues(args...); }
	
	void BeginObject() { s << "{"; }
	void AddString(const Char* key, const Char* value) { AddKey(key); s << '"' << value << '"'; }
	template<class... Args> void AddArray(const Char* key, Args... args) { AddKey(key); s << '['; AddValues(args...); s << ']'; }
	void EndObject() { s << "}"; }
};
#endif

#ifdef PHANTASMA_HTTPCLIENT
typedef PHANTASMA_HTTPCLIENT HttpClient;
//JSONDocument HttpPost(HttpClient&, const Char* uri, const JSONBuilder&);
#endif

//If providing a JSON library (highly recommended that you do!), then you must provide these functions yourself:
namespace json
{
#ifndef PHANTASMA_JSONBUILDER
    JSONValue Parse(const JSONDocument&);
	Int32 LookupInt32(const JSONValue&, const Char* field, bool& out_error);
	UInt32 LookupUInt32(const JSONValue&, const Char* field, bool& out_error);
	String LookupString(const JSONValue&, const Char* field, bool& out_error);
	JSONValue LookupValue(const JSONValue&, const Char* field, bool& out_error);
	JSONArray LookupArray(const JSONValue&, const Char* field, bool& out_error);
	bool HasField(const JSONValue&, const Char* field, bool& out_error);
	bool HasArrayField(const JSONValue&, const Char* field, bool& out_error);
	Int32  AsInt32(const JSONValue&, bool& out_error);
	UInt32 AsUInt32(const JSONValue&, bool& out_error);
	String AsString(const JSONValue&, bool& out_error);
	JSONArray AsArray(const JSONValue&, bool& out_error);
	bool IsArray(const JSONValue&, bool& out_error);
	bool IsObject(const JSONValue&, bool& out_error);

	int ArraySize(const JSONArray&, bool& out_error);
	JSONValue IndexArray(const JSONArray&, int index, bool& out_error);

	void BeginObject(JSONBuilder&);
	void AddString(JSONBuilder&, const Char* key, const Char* value);
	template<class... Args>
	void AddArray(JSONBuilder&, const Char* key, Args... args);
	void EndObject(JSONBuilder&);
#endif
}


struct Balance
{
	String chain;//
	String amount;//
	String symbol;//
	UInt32 decimals;//
	PHANTASMA_VECTOR<String> ids;//
};

struct Account
{
	String address;//
	String name;//
	PHANTASMA_VECTOR<Balance> balances;//
};

struct Chain
{
	String name;//
	String address;//
	String parentAddress;//
	UInt32 height;//
};

struct App
{
	String id;//
	String title;//
	String url;//
	String description;//
	String icon;//
};

struct Event
{
	String address;//
	String kind;//
	String data;//
};

struct Transaction
{
	String hash;//
	String chainAddress;//
	UInt32 timestamp;//
	Int32 confirmations;//
	UInt32 blockHeight;//
	String blockHash;//
	String script;//
	PHANTASMA_VECTOR<Event> events;//
	String result;//
};

struct AccountTransactions
{
	String address;//
	PHANTASMA_VECTOR<Transaction> txs;//
};

struct Paginated
{
	UInt32 page;//
	UInt32 pageSize;//
	UInt32 total;//
	UInt32 totalPages;//
	JSONValue result;//
};

struct Block
{
	String hash;//
	String previousHash;//
	UInt32 timestamp;//
	UInt32 height;//
	String chainAddress;//
	String payload;//
	PHANTASMA_VECTOR<Transaction> txs;//
	String validatorAddress;//
	String reward;//
};

struct TokenMetadata
{
	String key;//
	String value;//
};

struct Token
{
	String symbol;//
	String name;//
	Int32 decimals;//
	String currentSupply;//
	String maxSupply;//
	String ownerAddress;//
	PHANTASMA_VECTOR<TokenMetadata> metadataList;//
	String flags;//
};

struct TokenData
{
	String ID;//
	String chainAddress;//
	String ownerAddress;//
	String ram;//
	String rom;//
	bool forSale;//
};

struct SendRawTx
{
	String hash;//
	String error;//
};

struct Auction
{
	String creatorAddress;//
	UInt32 startDate;//
	UInt32 endDate;//
	String baseSymbol;//
	String quoteSymbol;//
	String tokenId;//
	String price;//
};

struct Script
{
	PHANTASMA_VECTOR<Event> events;//
	String result;//
};


class PhantasmaJsonAPI
{
public:
	static const Char* Uri() { return PHANTASMA_LITERAL("/rpc"); }

	// Returns the account name and balance of given address. 
	static void MakeGetAccountRequest(JSONBuilder&, const Char* addressText);
	static bool ParseGetAccountResponse(const JSONValue&, Account& out);
	// Returns the address that owns a given name. 
	static void MakeLookUpNameRequest(JSONBuilder&, const Char* name);
	static bool ParseLookUpNameResponse(const JSONValue&, String& out);
	// Returns the height of a chain. 
	static void MakeGetBlockHeightRequest(JSONBuilder&, const Char* chainInput);
	static bool ParseGetBlockHeightResponse(const JSONValue&, Int32& out);
	// Returns the number of transactions of given block hash or error if given hash is invalid or is not found. 
	static void MakeGetBlockTransactionCountByHashRequest(JSONBuilder&, const Char* blockHash);
	static bool ParseGetBlockTransactionCountByHashResponse(const JSONValue&, Int32& out);
	// Returns information about a block by hash. 
	static void MakeGetBlockByHashRequest(JSONBuilder&, const Char* blockHash);
	static bool ParseGetBlockByHashResponse(const JSONValue&, Block& out);
	// Returns a serialized string, containing information about a block by hash. 
	static void MakeGetRawBlockByHashRequest(JSONBuilder&, const Char* blockHash);
	static bool ParseGetRawBlockByHashResponse(const JSONValue&, String& out);
	// Returns information about a block by height and chain. 
	static void MakeGetBlockByHeightRequest(JSONBuilder&, const Char* chainInput, UInt32 height);
	static bool ParseGetBlockByHeightResponse(const JSONValue&, Block& out);
	// Returns a serialized string, in hex format, containing information about a block by height and chain. 
	static void MakeGetRawBlockByHeightRequest(JSONBuilder&, const Char* chainInput, UInt32 height);
	static bool ParseGetRawBlockByHeightResponse(const JSONValue&, String& out);
	// Returns the information about a transaction requested by a block hash and transaction index. 
	static void MakeGetTransactionByBlockHashAndIndexRequest(JSONBuilder&, const Char* blockHash, Int32 index);
	static bool ParseGetTransactionByBlockHashAndIndexResponse(const JSONValue&, Transaction& out);
	// Returns last X transactions of given address. (paginated call)
	static void MakeGetAddressTransactionsRequest(JSONBuilder&, const Char* addressText, UInt32 page, UInt32 pageSize);
	static bool ParseGetAddressTransactionsResponse(const JSONValue&, AccountTransactions& out);
	// Get number of transactions in a specific address and chain 
	static void MakeGetAddressTransactionCountRequest(JSONBuilder&, const Char* addressText, const Char* chainInput);
	static bool ParseGetAddressTransactionCountResponse(const JSONValue&, Int32& out);
	// Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually. 
	static void MakeSendRawTransactionRequest(JSONBuilder&, const Char* txData);
	static bool ParseSendRawTransactionResponse(const JSONValue&, String& out);
	// Allows to invoke script based on network state, without state changes. 
	static void MakeInvokeRawScriptRequest(JSONBuilder&, const Char* chainInput, const Char* scriptData);
	static bool ParseInvokeRawScriptResponse(const JSONValue&, Script& out);
	// Returns information about a transaction by hash. 
	static void MakeGetTransactionRequest(JSONBuilder&, const Char* hashText);
	static bool ParseGetTransactionResponse(const JSONValue&, Transaction& out);
	// Removes a pending transaction from the mempool. 
	static void MakeCancelTransactionRequest(JSONBuilder&, const Char* hashText);
	static bool ParseCancelTransactionResponse(const JSONValue&, String& out);
	// Returns an array of all chains deployed in Phantasma. 
	static void MakeGetChainsRequest(JSONBuilder&);
	static bool ParseGetChainsResponse(const JSONValue&, PHANTASMA_VECTOR<Chain>& out);
	// Returns an array of tokens deployed in Phantasma. 
	static void MakeGetTokensRequest(JSONBuilder&);
	static bool ParseGetTokensResponse(const JSONValue&, PHANTASMA_VECTOR<Token>& out);
	// Returns info about a specific token deployed in Phantasma. 
	static void MakeGetTokenRequest(JSONBuilder&, const Char* symbol);
	static bool ParseGetTokenResponse(const JSONValue&, Token& out);
	// Returns data of a non-fungible token, in hexadecimal format. 
	static void MakeGetTokenDataRequest(JSONBuilder&, const Char* symbol, const Char* IDtext);
	static bool ParseGetTokenDataResponse(const JSONValue&, TokenData& out);
	// Returns an array of apps deployed in Phantasma. 
	static void MakeGetAppsRequest(JSONBuilder&);
	static bool ParseGetAppsResponse(const JSONValue&, PHANTASMA_VECTOR<App>& out);
	// Returns last X transactions of given token. (paginated call)
	static void MakeGetTokenTransfersRequest(JSONBuilder&, const Char* tokenSymbol, UInt32 page, UInt32 pageSize);
	static bool ParseGetTokenTransfersResponse(const JSONValue&, PHANTASMA_VECTOR<Transaction>& out);
	// Returns the number of transaction of a given token. 
	static void MakeGetTokenTransferCountRequest(JSONBuilder&, const Char* tokenSymbol);
	static bool ParseGetTokenTransferCountResponse(const JSONValue&, Int32& out);
	// Returns the balance for a specific token and chain, given an address. 
	static void MakeGetTokenBalanceRequest(JSONBuilder&, const Char* addressText, const Char* tokenSymbol, const Char* chainInput);
	static bool ParseGetTokenBalanceResponse(const JSONValue&, Balance& out);
	// Returns the number of active auctions. 
	static void MakeGetAuctionsCountRequest(JSONBuilder&, const Char* symbol);
	static bool ParseGetAuctionsCountResponse(const JSONValue&, Int32& out);
	// Returns the auctions available in the market. (paginated call)
	static void MakeGetAuctionsRequest(JSONBuilder&, const Char* symbol, UInt32 page, UInt32 pageSize);
	static bool ParseGetAuctionsResponse(const JSONValue&, PHANTASMA_VECTOR<Auction>& out);
	// Returns the auction for a specific token. 
	static void MakeGetAuctionRequest(JSONBuilder&, const Char* symbol, const Char* IDtext);
	static bool ParseGetAuctionResponse(const JSONValue&, Auction& out);
	

private:
	static JSONValue CheckResponse(JSONValue response, bool& out_error);
	static Balance DeserializeBalance(const JSONValue& json, bool& out_error);
	static Account DeserializeAccount(const JSONValue& json, bool& out_error);
	static Chain DeserializeChain(const JSONValue& json, bool& out_error);
	static App DeserializeApp(const JSONValue& json, bool& out_error);
	static Event DeserializeEvent(const JSONValue& json, bool& out_error);
	static Transaction DeserializeTransaction(const JSONValue& json, bool& out_error);
	static AccountTransactions DeserializeAccountTransactions(const JSONValue& json, bool& out_error);
	static Paginated DeserializePaginated(const JSONValue& json, bool& out_error);
	static Block DeserializeBlock(const JSONValue& json, bool& out_error);
	static TokenMetadata DeserializeTokenMetadata(const JSONValue& json, bool& out_error);
	static Token DeserializeToken(const JSONValue& json, bool& out_error);
	static TokenData DeserializeTokenData(const JSONValue& json, bool& out_error);
	static SendRawTx DeserializeSendRawTx(const JSONValue& json, bool& out_error);
	static Auction DeserializeAuction(const JSONValue& json, bool& out_error);
	static Script DeserializeScript(const JSONValue& json, bool& out_error);
	
};

#if defined(PHANTASMA_HTTPCLIENT)
class PhantasmaAPI
{
public:
	PhantasmaAPI(HttpClient& client) // client must have a longer lifetime than this API object
		: m_httpClient(client)
	{}

	// Returns the account name and balance of given address. 
	Account GetAccount(const Char* addressText, bool* out_error = nullptr);
	// Returns the address that owns a given name. 
	String LookUpName(const Char* name, bool* out_error = nullptr);
	// Returns the height of a chain. 
	Int32 GetBlockHeight(const Char* chainInput, bool* out_error = nullptr);
	// Returns the number of transactions of given block hash or error if given hash is invalid or is not found. 
	Int32 GetBlockTransactionCountByHash(const Char* blockHash, bool* out_error = nullptr);
	// Returns information about a block by hash. 
	Block GetBlockByHash(const Char* blockHash, bool* out_error = nullptr);
	// Returns a serialized string, containing information about a block by hash. 
	String GetRawBlockByHash(const Char* blockHash, bool* out_error = nullptr);
	// Returns information about a block by height and chain. 
	Block GetBlockByHeight(const Char* chainInput, UInt32 height, bool* out_error = nullptr);
	// Returns a serialized string, in hex format, containing information about a block by height and chain. 
	String GetRawBlockByHeight(const Char* chainInput, UInt32 height, bool* out_error = nullptr);
	// Returns the information about a transaction requested by a block hash and transaction index. 
	Transaction GetTransactionByBlockHashAndIndex(const Char* blockHash, Int32 index, bool* out_error = nullptr);
	// Returns last X transactions of given address. (paginated call)
	AccountTransactions GetAddressTransactions(const Char* addressText, UInt32 page, UInt32 pageSize, bool* out_error = nullptr);
	// Get number of transactions in a specific address and chain 
	Int32 GetAddressTransactionCount(const Char* addressText, const Char* chainInput, bool* out_error = nullptr);
	// Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually. 
	String SendRawTransaction(const Char* txData, bool* out_error = nullptr);
	// Allows to invoke script based on network state, without state changes. 
	Script InvokeRawScript(const Char* chainInput, const Char* scriptData, bool* out_error = nullptr);
	// Returns information about a transaction by hash. 
	Transaction GetTransaction(const Char* hashText, bool* out_error = nullptr);
	// Removes a pending transaction from the mempool. 
	String CancelTransaction(const Char* hashText, bool* out_error = nullptr);
	// Returns an array of all chains deployed in Phantasma. 
	PHANTASMA_VECTOR<Chain> GetChains(bool* out_error = nullptr);
	// Returns an array of tokens deployed in Phantasma. 
	PHANTASMA_VECTOR<Token> GetTokens(bool* out_error = nullptr);
	// Returns info about a specific token deployed in Phantasma. 
	Token GetToken(const Char* symbol, bool* out_error = nullptr);
	// Returns data of a non-fungible token, in hexadecimal format. 
	TokenData GetTokenData(const Char* symbol, const Char* IDtext, bool* out_error = nullptr);
	// Returns an array of apps deployed in Phantasma. 
	PHANTASMA_VECTOR<App> GetApps(bool* out_error = nullptr);
	// Returns last X transactions of given token. (paginated call)
	PHANTASMA_VECTOR<Transaction> GetTokenTransfers(const Char* tokenSymbol, UInt32 page, UInt32 pageSize, bool* out_error = nullptr);
	// Returns the number of transaction of a given token. 
	Int32 GetTokenTransferCount(const Char* tokenSymbol, bool* out_error = nullptr);
	// Returns the balance for a specific token and chain, given an address. 
	Balance GetTokenBalance(const Char* addressText, const Char* tokenSymbol, const Char* chainInput, bool* out_error = nullptr);
	// Returns the number of active auctions. 
	Int32 GetAuctionsCount(const Char* symbol, bool* out_error = nullptr);
	// Returns the auctions available in the market. (paginated call)
	PHANTASMA_VECTOR<Auction> GetAuctions(const Char* symbol, UInt32 page, UInt32 pageSize, bool* out_error = nullptr);
	// Returns the auction for a specific token. 
	Auction GetAuction(const Char* symbol, const Char* IDtext, bool* out_error = nullptr);
	
private:
	HttpClient& m_httpClient;
};
#endif
	
#if defined(PHANTASMA_IMPLEMENTATION)

PHANTASMA_FUNCTION Balance PhantasmaJsonAPI::DeserializeBalance(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<String> idsVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("ids"), err))
	{
		const JSONArray& idsJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("ids"), err);
		int size = json::ArraySize(idsJsonArray, err);
		idsVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			idsVector.push_back(json::AsString(json::IndexArray(idsJsonArray, i, err), err));
		}
	}	
	return Balance { 
		json::LookupString(value, PHANTASMA_LITERAL("chain"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("amount"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("symbol"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("decimals"), err), 
		idsVector
	};
}

PHANTASMA_FUNCTION Account PhantasmaJsonAPI::DeserializeAccount(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<Balance> balancesVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("balances"), err))
	{
		const JSONArray& balancesJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("balances"), err);
		int size = json::ArraySize(balancesJsonArray, err);
		balancesVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			balancesVector.push_back(DeserializeBalance(json::IndexArray(balancesJsonArray, i, err), err));
		}
	}	
	return Account { 
		json::LookupString(value, PHANTASMA_LITERAL("address"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("name"), err), 
		balancesVector
	};
}

PHANTASMA_FUNCTION Chain PhantasmaJsonAPI::DeserializeChain(const JSONValue& value, bool& err)
{ 	
	return Chain { 
		json::LookupString(value, PHANTASMA_LITERAL("name"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("address"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("parentAddress"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("height"), err)
	};
}

PHANTASMA_FUNCTION App PhantasmaJsonAPI::DeserializeApp(const JSONValue& value, bool& err)
{ 	
	return App { 
		json::LookupString(value, PHANTASMA_LITERAL("id"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("title"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("url"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("description"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("icon"), err)
	};
}

PHANTASMA_FUNCTION Event PhantasmaJsonAPI::DeserializeEvent(const JSONValue& value, bool& err)
{ 	
	return Event { 
		json::LookupString(value, PHANTASMA_LITERAL("address"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("kind"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("data"), err)
	};
}

PHANTASMA_FUNCTION Transaction PhantasmaJsonAPI::DeserializeTransaction(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<Event> eventsVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("events"), err))
	{
		const JSONArray& eventsJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("events"), err);
		int size = json::ArraySize(eventsJsonArray, err);
		eventsVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			eventsVector.push_back(DeserializeEvent(json::IndexArray(eventsJsonArray, i, err), err));
		}
	}	
	return Transaction { 
		json::LookupString(value, PHANTASMA_LITERAL("hash"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("chainAddress"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("timestamp"), err), 
		json::LookupInt32(value, PHANTASMA_LITERAL("confirmations"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("blockHeight"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("blockHash"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("script"), err), 
		eventsVector, 
		json::LookupString(value, PHANTASMA_LITERAL("result"), err)
	};
}

PHANTASMA_FUNCTION AccountTransactions PhantasmaJsonAPI::DeserializeAccountTransactions(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<Transaction> txsVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("txs"), err))
	{
		const JSONArray& txsJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("txs"), err);
		int size = json::ArraySize(txsJsonArray, err);
		txsVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			txsVector.push_back(DeserializeTransaction(json::IndexArray(txsJsonArray, i, err), err));
		}
	}	
	return AccountTransactions { 
		json::LookupString(value, PHANTASMA_LITERAL("address"), err), 
		txsVector
	};
}

PHANTASMA_FUNCTION Paginated PhantasmaJsonAPI::DeserializePaginated(const JSONValue& value, bool& err)
{ 	
	return Paginated { 
		json::LookupUInt32(value, PHANTASMA_LITERAL("page"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("pageSize"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("total"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("totalPages"), err), 
		json::LookupValue(value, PHANTASMA_LITERAL("result"), err)
	};
}

PHANTASMA_FUNCTION Block PhantasmaJsonAPI::DeserializeBlock(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<Transaction> txsVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("txs"), err))
	{
		const JSONArray& txsJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("txs"), err);
		int size = json::ArraySize(txsJsonArray, err);
		txsVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			txsVector.push_back(DeserializeTransaction(json::IndexArray(txsJsonArray, i, err), err));
		}
	}	
	return Block { 
		json::LookupString(value, PHANTASMA_LITERAL("hash"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("previousHash"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("timestamp"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("height"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("chainAddress"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("payload"), err), 
		txsVector, 
		json::LookupString(value, PHANTASMA_LITERAL("validatorAddress"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("reward"), err)
	};
}

PHANTASMA_FUNCTION TokenMetadata PhantasmaJsonAPI::DeserializeTokenMetadata(const JSONValue& value, bool& err)
{ 	
	return TokenMetadata { 
		json::LookupString(value, PHANTASMA_LITERAL("key"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("value"), err)
	};
}

PHANTASMA_FUNCTION Token PhantasmaJsonAPI::DeserializeToken(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<TokenMetadata> metadataListVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("metadataList"), err))
	{
		const JSONArray& metadataListJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("metadataList"), err);
		int size = json::ArraySize(metadataListJsonArray, err);
		metadataListVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			metadataListVector.push_back(DeserializeTokenMetadata(json::IndexArray(metadataListJsonArray, i, err), err));
		}
	}	
	return Token { 
		json::LookupString(value, PHANTASMA_LITERAL("symbol"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("name"), err), 
		json::LookupInt32(value, PHANTASMA_LITERAL("decimals"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("currentSupply"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("maxSupply"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("ownerAddress"), err), 
		metadataListVector, 
		json::LookupString(value, PHANTASMA_LITERAL("flags"), err)
	};
}

PHANTASMA_FUNCTION TokenData PhantasmaJsonAPI::DeserializeTokenData(const JSONValue& value, bool& err)
{ 	
	return TokenData { 
		json::LookupString(value, PHANTASMA_LITERAL("ID"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("chainAddress"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("ownerAddress"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("ram"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("rom"), err), 
		(err=true, "Variable type Boolean isnt currently handled by the template system")
	};
}

PHANTASMA_FUNCTION SendRawTx PhantasmaJsonAPI::DeserializeSendRawTx(const JSONValue& value, bool& err)
{ 	
	return SendRawTx { 
		json::LookupString(value, PHANTASMA_LITERAL("hash"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("error"), err)
	};
}

PHANTASMA_FUNCTION Auction PhantasmaJsonAPI::DeserializeAuction(const JSONValue& value, bool& err)
{ 	
	return Auction { 
		json::LookupString(value, PHANTASMA_LITERAL("creatorAddress"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("startDate"), err), 
		json::LookupUInt32(value, PHANTASMA_LITERAL("endDate"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("baseSymbol"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("quoteSymbol"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("tokenId"), err), 
		json::LookupString(value, PHANTASMA_LITERAL("price"), err)
	};
}

PHANTASMA_FUNCTION Script PhantasmaJsonAPI::DeserializeScript(const JSONValue& value, bool& err)
{ 
	PHANTASMA_VECTOR<Event> eventsVector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("events"), err))
	{
		const JSONArray& eventsJsonArray = json::LookupArray(value, PHANTASMA_LITERAL("events"), err);
		int size = json::ArraySize(eventsJsonArray, err);
		eventsVector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			eventsVector.push_back(DeserializeEvent(json::IndexArray(eventsJsonArray, i, err), err));
		}
	}	
	return Script { 
		eventsVector, 
		json::LookupString(value, PHANTASMA_LITERAL("result"), err)
	};
}



PHANTASMA_FUNCTION JSONValue PhantasmaJsonAPI::CheckResponse(JSONValue response, bool& out_error)
{
	if( !json::IsObject(response, out_error) )
	{
		PHANTASMA_EXCEPTION("Failed to parse JSON");
		out_error = true;
		return response;
	}
	if( json::HasField(response, PHANTASMA_LITERAL("error"), out_error) )
	{
		String msg = json::LookupString(response, PHANTASMA_LITERAL("error"), out_error);
		PHANTASMA_EXCEPTION_MESSAGE("Server returned error: %s", msg);
		out_error = true;
		return response;
	}
	if( !json::HasField(response, PHANTASMA_LITERAL("result"), out_error) )
	{
		PHANTASMA_EXCEPTION("Malformed response: No \"result\" node on the JSON body");
		out_error = true;
		return response;
	}
	return json::LookupValue(response, PHANTASMA_LITERAL("result"), out_error);
}


// Returns the account name and balance of given address. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAccountRequest(JSONBuilder& request, const Char* addressText)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getAccount"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), addressText);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAccountResponse(const JSONValue& _jsonResponse, Account& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeAccount(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns the address that owns a given name. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeLookUpNameRequest(JSONBuilder& request, const Char* name)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("lookUpName"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), name);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseLookUpNameResponse(const JSONValue& _jsonResponse, String& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsString(jsonResponse, err);
	return !err;
}


// Returns the height of a chain. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetBlockHeightRequest(JSONBuilder& request, const Char* chainInput)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getBlockHeight"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), chainInput);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetBlockHeightResponse(const JSONValue& _jsonResponse, Int32& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsInt32(jsonResponse, err);
	return !err;
}


// Returns the number of transactions of given block hash or error if given hash is invalid or is not found. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetBlockTransactionCountByHashRequest(JSONBuilder& request, const Char* blockHash)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getBlockTransactionCountByHash"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), blockHash);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetBlockTransactionCountByHashResponse(const JSONValue& _jsonResponse, Int32& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsInt32(jsonResponse, err);
	return !err;
}


// Returns information about a block by hash. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetBlockByHashRequest(JSONBuilder& request, const Char* blockHash)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getBlockByHash"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), blockHash);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetBlockByHashResponse(const JSONValue& _jsonResponse, Block& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeBlock(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns a serialized string, containing information about a block by hash. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetRawBlockByHashRequest(JSONBuilder& request, const Char* blockHash)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getRawBlockByHash"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), blockHash);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetRawBlockByHashResponse(const JSONValue& _jsonResponse, String& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsString(jsonResponse, err);
	return !err;
}


// Returns information about a block by height and chain. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetBlockByHeightRequest(JSONBuilder& request, const Char* chainInput, UInt32 height)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getBlockByHeight"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), chainInput, height);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetBlockByHeightResponse(const JSONValue& _jsonResponse, Block& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeBlock(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns a serialized string, in hex format, containing information about a block by height and chain. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetRawBlockByHeightRequest(JSONBuilder& request, const Char* chainInput, UInt32 height)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getRawBlockByHeight"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), chainInput, height);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetRawBlockByHeightResponse(const JSONValue& _jsonResponse, String& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsString(jsonResponse, err);
	return !err;
}


// Returns the information about a transaction requested by a block hash and transaction index. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTransactionByBlockHashAndIndexRequest(JSONBuilder& request, const Char* blockHash, Int32 index)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTransactionByBlockHashAndIndex"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), blockHash, index);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTransactionByBlockHashAndIndexResponse(const JSONValue& _jsonResponse, Transaction& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeTransaction(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns last X transactions of given address. (Paginated)
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAddressTransactionsRequest(JSONBuilder& request, const Char* addressText, UInt32 page, UInt32 pageSize)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getAddressTransactions"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), addressText, page, pageSize);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAddressTransactionsResponse(const JSONValue& _jsonResponse, AccountTransactions& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	Paginated pageStruct = DeserializePaginated(jsonResponse, err);
	output = DeserializeAccountTransactions(pageStruct.result, err);
	return !err;
}

// Get number of transactions in a specific address and chain 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAddressTransactionCountRequest(JSONBuilder& request, const Char* addressText, const Char* chainInput)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getAddressTransactionCount"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), addressText, chainInput);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAddressTransactionCountResponse(const JSONValue& _jsonResponse, Int32& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsInt32(jsonResponse, err);
	return !err;
}


// Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeSendRawTransactionRequest(JSONBuilder& request, const Char* txData)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("sendRawTransaction"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), txData);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseSendRawTransactionResponse(const JSONValue& _jsonResponse, String& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsString(jsonResponse, err);
	return !err;
}


// Allows to invoke script based on network state, without state changes. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeInvokeRawScriptRequest(JSONBuilder& request, const Char* chainInput, const Char* scriptData)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("invokeRawScript"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), chainInput, scriptData);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseInvokeRawScriptResponse(const JSONValue& _jsonResponse, Script& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeScript(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns information about a transaction by hash. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTransactionRequest(JSONBuilder& request, const Char* hashText)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTransaction"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), hashText);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTransactionResponse(const JSONValue& _jsonResponse, Transaction& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeTransaction(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Removes a pending transaction from the mempool. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeCancelTransactionRequest(JSONBuilder& request, const Char* hashText)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("cancelTransaction"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), hashText);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseCancelTransactionResponse(const JSONValue& _jsonResponse, String& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsString(jsonResponse, err);
	return !err;
}


// Returns an array of all chains deployed in Phantasma. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetChainsRequest(JSONBuilder& request)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getChains"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"));
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetChainsResponse(const JSONValue& _jsonResponse, PHANTASMA_VECTOR<Chain>& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	if (!json::IsArray(jsonResponse, err))
	{ 
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");
		return false;
	} 

	const JSONArray& resultArray = json::AsArray(jsonResponse, err);
	int resultArraySize = json::ArraySize(resultArray, err);
	output.reserve(resultArraySize);
	for(int i = 0; i < resultArraySize; ++i)
	{
		output.push_back(DeserializeChain(json::IndexArray(resultArray, i, err), err));
		if( err ) return false;
	}
	return !err;
}


// Returns an array of tokens deployed in Phantasma. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTokensRequest(JSONBuilder& request)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTokens"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"));
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTokensResponse(const JSONValue& _jsonResponse, PHANTASMA_VECTOR<Token>& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	if (!json::IsArray(jsonResponse, err))
	{ 
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");
		return false;
	} 

	const JSONArray& resultArray = json::AsArray(jsonResponse, err);
	int resultArraySize = json::ArraySize(resultArray, err);
	output.reserve(resultArraySize);
	for(int i = 0; i < resultArraySize; ++i)
	{
		output.push_back(DeserializeToken(json::IndexArray(resultArray, i, err), err));
		if( err ) return false;
	}
	return !err;
}


// Returns info about a specific token deployed in Phantasma. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTokenRequest(JSONBuilder& request, const Char* symbol)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getToken"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), symbol);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTokenResponse(const JSONValue& _jsonResponse, Token& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeToken(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns data of a non-fungible token, in hexadecimal format. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTokenDataRequest(JSONBuilder& request, const Char* symbol, const Char* IDtext)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTokenData"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), symbol, IDtext);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTokenDataResponse(const JSONValue& _jsonResponse, TokenData& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeTokenData(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns an array of apps deployed in Phantasma. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAppsRequest(JSONBuilder& request)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getApps"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"));
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAppsResponse(const JSONValue& _jsonResponse, PHANTASMA_VECTOR<App>& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	if (!json::IsArray(jsonResponse, err))
	{ 
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");
		return false;
	} 

	const JSONArray& resultArray = json::AsArray(jsonResponse, err);
	int resultArraySize = json::ArraySize(resultArray, err);
	output.reserve(resultArraySize);
	for(int i = 0; i < resultArraySize; ++i)
	{
		output.push_back(DeserializeApp(json::IndexArray(resultArray, i, err), err));
		if( err ) return false;
	}
	return !err;
}


// Returns last X transactions of given token. (Paginated)
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTokenTransfersRequest(JSONBuilder& request, const Char* tokenSymbol, UInt32 page, UInt32 pageSize)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTokenTransfers"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), tokenSymbol, page, pageSize);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTokenTransfersResponse(const JSONValue& _jsonResponse, PHANTASMA_VECTOR<Transaction>& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	Paginated pageStruct = DeserializePaginated(jsonResponse, err);
	if(!json::IsArray(pageStruct.result, err))
	{ 
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");
		return false;
	} 
	const JSONArray& pages = json::AsArray(pageStruct.result, err);
	int size = json::ArraySize(pages, err);
	output.reserve(size);
	for(int i = 0; i < size; ++i)
	{
		output.push_back(DeserializeTransaction(json::IndexArray(pages, i, err), err));
	}
	return !err;
}

// Returns the number of transaction of a given token. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTokenTransferCountRequest(JSONBuilder& request, const Char* tokenSymbol)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTokenTransferCount"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), tokenSymbol);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTokenTransferCountResponse(const JSONValue& _jsonResponse, Int32& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsInt32(jsonResponse, err);
	return !err;
}


// Returns the balance for a specific token and chain, given an address. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetTokenBalanceRequest(JSONBuilder& request, const Char* addressText, const Char* tokenSymbol, const Char* chainInput)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getTokenBalance"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), addressText, tokenSymbol, chainInput);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetTokenBalanceResponse(const JSONValue& _jsonResponse, Balance& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeBalance(jsonResponse, err);
		if( err ) return false;
	return !err;
}


// Returns the number of active auctions. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAuctionsCountRequest(JSONBuilder& request, const Char* symbol)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getAuctionsCount"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), symbol);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAuctionsCountResponse(const JSONValue& _jsonResponse, Int32& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = json::AsInt32(jsonResponse, err);
	return !err;
}


// Returns the auctions available in the market. (Paginated)
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAuctionsRequest(JSONBuilder& request, const Char* symbol, UInt32 page, UInt32 pageSize)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getAuctions"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), symbol, page, pageSize);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAuctionsResponse(const JSONValue& _jsonResponse, PHANTASMA_VECTOR<Auction>& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	Paginated pageStruct = DeserializePaginated(jsonResponse, err);
	if(!json::IsArray(pageStruct.result, err))
	{ 
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");
		return false;
	} 
	const JSONArray& pages = json::AsArray(pageStruct.result, err);
	int size = json::ArraySize(pages, err);
	output.reserve(size);
	for(int i = 0; i < size; ++i)
	{
		output.push_back(DeserializeAuction(json::IndexArray(pages, i, err), err));
	}
	return !err;
}

// Returns the auction for a specific token. 
PHANTASMA_FUNCTION void PhantasmaJsonAPI::MakeGetAuctionRequest(JSONBuilder& request, const Char* symbol, const Char* IDtext)
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("getAuction"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"), symbol, IDtext);
	json::EndObject(request);
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::ParseGetAuctionResponse(const JSONValue& _jsonResponse, Auction& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	output = DeserializeAuction(jsonResponse, err);
		if( err ) return false;
	return !err;
}


	
#if defined(PHANTASMA_HTTPCLIENT)

PHANTASMA_FUNCTION Account PhantasmaAPI::GetAccount(const Char* addressText, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAccountRequest(request, addressText);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Account output;
	bool success = PhantasmaJsonAPI::ParseGetAccountResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION String PhantasmaAPI::LookUpName(const Char* name, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeLookUpNameRequest(request, name);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	String output;
	bool success = PhantasmaJsonAPI::ParseLookUpNameResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Int32 PhantasmaAPI::GetBlockHeight(const Char* chainInput, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetBlockHeightRequest(request, chainInput);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Int32 output;
	bool success = PhantasmaJsonAPI::ParseGetBlockHeightResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Int32 PhantasmaAPI::GetBlockTransactionCountByHash(const Char* blockHash, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetBlockTransactionCountByHashRequest(request, blockHash);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Int32 output;
	bool success = PhantasmaJsonAPI::ParseGetBlockTransactionCountByHashResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Block PhantasmaAPI::GetBlockByHash(const Char* blockHash, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetBlockByHashRequest(request, blockHash);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Block output;
	bool success = PhantasmaJsonAPI::ParseGetBlockByHashResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION String PhantasmaAPI::GetRawBlockByHash(const Char* blockHash, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetRawBlockByHashRequest(request, blockHash);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	String output;
	bool success = PhantasmaJsonAPI::ParseGetRawBlockByHashResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Block PhantasmaAPI::GetBlockByHeight(const Char* chainInput, UInt32 height, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetBlockByHeightRequest(request, chainInput, height);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Block output;
	bool success = PhantasmaJsonAPI::ParseGetBlockByHeightResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION String PhantasmaAPI::GetRawBlockByHeight(const Char* chainInput, UInt32 height, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetRawBlockByHeightRequest(request, chainInput, height);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	String output;
	bool success = PhantasmaJsonAPI::ParseGetRawBlockByHeightResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Transaction PhantasmaAPI::GetTransactionByBlockHashAndIndex(const Char* blockHash, Int32 index, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTransactionByBlockHashAndIndexRequest(request, blockHash, index);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Transaction output;
	bool success = PhantasmaJsonAPI::ParseGetTransactionByBlockHashAndIndexResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION AccountTransactions PhantasmaAPI::GetAddressTransactions(const Char* addressText, UInt32 page, UInt32 pageSize, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAddressTransactionsRequest(request, addressText, page, pageSize);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	AccountTransactions output;
	bool success = PhantasmaJsonAPI::ParseGetAddressTransactionsResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Int32 PhantasmaAPI::GetAddressTransactionCount(const Char* addressText, const Char* chainInput, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAddressTransactionCountRequest(request, addressText, chainInput);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Int32 output;
	bool success = PhantasmaJsonAPI::ParseGetAddressTransactionCountResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION String PhantasmaAPI::SendRawTransaction(const Char* txData, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeSendRawTransactionRequest(request, txData);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	String output;
	bool success = PhantasmaJsonAPI::ParseSendRawTransactionResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Script PhantasmaAPI::InvokeRawScript(const Char* chainInput, const Char* scriptData, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeInvokeRawScriptRequest(request, chainInput, scriptData);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Script output;
	bool success = PhantasmaJsonAPI::ParseInvokeRawScriptResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Transaction PhantasmaAPI::GetTransaction(const Char* hashText, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTransactionRequest(request, hashText);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Transaction output;
	bool success = PhantasmaJsonAPI::ParseGetTransactionResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION String PhantasmaAPI::CancelTransaction(const Char* hashText, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeCancelTransactionRequest(request, hashText);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	String output;
	bool success = PhantasmaJsonAPI::ParseCancelTransactionResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION PHANTASMA_VECTOR<Chain> PhantasmaAPI::GetChains(bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetChainsRequest(request);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	PHANTASMA_VECTOR<Chain> output;
	bool success = PhantasmaJsonAPI::ParseGetChainsResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION PHANTASMA_VECTOR<Token> PhantasmaAPI::GetTokens(bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTokensRequest(request);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	PHANTASMA_VECTOR<Token> output;
	bool success = PhantasmaJsonAPI::ParseGetTokensResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Token PhantasmaAPI::GetToken(const Char* symbol, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTokenRequest(request, symbol);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Token output;
	bool success = PhantasmaJsonAPI::ParseGetTokenResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION TokenData PhantasmaAPI::GetTokenData(const Char* symbol, const Char* IDtext, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTokenDataRequest(request, symbol, IDtext);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	TokenData output;
	bool success = PhantasmaJsonAPI::ParseGetTokenDataResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION PHANTASMA_VECTOR<App> PhantasmaAPI::GetApps(bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAppsRequest(request);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	PHANTASMA_VECTOR<App> output;
	bool success = PhantasmaJsonAPI::ParseGetAppsResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION PHANTASMA_VECTOR<Transaction> PhantasmaAPI::GetTokenTransfers(const Char* tokenSymbol, UInt32 page, UInt32 pageSize, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTokenTransfersRequest(request, tokenSymbol, page, pageSize);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	PHANTASMA_VECTOR<Transaction> output;
	bool success = PhantasmaJsonAPI::ParseGetTokenTransfersResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Int32 PhantasmaAPI::GetTokenTransferCount(const Char* tokenSymbol, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTokenTransferCountRequest(request, tokenSymbol);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Int32 output;
	bool success = PhantasmaJsonAPI::ParseGetTokenTransferCountResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Balance PhantasmaAPI::GetTokenBalance(const Char* addressText, const Char* tokenSymbol, const Char* chainInput, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetTokenBalanceRequest(request, addressText, tokenSymbol, chainInput);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Balance output;
	bool success = PhantasmaJsonAPI::ParseGetTokenBalanceResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Int32 PhantasmaAPI::GetAuctionsCount(const Char* symbol, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAuctionsCountRequest(request, symbol);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Int32 output;
	bool success = PhantasmaJsonAPI::ParseGetAuctionsCountResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION PHANTASMA_VECTOR<Auction> PhantasmaAPI::GetAuctions(const Char* symbol, UInt32 page, UInt32 pageSize, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAuctionsRequest(request, symbol, page, pageSize);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	PHANTASMA_VECTOR<Auction> output;
	bool success = PhantasmaJsonAPI::ParseGetAuctionsResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

PHANTASMA_FUNCTION Auction PhantasmaAPI::GetAuction(const Char* symbol, const Char* IDtext, bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::MakeGetAuctionRequest(request, symbol, IDtext);
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	Auction output;
	bool success = PhantasmaJsonAPI::ParseGetAuctionResponse(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}

#endif

namespace json
{
#ifndef PHANTASMA_JSONVALUE
    JSONValue Parse(const JSONDocument& doc) { return doc; }

	inline size_t SkipNumber(const JSONValue& v, size_t i, bool& out_error)
	{
		size_t j = v.find_first_not_of("+-0123456789.eE", i);
		if( i==j ) { PHANTASMA_EXCEPTION("Invalid Number"); out_error = true; return i+1; }
		return j;
	}
	inline size_t SkipString(const JSONValue& v, size_t i, bool& out_error)
	{
		if( v[i] != '"' ) { PHANTASMA_EXCEPTION("Invalid String"); out_error = true; return i+1; }
		for(++i; i<v.size();)
		{
			i =  v.find_first_of("\"\\", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated String"); out_error = true; return i; }
			if( v[i] == '"' ) { break; }
			if( i+1 < v.size() )
			{
				switch(v[i+1])
				{
				case '\\': case 'b': case 'f': case 'n': case 'r': case 't': case '"': i = i+2; continue;
				case 'u': i = i+6; continue;
				}
			}
			PHANTASMA_EXCEPTION("Invalid Escape sequence"); out_error = true; ++i;
		}
		return i == JSONValue::npos ? i : i+1;
	}
	inline size_t SkipBoolNull(const JSONValue& v, size_t i, bool& out_error)
	{
		switch(v[i])
		{
		case 'f': if(v.compare(i, 5, "false")) { break; } return i+5;
		case 't': if(v.compare(i, 4, "true"))  { break; } return i+4;
		case 'n': if(v.compare(i, 4, "null"))  { break; } return i+4;
		}
		PHANTASMA_EXCEPTION("Invalid Value"); out_error = true; return i+1;
	}
	inline size_t SkipObject(const JSONValue& v, size_t i, bool& out_error);
	inline size_t SkipArray(const JSONValue& v, size_t i, bool& out_error)
	{
		if( v[i] != '[' ) { PHANTASMA_EXCEPTION("Invalid Array"); out_error = true; return i+1; }
		++i;
		for(; i<v.size();)
		{
			i = v.find_first_not_of(", \t\r\n\f\b", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated array"); out_error = true; return i; }
			switch( v[i] )
			{
			case ']': return i+1;
			case '{': i = SkipObject(v, i, out_error); break;
			case '[': i = SkipArray(v, i, out_error); break;
			case '"': i = SkipString(v, i, out_error); break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, i, out_error); break;
			}
		}
		PHANTASMA_EXCEPTION("Unterminated array");
		out_error = true;
		return i == JSONValue::npos ? i : i+1;
	}
	inline size_t SkipObject(const JSONValue& v, size_t i, bool& out_error)
	{
		if( v[i] != '{' ) { PHANTASMA_EXCEPTION("Invalid object"); out_error = true; return i+1; }
		for(; i<v.size();)
		{
			size_t keyBegin = v.find_first_of("\"}", i);
			if( keyBegin == JSONValue::npos || v[keyBegin] == '}' ) { break; }//no more keys
			size_t keyEnd = v.find_first_of('"', keyBegin+1);
			if( keyEnd == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated string"); out_error = true; break; }
			size_t valueBegin = v.find_first_not_of(" \t\r\n\f\b:", keyEnd+1);
			if( valueBegin == JSONValue::npos ) { PHANTASMA_EXCEPTION("No value following object key"); out_error = true; break; }
			char value0 = v[valueBegin];
			switch(value0)
			{
			case '{': i = SkipObject(v, valueBegin, out_error); break;
			case '[': i = SkipArray(v, valueBegin, out_error); break;
			case '"': i = SkipString(v, valueBegin, out_error); break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, valueBegin, out_error); break;
			}
		}
		return i == JSONValue::npos ? i : i+1;
	}

	PHANTASMA_FUNCTION Int32 LookupInt32(const JSONValue& v, const Char* field, bool& out_error)     { return AsInt32( LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION UInt32 LookupUInt32(const JSONValue& v, const Char* field, bool& out_error)   { return AsUInt32(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION String LookupString(const JSONValue& v, const Char* field, bool& out_error)   { return AsString(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION JSONArray LookupArray(const JSONValue& v, const Char* field, bool& out_error) { return AsArray(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION bool HasField(const JSONValue& v, const Char* field, bool& out_error)         { return "" != LookupValue(v, field, out_error); }
	PHANTASMA_FUNCTION bool HasArrayField(const JSONValue& v, const Char* field, bool& out_error)    { return IsArray(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION JSONValue LookupValue(const JSONValue& v, const Char* field, bool& out_error)
	{
		if( v.length() < 1 || v[0] != '{' ) { out_error = true; return ""; }
		size_t fieldLen = strlen(field);
		for(size_t i=0; i<v.size();)
		{
			size_t keyBegin = v.find_first_of("\"}", i);
			if( keyBegin == JSONValue::npos || v[keyBegin] == '}' ) { break; }//no more keys
			++keyBegin;
			size_t keyEnd = v.find_first_of('"', keyBegin);
			if( keyEnd == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated string"); out_error = true; break; }
			size_t valueBegin = v.find_first_not_of(" \t\r\n\f\b:", keyEnd+1);
			if( valueBegin == JSONValue::npos ) { PHANTASMA_EXCEPTION("No value following object key"); out_error = true; break; }
			size_t keyLen = keyEnd-keyBegin;
			bool correctKey = fieldLen == keyLen && 0==v.compare(keyBegin, keyLen, field);
			if( correctKey )
				return v.substr(valueBegin);//should really be using string views if this was a serious json parser...
			char value0 = v[valueBegin];
			switch(value0)
			{
			case '{': i = SkipObject(v, valueBegin, out_error); break;
			case '[': i = SkipArray( v, valueBegin, out_error); break;
			case '"': i = SkipString(v, valueBegin, out_error); break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, valueBegin, out_error); break;
			default:  i = SkipNumber(v, valueBegin, out_error); break;
			}
		}
		return "";
	}
	PHANTASMA_FUNCTION Int32  AsInt32(const JSONValue& v, bool& out_error)
	{
		const char* numeric = "-0123456789";
		size_t begin = v.find_first_of(numeric, 0);
		if( begin != 0 ) { PHANTASMA_EXCEPTION("Invalid number"); out_error = true; return 0; }
		size_t pos = v.find_first_not_of(numeric, 0);
		if( pos == 0 ) { PHANTASMA_EXCEPTION("Invalid number"); out_error = true; return 0; }
		JSONValue n = pos == JSONValue::npos ? v : v.substr(0, pos);
		return (Int32)std::strtol(n.data(), 0, 10);
	}
	PHANTASMA_FUNCTION UInt32 AsUInt32(const JSONValue& v, bool& out_error) { return (UInt32)AsInt32(v, out_error); }
	PHANTASMA_FUNCTION String AsString(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 || v[0] != '"' ) { PHANTASMA_EXCEPTION("Casting non-string value to string"); out_error = true; return String(""); }
		size_t pos = v.find('"', 1);
		if( pos == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated string"); out_error = true; return String(""); }
		return String(v.substr(1, pos-1));
	}
	PHANTASMA_FUNCTION JSONArray AsArray(const JSONValue& v, bool& out_error) { if(!IsArray(v, out_error)) { PHANTASMA_EXCEPTION("Casting non-array value to array"); out_error = true; } return v; }
	PHANTASMA_FUNCTION bool IsArray(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 ) { out_error = true; return false; }
		return v[0] == '[';
	}
	PHANTASMA_FUNCTION bool IsObject(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 ) { out_error = true; return false; }
		return v[0] == '{';
	}

	PHANTASMA_FUNCTION int ArraySize(const JSONArray& v, bool& out_error)
	{
		if( v[0] != '[' ) { PHANTASMA_EXCEPTION("Invalid Array"); out_error = true; return 0; }
		int count = 0;
		for(size_t i=1; i<v.size();)
		{
			i = v.find_first_not_of(", \t\r\n\f\b", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated array"); out_error = true; return count; }
			switch( v[i] )
			{
			case ']': return count;
			case '{': i = SkipObject(v, i, out_error); count++; break;
			case '[': i = SkipArray(v, i, out_error);  count++; break;
			case '"': i = SkipString(v, i, out_error); count++; break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, i, out_error); count++; break;
			}
		}
		return count;
	}
	PHANTASMA_FUNCTION JSONValue IndexArray(const JSONArray& v, int index, bool& out_error)
	{
		if( v[0] != '[' ) { PHANTASMA_EXCEPTION("Invalid Array"); out_error = true; return ""; }
		int count = 0;
		for(size_t i=1; i<v.size();)
		{
			i = v.find_first_not_of(", \t\r\n\f\b", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated array"); out_error = true; return ""; }
			if( count == index )
				return v.substr(i);
			switch( v[i] )
			{
			case ']': goto err;
			case '{': i = SkipObject(v, i, out_error); count++; break;
			case '[': i = SkipArray(v, i, out_error);  count++; break;
			case '"': i = SkipString(v, i, out_error); count++; break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, i, out_error); count++; break;
			}
		}
	err:
		PHANTASMA_EXCEPTION("Array index out of bounds"); 
		out_error = true;
		return "";
	}
#endif
#ifndef PHANTASMA_JSONBUILDER
	PHANTASMA_FUNCTION void BeginObject(JSONBuilder& b)                                   { b.BeginObject(); }
	PHANTASMA_FUNCTION void AddString(JSONBuilder& b, const Char* key, const Char* value) { b.AddString(key, value); }
	template<class... Args>
	void AddArray(JSONBuilder& b, const Char* key, Args... args)                          { b.AddArray(key, args...); }
	PHANTASMA_FUNCTION void EndObject(JSONBuilder& b)                                     { b.EndObject(); }
#endif
}
#endif
}
