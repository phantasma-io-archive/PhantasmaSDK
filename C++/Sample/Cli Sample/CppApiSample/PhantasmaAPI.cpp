#include "pch.h"
#include "PhantasmaAPI.h"

RPCRequest::RPCRequest(std::string method)
{
	requestBody = 
	{
		{"jsonrpc", "2.0"},
		{"method", method},
		{"id", "1"},
		{"params", {}}
	};
}

template<typename T>
void RPCRequest::AddParameter(T param)
{
	requestBody["params"].emplace_back(param);
}

json::value RPCRequest::BuildRequest()
{
	std::string str = requestBody.dump();
	return json::value::parse(converter.from_bytes(str));
}

template<typename T, typename... Args>
void RPCRequest::AddParameter(T param, Args... params)
{
	AddParameter(param);
	AddParameter(params...);
}


PhantasmaAPI::PhantasmaAPI(std::string host)
{
	apiHost = converter.from_bytes(host);
}

PhantasmaAPI::~PhantasmaAPI()
= default;

std::string PhantasmaAPI::FieldToString(value json, const string_t field)
{
	if(!json.has_string_field(field))
	{
		std::string str1 = "Unexpected JSON format: missing string field ";
		std::string str2 = (std::string) converter.to_bytes(field);
		throw new std::exception(str1.append(str2).c_str());
	}

	return conversions::to_utf8string(json.at(field).as_string());
}

uint32_t PhantasmaAPI::FieldToNumber(value json, const string_t field)
{
	if (!json.has_number_field(field))
	{
		std::string str1 = "Unexpected JSON format: missing number field ";
		std::string str2 = (std::string) converter.to_bytes(field);
		throw new std::exception(str1.append(str2).c_str());
	}

	return json.at(field).as_number().to_uint32();
}

json::value PhantasmaAPI::SendRequest(const json::value& body)
{
	http_client client(apiHost);

	// Build request URI and start the request.
	uri_builder builder(U("/rpc"));
	json::value output;

	pplx::task<json::value> requestTask = client.request(methods::POST, builder.to_string(), body)
		.then([&](http_response response) -> json::value		// Handle response headers arriving.
	{
		const auto statusCode = response.status_code();
		printf("Received response status code:%u\n", statusCode);

		if (statusCode != 200)
			throw http_exception("Malformed RPC request or endpoint: response status = " + statusCode);
			

		response.content_ready().wait();

		auto json = response.extract_json().then([&](value body) -> json::value
		{
			if (body.is_null())
				throw json_exception("Failed to parse JSON");

			if (body.has_field(U("error")))
				throw std::exception(("Error: " + conversions::to_utf8string(body.at(U("error")).at(U("message")).as_string())).c_str());

			if (!body.has_field(U("result")))
				throw json_exception("Malformed response: No \"result\" node on the JSON body");
			
			return body.at(U("result"));
		}).get();

		return json;
	});

	try
	{
		output = requestTask.get();
	}
	catch (const std::exception &e)
	{
		printf("Error exception:%s\n", e.what());
	}

	return output;
}


Balance PhantasmaAPI::DeserializeBalance(json::value json)
{
	std::string chain = FieldToString(json, U("chain"));
	std::string amount = FieldToString(json, U("amount"));
	std::string symbol = FieldToString(json, U("symbol"));
	uint32_t decimals = FieldToNumber(json, U("decimals"));

	std::vector<std::string> idsVector;
	if(json.has_array_field(U("ids")))
	{
		json::array idsJsonArray = json.at(U("ids")).as_array();

		for(int i = 0; i < idsJsonArray.size(); i++)
		{
			idsVector.push_back(conversions::to_utf8string(idsJsonArray[i].as_string()));
		}
	}
	return Balance { chain, amount, symbol, decimals, idsVector };
}

