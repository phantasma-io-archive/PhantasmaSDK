------------------------------------------------------------------------------
 Low-level API
------------------------------------------------------------------------------
  The PhantasmaJsonAPI namespace can construct JSON requests and parse JSON responses,
   but you are responsible for sending/receiving these messages via HTTP on your own.
   You can call `PhantasmaJsonAPI::Uri()` to determine where to send them.

     void PhantasmaJsonAPI::MakeGetAccountRequest(JSONBuilder, addressText);
     bool PhantasmaJsonAPI::ParseGetAccountResponse(JSONValue, Account);
     void PhantasmaJsonAPI::MakeLookUpNameRequest(JSONBuilder, name);
     bool PhantasmaJsonAPI::ParseLookUpNameResponse(JSONValue, String);
     void PhantasmaJsonAPI::MakeGetBlockHeightRequest(JSONBuilder, chainInput);
     bool PhantasmaJsonAPI::ParseGetBlockHeightResponse(JSONValue, Int32);
     void PhantasmaJsonAPI::MakeGetBlockTransactionCountByHashRequest(JSONBuilder, blockHash);
     bool PhantasmaJsonAPI::ParseGetBlockTransactionCountByHashResponse(JSONValue, Int32);
     void PhantasmaJsonAPI::MakeGetBlockByHashRequest(JSONBuilder, blockHash);
     bool PhantasmaJsonAPI::ParseGetBlockByHashResponse(JSONValue, Block);
     void PhantasmaJsonAPI::MakeGetRawBlockByHashRequest(JSONBuilder, blockHash);
     bool PhantasmaJsonAPI::ParseGetRawBlockByHashResponse(JSONValue, String);
     void PhantasmaJsonAPI::MakeGetBlockByHeightRequest(JSONBuilder, chainInput, height);
     bool PhantasmaJsonAPI::ParseGetBlockByHeightResponse(JSONValue, Block);
     void PhantasmaJsonAPI::MakeGetRawBlockByHeightRequest(JSONBuilder, chainInput, height);
     bool PhantasmaJsonAPI::ParseGetRawBlockByHeightResponse(JSONValue, String);
     void PhantasmaJsonAPI::MakeGetTransactionByBlockHashAndIndexRequest(JSONBuilder, blockHash, index);
     bool PhantasmaJsonAPI::ParseGetTransactionByBlockHashAndIndexResponse(JSONValue, Transaction);
     void PhantasmaJsonAPI::MakeGetAddressTransactionsRequest(JSONBuilder, addressText, page, pageSize);
     bool PhantasmaJsonAPI::ParseGetAddressTransactionsResponse(JSONValue, AccountTransactions);
     void PhantasmaJsonAPI::MakeGetAddressTransactionCountRequest(JSONBuilder, addressText, chainInput);
     bool PhantasmaJsonAPI::ParseGetAddressTransactionCountResponse(JSONValue, Int32);
     void PhantasmaJsonAPI::MakeSendRawTransactionRequest(JSONBuilder, txData);
     bool PhantasmaJsonAPI::ParseSendRawTransactionResponse(JSONValue, String);
     void PhantasmaJsonAPI::MakeInvokeRawScriptRequest(JSONBuilder, chainInput, scriptData);
     bool PhantasmaJsonAPI::ParseInvokeRawScriptResponse(JSONValue, Script);
     void PhantasmaJsonAPI::MakeGetTransactionRequest(JSONBuilder, hashText);
     bool PhantasmaJsonAPI::ParseGetTransactionResponse(JSONValue, Transaction);
     void PhantasmaJsonAPI::MakeCancelTransactionRequest(JSONBuilder, hashText);
     bool PhantasmaJsonAPI::ParseCancelTransactionResponse(JSONValue, String);
     void PhantasmaJsonAPI::MakeGetChainsRequest(JSONBuilder);
     bool PhantasmaJsonAPI::ParseGetChainsResponse(JSONValue, vector<Chain>);
     void PhantasmaJsonAPI::MakeGetTokensRequest(JSONBuilder);
     bool PhantasmaJsonAPI::ParseGetTokensResponse(JSONValue, vector<Token>);
     void PhantasmaJsonAPI::MakeGetTokenRequest(JSONBuilder, symbol);
     bool PhantasmaJsonAPI::ParseGetTokenResponse(JSONValue, Token);
     void PhantasmaJsonAPI::MakeGetTokenDataRequest(JSONBuilder, symbol, IDtext);
     bool PhantasmaJsonAPI::ParseGetTokenDataResponse(JSONValue, TokenData);
     void PhantasmaJsonAPI::MakeGetAppsRequest(JSONBuilder);
     bool PhantasmaJsonAPI::ParseGetAppsResponse(JSONValue, vector<App>);
     void PhantasmaJsonAPI::MakeGetTokenTransfersRequest(JSONBuilder, tokenSymbol, page, pageSize);
     bool PhantasmaJsonAPI::ParseGetTokenTransfersResponse(JSONValue, vector<Transaction>);
     void PhantasmaJsonAPI::MakeGetTokenTransferCountRequest(JSONBuilder, tokenSymbol);
     bool PhantasmaJsonAPI::ParseGetTokenTransferCountResponse(JSONValue, Int32);
     void PhantasmaJsonAPI::MakeGetTokenBalanceRequest(JSONBuilder, addressText, tokenSymbol, chainInput);
     bool PhantasmaJsonAPI::ParseGetTokenBalanceResponse(JSONValue, Balance);
     void PhantasmaJsonAPI::MakeGetAuctionsCountRequest(JSONBuilder, chainAddressOrName, symbol);
     bool PhantasmaJsonAPI::ParseGetAuctionsCountResponse(JSONValue, Int32);
     void PhantasmaJsonAPI::MakeGetAuctionsRequest(JSONBuilder, chainAddressOrName, symbol, page, pageSize);
     bool PhantasmaJsonAPI::ParseGetAuctionsResponse(JSONValue, vector<Auction>);
     void PhantasmaJsonAPI::MakeGetAuctionRequest(JSONBuilder, chainAddressOrName, symbol, IDtext);
     bool PhantasmaJsonAPI::ParseGetAuctionResponse(JSONValue, Auction);
     void PhantasmaJsonAPI::MakeGetArchiveRequest(JSONBuilder, hashText);
     bool PhantasmaJsonAPI::ParseGetArchiveResponse(JSONValue, Archive);
     void PhantasmaJsonAPI::MakeWriteArchiveRequest(JSONBuilder, hashText, blockIndex, blockContent);
     bool PhantasmaJsonAPI::ParseWriteArchiveResponse(JSONValue, bool);
     void PhantasmaJsonAPI::MakeGetABIRequest(JSONBuilder, chainAddressOrName, contractName);
     bool PhantasmaJsonAPI::ParseGetABIResponse(JSONValue, ABIContract);
     void PhantasmaJsonAPI::MakeRelaySendRequest(JSONBuilder, receiptHex);
     bool PhantasmaJsonAPI::ParseRelaySendResponse(JSONValue, bool);
     void PhantasmaJsonAPI::MakeRelayReceiveRequest(JSONBuilder, accountInput);
     bool PhantasmaJsonAPI::ParseRelayReceiveResponse(JSONValue, vector<Receipt>);
     void PhantasmaJsonAPI::MakeGetEventsRequest(JSONBuilder, accountInput);
     bool PhantasmaJsonAPI::ParseGetEventsResponse(JSONValue, vector<Event>);

