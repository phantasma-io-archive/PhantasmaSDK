#define PHANTASMA_IMPLEMENTATION
#define CURL_STATICLIB
#include "../../../Libs/Adapters/PhantasmaAPI_rapidjson.h"
#include "../../../Libs/Adapters/PhantasmaAPI_curl.h"
#include "../../../Libs/PhantasmaAPI.h"
#include <iostream>

using namespace phantasma;
using namespace phantasma::rpc;

//Sorry, I haven't actually bundled a compiled version of libCurl with the project.
//You have to download/build libCurl yourself!
#pragma comment(lib, "libcurl_a.lib")

int main()
{
	std::string host = "http://localhost:7077";
	CurlClient http(host);
	PhantasmaAPI phantasmaAPI(http);

	//std::string address = "P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr";	//genesis address
	//std::string address = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";	//nft address

	const char* wif = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";

	PhantasmaError error;
	Account account = phantasmaAPI.GetAccount(wif, &error);

	if( !error.code )
	{
		std::cout << "Balance description for address " << wif << std::endl;

		for (int i = 0; i < account.balances.size(); i++) 
		{
			std::cout << account.balances[i].amount << " " << account.balances[i].symbol << " tokens available on " << account.balances[i].chain << " chain" << std::endl;
		}
	}
	else
	{
		std::cout << "Unable to communicate with the RPC node" << std::endl;
	}
}
