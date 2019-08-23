//------------------------------------------------------------------------------
// This sample does NOT actually communicate with the network/chain. It only 
//  serves to demonstrate how to use the low-level JSON API, allowing you to 
//  perform HTTP requests yourself.
//------------------------------------------------------------------------------

#define PHANTASMA_IMPLEMENTATION
#include "../../../Libs/PhantasmaAPI.h"
#include <iostream>

using namespace phantasma;
using namespace phantasma::rpc;

const char* DoHttpPost( const char* uri, const std::string& request )
{
	//This is where you'd perform the HTTP POST request...
	//Hard-coded response:
	return R"({"jsonrpc": "2.0", "result": {"address" : "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE","name" : "anonymous","balances" : [{"chain" : "main","amount" : "208000046079","symbol" : "SOUL","decimals" : 10},{"chain" : "main","amount" : "5000000000000","symbol" : "NEOSOUL","decimals" : 8},{"chain" : "main","amount" : "6","symbol" : "NACHO","decimals" : 0,"ids" : ["101","102","103","104","105","106"]}]}, "id": 1})";
}

int main()
{
	//std::string address = "P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr";	//genesis address
	//std::string address = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";	//nft address

	const char* wif = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";

	JSONBuilder json;
	PhantasmaJsonAPI::MakeGetAccountRequest(json, wif);
	
	JSONValue response = DoHttpPost(PhantasmaJsonAPI::Uri(), json.s.str());

	Account account;
	PhantasmaJsonAPI::ParseGetAccountResponse(response, account);

	std::cout << "Balance description for address " << wif << std::endl;

	for (int i = 0; i < account.balances.size(); i++) 
	{
		std::cout << account.balances[i].amount << " " << account.balances[i].symbol << " tokens available on " << account.balances[i].chain << " chain" << std::endl;
	}
}