------------------------------------------------------------------------------
 High-level API
------------------------------------------------------------------------------
  If you have defined `PHANTASMA_HTTPCLIENT`, then you can construct a 
   PhantasmaAPI object, which provides a simplified API that hides the 
   internal JSON messaging.

     PhantasmaAPI phantasmaAPI(httpClient);
     Account = phantasmaAPI.GetAccount(addressText, error);
     String = phantasmaAPI.LookUpName(name, error);
     Int32 = phantasmaAPI.GetBlockHeight(chainInput, error);
     Int32 = phantasmaAPI.GetBlockTransactionCountByHash(blockHash, error);
     Block = phantasmaAPI.GetBlockByHash(blockHash, error);
     String = phantasmaAPI.GetRawBlockByHash(blockHash, error);
     Block = phantasmaAPI.GetBlockByHeight(chainInput, height, error);
     String = phantasmaAPI.GetRawBlockByHeight(chainInput, height, error);
     Transaction = phantasmaAPI.GetTransactionByBlockHashAndIndex(blockHash, index, error);
     AccountTransactions = phantasmaAPI.GetAddressTransactions(addressText, page, pageSize, error);
     Int32 = phantasmaAPI.GetAddressTransactionCount(addressText, chainInput, error);
     String = phantasmaAPI.SendRawTransaction(txData, error);
     Script = phantasmaAPI.InvokeRawScript(chainInput, scriptData, error);
     Transaction = phantasmaAPI.GetTransaction(hashText, error);
     String = phantasmaAPI.CancelTransaction(hashText, error);
     vector<Chain> = phantasmaAPI.GetChains(error);
     vector<Token> = phantasmaAPI.GetTokens(error);
     Token = phantasmaAPI.GetToken(symbol, error);
     TokenData = phantasmaAPI.GetTokenData(symbol, IDtext, error);
     vector<App> = phantasmaAPI.GetApps(error);
     vector<Transaction> = phantasmaAPI.GetTokenTransfers(tokenSymbol, page, pageSize, error);
     Int32 = phantasmaAPI.GetTokenTransferCount(tokenSymbol, error);
     Balance = phantasmaAPI.GetTokenBalance(addressText, tokenSymbol, chainInput, error);
     Int32 = phantasmaAPI.GetAuctionsCount(chainAddressOrName, symbol, error);
     vector<Auction> = phantasmaAPI.GetAuctions(chainAddressOrName, symbol, page, pageSize, error);
     Auction = phantasmaAPI.GetAuction(chainAddressOrName, symbol, IDtext, error);
     Archive = phantasmaAPI.GetArchive(hashText, error);
     bool = phantasmaAPI.WriteArchive(hashText, blockIndex, blockContent, error);
     ABIContract = phantasmaAPI.GetABI(chainAddressOrName, contractName, error);
     bool = phantasmaAPI.RelaySend(receiptHex, error);
     vector<Receipt> = phantasmaAPI.RelayReceive(accountInput, error);
     vector<Event> = phantasmaAPI.GetEvents(accountInput, error);