Account PhantasmaAPI::DeserializeAccount(json::value json)
{
	std::string address = FieldToString(json, U("address"));
	std::string name = FieldToString(json, U("name"));

	std::vector<Balance> balancesVector;
	if(json.has_array_field(U("balances")))
	{
		json::array balancesJsonArray = json.at(U("balances")).as_array();

		for(int i = 0; i < balancesJsonArray.size(); i++)
		{
			balancesVector.push_back(DeserializeBalance(balancesJsonArray[i]));
		}
	}
	return Account { address, name, balancesVector };
}

Chain PhantasmaAPI::DeserializeChain(json::value json)
{
	std::string name = FieldToString(json, U("name"));
	std::string address = FieldToString(json, U("address"));
	std::string parentAddress = FieldToString(json, U("parentAddress"));
	uint32_t height = FieldToNumber(json, U("height"));

	return Chain { name, address, parentAddress, height };
}

App PhantasmaAPI::DeserializeApp(json::value json)
{
	std::string id = FieldToString(json, U("id"));
	std::string title = FieldToString(json, U("title"));
	std::string url = FieldToString(json, U("url"));
	std::string description = FieldToString(json, U("description"));
	std::string icon = FieldToString(json, U("icon"));

	return App { id, title, url, description, icon };
}

Event PhantasmaAPI::DeserializeEvent(json::value json)
{
	std::string address = FieldToString(json, U("address"));
	std::string kind = FieldToString(json, U("kind"));
	std::string data = FieldToString(json, U("data"));

	return Event { address, kind, data };
}

Transaction PhantasmaAPI::DeserializeTransaction(json::value json)
{
	std::string hash = FieldToString(json, U("hash"));
	std::string chainAddress = FieldToString(json, U("chainAddress"));
	uint32_t timestamp = FieldToNumber(json, U("timestamp"));
	uint32_t blockHeight = FieldToNumber(json, U("blockHeight"));
	std::string script = FieldToString(json, U("script"));

	std::vector<Event> eventsVector;
	if(json.has_array_field(U("events")))
	{
		json::array eventsJsonArray = json.at(U("events")).as_array();

		for(int i = 0; i < eventsJsonArray.size(); i++)
		{
			eventsVector.push_back(DeserializeEvent(eventsJsonArray[i]));
		}
	}	std::string result = FieldToString(json, U("result"));

	return Transaction { hash, chainAddress, timestamp, blockHeight, script, eventsVector, result };
}

AccountTransactions PhantasmaAPI::DeserializeAccountTransactions(json::value json)
{
	std::string address = FieldToString(json, U("address"));

	std::vector<Transaction> txsVector;
	if(json.has_array_field(U("txs")))
	{
		json::array txsJsonArray = json.at(U("txs")).as_array();

		for(int i = 0; i < txsJsonArray.size(); i++)
		{
			txsVector.push_back(DeserializeTransaction(txsJsonArray[i]));
		}
	}
	return AccountTransactions { address, txsVector };
}

Paginated PhantasmaAPI::DeserializePaginated(json::value json)
{
	uint32_t page = FieldToNumber(json, U("page"));
	uint32_t pageSize = FieldToNumber(json, U("pageSize"));
	uint32_t total = FieldToNumber(json, U("total"));
	uint32_t totalPages = FieldToNumber(json, U("totalPages"));
	json::value result = json.at(U("result"));

	return Paginated { page, pageSize, total, totalPages, result };
}

Block PhantasmaAPI::DeserializeBlock(json::value json)
{
	std::string hash = FieldToString(json, U("hash"));
	std::string previousHash = FieldToString(json, U("previousHash"));
	uint32_t timestamp = FieldToNumber(json, U("timestamp"));
	uint32_t height = FieldToNumber(json, U("height"));
	std::string chainAddress = FieldToString(json, U("chainAddress"));
	std::string payload = FieldToString(json, U("payload"));

	std::vector<Transaction> txsVector;
	if(json.has_array_field(U("txs")))
	{
		json::array txsJsonArray = json.at(U("txs")).as_array();

		for(int i = 0; i < txsJsonArray.size(); i++)
		{
			txsVector.push_back(DeserializeTransaction(txsJsonArray[i]));
		}
	}	std::string validatorAddress = FieldToString(json, U("validatorAddress"));
	std::string reward = FieldToString(json, U("reward"));

	return Block { hash, previousHash, timestamp, height, chainAddress, payload, txsVector, validatorAddress, reward };
}

TokenMetadata PhantasmaAPI::DeserializeTokenMetadata(json::value json)
{
	std::string key = FieldToString(json, U("key"));
	std::string value = FieldToString(json, U("value"));

	return TokenMetadata { key, value };
}

Token PhantasmaAPI::DeserializeToken(json::value json)
{
	std::string symbol = FieldToString(json, U("symbol"));
	std::string name = FieldToString(json, U("name"));
	uint32_t decimals = FieldToNumber(json, U("decimals"));
	std::string currentSupply = FieldToString(json, U("currentSupply"));
	std::string maxSupply = FieldToString(json, U("maxSupply"));
	std::string ownerAddress = FieldToString(json, U("ownerAddress"));

	std::vector<TokenMetadata> metadataListVector;
	if(json.has_array_field(U("metadataList")))
	{
		json::array metadataListJsonArray = json.at(U("metadataList")).as_array();

		for(int i = 0; i < metadataListJsonArray.size(); i++)
		{
			metadataListVector.push_back(DeserializeTokenMetadata(metadataListJsonArray[i]));
		}
	}	std::string flags = FieldToString(json, U("flags"));

	return Token { symbol, name, decimals, currentSupply, maxSupply, ownerAddress, metadataListVector, flags };
}

TokenData PhantasmaAPI::DeserializeTokenData(json::value json)
{
	std::string ID = FieldToString(json, U("ID"));
	std::string chainAddress = FieldToString(json, U("chainAddress"));
	std::string ownerAddress = FieldToString(json, U("ownerAddress"));
	std::string ram = FieldToString(json, U("ram"));
	std::string rom = FieldToString(json, U("rom"));

	return TokenData { ID, chainAddress, ownerAddress, ram, rom };
}

TxConfirmation PhantasmaAPI::DeserializeTxConfirmation(json::value json)
{
	std::string hash = FieldToString(json, U("hash"));
	std::string chainAddress = FieldToString(json, U("chainAddress"));
	uint32_t confirmations = FieldToNumber(json, U("confirmations"));
	uint32_t height = FieldToNumber(json, U("height"));

	return TxConfirmation { hash, chainAddress, confirmations, height };
}

SendRawTx PhantasmaAPI::DeserializeSendRawTx(json::value json)
{
	std::string hash = FieldToString(json, U("hash"));
	std::string error = FieldToString(json, U("error"));

	return SendRawTx { hash, error };
}

Auction PhantasmaAPI::DeserializeAuction(json::value json)
{
	std::string creatorAddress = FieldToString(json, U("creatorAddress"));
	uint32_t startDate = FieldToNumber(json, U("startDate"));
	uint32_t endDate = FieldToNumber(json, U("endDate"));
	std::string baseSymbol = FieldToString(json, U("baseSymbol"));
	std::string quoteSymbol = FieldToString(json, U("quoteSymbol"));
	std::string tokenId = FieldToString(json, U("tokenId"));
	std::string price = FieldToString(json, U("price"));

	return Auction { creatorAddress, startDate, endDate, baseSymbol, quoteSymbol, tokenId, price };
}


//Returns the account name and balance of given address.
Account PhantasmaAPI::GetAccount(std::string addressText)
{
	RPCRequest body("getAccount");
	body.AddParameter(addressText);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Account output;

	output = DeserializeAccount(jsonResponse);

	return output;
}


//Returns the height of a chain.
uint32_t PhantasmaAPI::GetBlockHeight(std::string chainInput)
{
	RPCRequest body("getBlockHeight");
	body.AddParameter(chainInput);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	uint32_t output;

	output = jsonResponse.as_number().to_uint32();

	return output;
}


//Returns the number of transactions of given block hash or error if given hash is invalid or is not found.
uint32_t PhantasmaAPI::GetBlockTransactionCountByHash(std::string blockHash)
{
	RPCRequest body("getBlockTransactionCountByHash");
	body.AddParameter(blockHash);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	uint32_t output;

	output = jsonResponse.as_number().to_uint32();

	return output;
}