------------------------------------------------------------------------------
 API configuration
------------------------------------------------------------------------------
 As different C++ projects may use different primitive types, you can use the 
  following #defines (BEFORE including `phantasma.h`) to override the default types.

 |#define                  | typedef                   | Default             | Notes                                                  |
 |-------------------------|---------------------------|---------------------|--------------------------------------------------------|
 |`PHANTASMA_BYTE`         | `phantasma::Byte`         | `uint8_t`           |                                                        |
 |`PHANTASMA_INT32`        | `phantasma::Int32`        | `int32_t`           |                                                        |
 |`PHANTASMA_UINT32`       | `phantasma::UInt32`       | `uint32_t`          |                                                        |
 |`PHANTASMA_INT64`        | `phantasma::Int64`        | `int64_t`           |                                                        |
 |`PHANTASMA_UINT64`       | `phantasma::UInt64`       | `uint64_t`          |                                                        |
 |`PHANTASMA_CHAR`         | `phantasma::Char`         | `char`              | See Unicode section                                    |
 |`PHANTASMA_STRING`       | `phantasma::String`       | `std::string`       | Must support construction from `const phantasma::Char*`|
 |`PHANTASMA_STRINGBUILDER`| `phantasma::StringBuilder`| `std::stringstream` |                                                        |
 |`PHANTASMA_VECTOR`       |                           | `std::vector`       | Must support `push_back` and `size` members            |
 |`PHANTASMA_JSONVALUE`    | `phantasma::JSONValue`    | `std::string_view`  | See JSON and Adaptors section                          |
 |`PHANTASMA_JSONARRAY`    | `phantasma::JSONArray`    | `JSONValue`         | See JSON and Adaptors section                          |
 |`PHANTASMA_JSONDOCUMENT` | `phantasma::JSONDocument` | `std::string`       | See JSON and Adaptors section                          |
 |`PHANTASMA_JSONBUILDER`  | `phantasma::JSONBuilder`  | `std::stringstream`*| See JSON and Adaptors section                          |
 |`PHANTASMA_HTTPCLIENT`   | `phantasma::HttpClient`   |                     | See HTTP and Adaptors section                          |

 The behavior of this header can further be modified by using the following 
  `#defines` (BEFORE including `phantasma.h`)
 
 |#define                                        | Notes                   |
 |-----------------------------------------------|-------------------------|
 |`PHANTASMA_EXCEPTION(message)`                 | See Exceptions section  |
 |`PHANTASMA_EXCEPTION_MESSAGE(message, String)` | See Exceptions section  |
 |`PHANTASMA_LITERAL(x)`                         | See Unicode section     |
 |`PHANTASMA_FUNCTION`                           | See Integration section |        
 |`PHANTASMA_IMPLEMENTATION`                     | See Integration section |

------------------------------------------------------------------------------
 Integration
------------------------------------------------------------------------------
 The core of API is provided in the "single header" style to support simple and 
  flexible integration into your project 
  (see https://github.com/nothings/single_file_libs / https://en.wikipedia.org/wiki/Header-only).
 The implementation of function bodies will be excluded unless you define
  `PHANTASMA_IMPLEMENTATION` before including `phantasma.h`.

 See the "Extended/Advanced usage" section, below for details on what is excluded
  from this single header file.

 Typical linking:
  In one CPP file, before including `phantasma.h`:
   `#define PHANTASMA_IMPLEMENTATION`
 
 Inline linking:
  In every CPP file that uses the API, before including `phantasma.h`:
   `#define PHANTASMA_IMPLEMENTATION`
   `#define PHANTASMA_FUNCTION inline`

 Aside from `PHANTASMA_IMPLEMENTATION` / `PHANTASMA_FUNCTION`, you should take 
  care to ensure that every other PHANTASMA_* macro is defined to the same value
  in all of your CPP files that use the phantasma API.

------------------------------------------------------------------------------
 Exceptions
------------------------------------------------------------------------------
 Support for C++ exceptions is opt-in. Before including `phantasma.h`, define
  the following to enable exceptions:

 `#define PHANTASMA_EXCEPTION_ENABLE`

 Alternatively, you can customize the exact type that is thrown by defining:

 `#define PHANTASMA_EXCEPTION(message)                 throw std::runtime_error(message)`
 `#define PHANTASMA_EXCEPTION_MESSAGE(message, string) throw std::runtime_error(string)`

------------------------------------------------------------------------------
 Unicode
------------------------------------------------------------------------------
 To build a wide-character version of the API, define the following before
  including `phantasma.h`:

 `#define PHANTASMA_CHAR          wchar_t`
 `#define PHANTASMA_LITERAL(x)    L ## x`
 `#define PHANTASMA_STRING        std::wstring`
 `#define PHANTASMA_STRINGBUILDER std::wstringstream`

 Alternatively, if `_UNICODE` is defined, then the above macros will be defined
  automatically.

 You should also provide a JSON and HTTP library with wide-character support.

------------------------------------------------------------------------------
 Adaptors
------------------------------------------------------------------------------
 Parts of the Phantasma SDK are designed to plug into external features, such
  as HTTP communications, JSON encoding and advanced cryptography.
 You can configure the SDK to connect to your own implemenations, or existing
  libraries.
 To make integration easier, we provide several "adaptor" header files that 
  contain the required configuration to connect the Phantasma SDK to existing
  popular open source libraries for different features:

 |Library   | Features     | #include file                       | Library URL                             |
 |----------|--------------|-------------------------------------|-----------------------------------------|
 |C++ REST  | HTTP + JSON  | `Adapters/PhantasmaAPI_cpprest.h`   | https://github.com/microsoft/cpprestsdk |
 |libcurl   | HTTP         | `Adapters/PhantasmaAPI_curl.h`      | https://curl.haxx.se/libcurl/           |
 |RapidJSON | JSON         | `Adapters/PhantasmaAPI_rapidjson.h` | http://rapidjson.org/                   |
 |Sodium    | Cryptography | `Adapters/PhantasmaAPI_sodium.h`    | https://libsodium.org                   |

------------------------------------------------------------------------------
 JSON
------------------------------------------------------------------------------
 This header contains JSON parsing and building code, but it is written to be
  as simple as possible (approx 200 lines of code) and is not high-performance
  or highly robust.

 It is recommended that you supply another JSON-parsing API, by defining the
  following macros before including `phantasma.h`:
  `#define PHANTASMA_JSONVALUE    Your_Json_Value_Type`
  `#define PHANTASMA_JSONARRAY    Your_Json_Array_Type`
  `#define PHANTASMA_JSONDOCUMENT Your_JSON_Document_Type`
  `#define PHANTASMA_JSONBUILDER  Your_Json_Serializer_Type`

 **The CPP REST and RapidJSON adaptors implement these macros.**

 Also, this header uses the following procedural API to interact with these types.
 If you have supplied your own JSON types, you must implement the following functions:

     namespace phantasma { namespace json {
     
        JSONValue Parse(const JSONDocument&);
     
        bool      LookupBool(   const JSONValue&, const Char* field, bool& out_error);
        Int32     LookupInt32(  const JSONValue&, const Char* field, bool& out_error);
        UInt32    LookupUInt32( const JSONValue&, const Char* field, bool& out_error);
        String    LookupString( const JSONValue&, const Char* field, bool& out_error);
        JSONValue LookupValue(  const JSONValue&, const Char* field, bool& out_error);
        JSONArray LookupArray(  const JSONValue&, const Char* field, bool& out_error);
        bool      HasField(     const JSONValue&, const Char* field, bool& out_error);
        bool      HasArrayField(const JSONValue&, const Char* field, bool& out_error);
     
        bool      AsBool(       const JSONValue&,                    bool& out_error);
        Int32     AsInt32(      const JSONValue&,                    bool& out_error);
        UInt32    AsUInt32(     const JSONValue&,                    bool& out_error);
        String    AsString(     const JSONValue&,                    bool& out_error);
        JSONArray AsArray(      const JSONValue&,                    bool& out_error);
        bool      IsArray(      const JSONValue&,                    bool& out_error);
        bool      IsObject(     const JSONValue&,                    bool& out_error);
        
        int       ArraySize(    const JSONArray&,                    bool& out_error);
        JSONValue IndexArray(   const JSONArray&, int index,         bool& out_error);
     
                               void BeginObject(JSONBuilder&);
                               void AddString  (JSONBuilder&, const Char* key, const Char* value);
       template<class... Args> void AddArray   (JSONBuilder&, const Char* key, Args...);
                               void EndObject  (JSONBuilder&);
     }}

------------------------------------------------------------------------------
 HTTP
------------------------------------------------------------------------------
 This header does not contain a HTTP client, nor a dependency on any specific
  HTTP client library. If you do not supply a HTTP client library, then only
  the Low-level phantasma API (`PhantasmaJsonAPI`) is available.

 To enable the `PhantasmaAPI` class, defining the following macro before 
  including `phantasma.h`:
 `#define PHANTASMA_HTTPCLIENT   Your_HTTP_Client_Type`

 **The CPP REST and libcurl adaptors implement this macro.**

 Also, this header uses the following procedural API to interact with this type.
 If you have defined `PHANTASMA_HTTPCLIENT`, you must implement the following,
  function, which should perform a HTTP POST request and return the result:

     namespace phantasma {
      JSONDocument HttpPost(HttpClient&, const Char* uri, const JSONBuilder&);
     }

------------------------------------------------------------------------------
 Extended/Advanced usage
------------------------------------------------------------------------------
 This header file contains the entirety of the RPC API requried to communicate 
  with a Phantasma node. If you are not trying to create transactions, this 
  may be enough for you.

 However, for advanced usage, such as creating and signing transactions, much
  more code is required, including cryptography, N-bit ingeger arithmetic, etc.
 The other header files that are included in this distribution, in sub-folders
  listed below, provide these extra features:

  |Directory     | Features                                                              |
  |--------------|-----------------------------------------------------------------------|
  | Adapters     | Configuration for this library to communicate with 3rd party libraries|
  | Blockchain   | Transactions                                                          |
  | Cryptography | Public/Private keys, Signatures, Random numbers, Encryption           |
  | Numerics     | N-bit integer implementation. Base 16/58 ASCII encoding.              |
  | Security     | Practical memory protection.                                          |

------------------------------------------------------------------------------
 - Extended/Advanced usage - Security configuration
------------------------------------------------------------------------------
   To securely process transactions and private keys, it is strongly advised to 
   pair the PhantasmaAPI with strong 3rd party security library.

   **The Sodium adaptor implements these macros.**
   
  |#define                      |                                                                                                              |
  |-----------------------------|--------------------------------------------------------------------------------------------------------------|
  |`PHANTASMA_RANDOMBYTES`      | Fill a memory range with cryptographically secure pseudo-random numbers                                      |
  |`PHANTASMA_WIPEMEM`          | Fill a memory range with 0's in a way that won't be "optimized away"                                         |
  |`PHANTASMA_LOCKMEM`          | Pin the memory pages containing this range, and otherwise inform the OS that it contains secrets.            |
  |`PHANTASMA_UNLOCKMEM`        | Undo the actions of `PHANTASMA_LOCKMEM`, but also fill the memory range with 0's as with `PHANTASMA_WIPEMEM`.|
  |`PHANTASMA_SECURE_ALLOC`     | Similar to malloc, but should return dedicated pages that can have their access permissions modified.        |
  |`PHANTASMA_SECURE_FREE`      | Similar to free - used with allocations returned from `PHANTASMA_SECURE_ALLOC`                               |
  |`PHANTASMA_SECURE_NOACCESS`  | Used with allocations returned from `PHANTASMA_SECURE_ALLOC`. Mark the pages as non-readable.                |
  |`PHANTASMA_SECURE_READONLY`  | Used with allocations returned from `PHANTASMA_SECURE_ALLOC`. Mark the pages as read only.                   |
  |`PHANTASMA_SECURE_READWRITE` | Used with allocations returned from `PHANTASMA_SECURE_ALLOC`. Mark the pages as writable.                    |

------------------------------------------------------------------------------
 - Extended/Advanced usage - Cryptography configuration
------------------------------------------------------------------------------
  To create or validate transactions, an EdDSA Ed25519 implementation is requied.
   The libSodium adaptor implements these macros.
   
  |#define                                |                                                               |
  |---------------------------------------|---------------------------------------------------------------|
  |`PHANTASMA_Ed25519_PublicKeyFromSeed`  | Generate a 32 byte public key from a 32 byte seed.            |
  |`PHANTASMA_Ed25519_PrivateKeyFromSeed` | Generate a 64 byte public key from a 32 byte seed.            |
  |`PHANTASMA_Ed25519_SignDetached`       | Generate a 64 byte signature from a message and a private key.|
  |`PHANTASMA_Ed25519_ValidateDetached`   | Validate a 64 byte signature using a public key.              |