//Returns information about a block by hash.
Block PhantasmaAPI::GetBlockByHash(std::string blockHash)
{
	RPCRequest body("getBlockByHash");
	body.AddParameter(blockHash);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Block output;

	output = DeserializeBlock(jsonResponse);

	return output;
}


//Returns a serialized string, containing information about a block by hash.
std::string PhantasmaAPI::GetRawBlockByHash(std::string blockHash)
{
	RPCRequest body("getRawBlockByHash");
	body.AddParameter(blockHash);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::string output;

	output = conversions::to_utf8string(jsonResponse.as_string());

	return output;
}


//Returns information about a block by height and chain.
Block PhantasmaAPI::GetBlockByHeight(std::string chainInput, uint32_t height)
{
	RPCRequest body("getBlockByHeight");
	body.AddParameter(chainInput, height);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Block output;

	output = DeserializeBlock(jsonResponse);

	return output;
}


//Returns a serialized string, in hex format, containing information about a block by height and chain.
std::string PhantasmaAPI::GetRawBlockByHeight(std::string chainInput, uint32_t height)
{
	RPCRequest body("getRawBlockByHeight");
	body.AddParameter(chainInput, height);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::string output;

	output = conversions::to_utf8string(jsonResponse.as_string());

	return output;
}


//Returns the information about a transaction requested by a block hash and transaction index.
Transaction PhantasmaAPI::GetTransactionByBlockHashAndIndex(std::string blockHash, uint32_t index)
{
	RPCRequest body("getTransactionByBlockHashAndIndex");
	body.AddParameter(blockHash, index);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Transaction output;

	output = DeserializeTransaction(jsonResponse);

	return output;
}


//Paginated api call: Returns last X transactions of given address.
AccountTransactions PhantasmaAPI::GetAddressTransactions(std::string addressText, uint32_t page, uint32_t pageSize)
{
	RPCRequest body("getAddressTransactions");
	body.AddParameter(addressText, page, pageSize);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	AccountTransactions output;

	Paginated pageStruct = DeserializePaginated(jsonResponse);

	output = DeserializeAccountTransactions(pageStruct.result);

	return output;
}
//Get number of transactions in a specific address and chain
uint32_t PhantasmaAPI::GetAddressTransactionCount(std::string addressText, std::string chainInput)
{
	RPCRequest body("getAddressTransactionCount");
	body.AddParameter(addressText, chainInput);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	uint32_t output;

	output = jsonResponse.as_number().to_uint32();

	return output;
}


//Returns the number of confirmations of given transaction hash and other useful info.
uint32_t PhantasmaAPI::GetConfirmations(std::string hashText)
{
	RPCRequest body("getConfirmations");
	body.AddParameter(hashText);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	uint32_t output;

	output = jsonResponse.as_number().to_uint32();

	return output;
}


//Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually.
std::string PhantasmaAPI::SendRawTransaction(std::string txData)
{
	RPCRequest body("sendRawTransaction");
	body.AddParameter(txData);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::string output;

	output = conversions::to_utf8string(jsonResponse.as_string());

	return output;
}


//Returns information about a transaction by hash.
Transaction PhantasmaAPI::GetTransaction(std::string hashText)
{
	RPCRequest body("getTransaction");
	body.AddParameter(hashText);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Transaction output;

	output = DeserializeTransaction(jsonResponse);

	return output;
}


//Removes a pending transaction from the mempool.
std::string PhantasmaAPI::CancelTransaction(std::string hashText)
{
	RPCRequest body("cancelTransaction");
	body.AddParameter(hashText);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::string output;

	output = conversions::to_utf8string(jsonResponse.as_string());

	return output;
}


//Returns an array of all chains deployed in Phantasma.
std::vector<Chain> PhantasmaAPI::GetChains()
{
	RPCRequest body("getChains");
	
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::vector<Chain> output;

	if (!jsonResponse.is_array())
		throw json_exception("Malformed response: No JSON array on the \"result\" node");

	json::array resultArray = jsonResponse.as_array();
	for(int i = 0; i < resultArray.size(); i++)
	{
		output.push_back(DeserializeChain(resultArray[i]));
	}

	return output;
}


//Returns an array of tokens deployed in Phantasma.
std::vector<Token> PhantasmaAPI::GetTokens()
{
	RPCRequest body("getTokens");
	
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::vector<Token> output;

	if (!jsonResponse.is_array())
		throw json_exception("Malformed response: No JSON array on the \"result\" node");

	json::array resultArray = jsonResponse.as_array();
	for(int i = 0; i < resultArray.size(); i++)
	{
		output.push_back(DeserializeToken(resultArray[i]));
	}

	return output;
}


//Returns info about a specific token deployed in Phantasma.
Token PhantasmaAPI::GetToken(std::string symbol)
{
	RPCRequest body("getToken");
	body.AddParameter(symbol);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Token output;

	output = DeserializeToken(jsonResponse);

	return output;
}


//Returns data of a non-fungible token, in hexadecimal format.
TokenData PhantasmaAPI::GetTokenData(std::string symbol, std::string IDtext)
{
	RPCRequest body("getTokenData");
	body.AddParameter(symbol, IDtext);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	TokenData output;

	output = DeserializeTokenData(jsonResponse);

	return output;
}


//Returns an array of apps deployed in Phantasma.
std::vector<App> PhantasmaAPI::GetApps()
{
	RPCRequest body("getApps");
	
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::vector<App> output;

	if (!jsonResponse.is_array())
		throw json_exception("Malformed response: No JSON array on the \"result\" node");

	json::array resultArray = jsonResponse.as_array();
	for(int i = 0; i < resultArray.size(); i++)
	{
		output.push_back(DeserializeApp(resultArray[i]));
	}

	return output;
}


//Paginated api call: Returns last X transactions of given token.
std::vector<Transaction> PhantasmaAPI::GetTokenTransfers(std::string tokenSymbol, uint32_t page, uint32_t pageSize)
{
	RPCRequest body("getTokenTransfers");
	body.AddParameter(tokenSymbol, page, pageSize);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::vector<Transaction> output;

	Paginated pageStruct = DeserializePaginated(jsonResponse);

	for(int i = 0; i < pageStruct.result.size(); i++)
	{
		output.push_back(DeserializeTransaction(pageStruct.result[i]));
	}

	return output;
}
//Returns the number of transaction of a given token.
uint32_t PhantasmaAPI::GetTokenTransferCount(std::string tokenSymbol)
{
	RPCRequest body("getTokenTransferCount");
	body.AddParameter(tokenSymbol);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	uint32_t output;

	output = jsonResponse.as_number().to_uint32();

	return output;
}


//Returns the balance for a specific token and chain, given an address.
Balance PhantasmaAPI::GetTokenBalance(std::string addressText, std::string tokenSymbol, std::string chainInput)
{
	RPCRequest body("getTokenBalance");
	body.AddParameter(addressText, tokenSymbol, chainInput);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	Balance output;

	output = DeserializeBalance(jsonResponse);

	return output;
}


//Returns the number of active auctions.
uint32_t PhantasmaAPI::GetAuctionsCount(std::string symbol)
{
	RPCRequest body("getAuctionsCount");
	body.AddParameter(symbol);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	uint32_t output;

	output = jsonResponse.as_number().to_uint32();

	return output;
}


//Paginated api call: Returns the auctions available in the market.
std::vector<Auction> PhantasmaAPI::GetAuctions(std::string symbol, uint32_t page, uint32_t pageSize)
{
	RPCRequest body("getAuctions");
	body.AddParameter(symbol, page, pageSize);
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	std::vector<Auction> output;

	Paginated pageStruct = DeserializePaginated(jsonResponse);

	for(int i = 0; i < pageStruct.result.size(); i++)
	{
		output.push_back(DeserializeAuction(pageStruct.result[i]));
	}

	return output;
}
