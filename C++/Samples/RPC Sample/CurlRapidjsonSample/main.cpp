#define PHANTASMA_IMPLEMENTATION
#include "curl_rapidjson_PhantasmaAPI.h"
#include <iostream>

int main()
{
	//std::string address = "P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr";	//genesis address
	//std::string address = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";	//nft address

	const char* wif = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";

	CurlClient http;
	phantasma::PhantasmaAPI phantasmaAPI(http);
	phantasma::Account account = phantasmaAPI.GetAccount(wif);

	std::cout << "Balance description for address " << wif << std::endl;

	for (int i = 0; i < account.balances.size(); i++) 
	{
		std::cout << account.balances[i].amount << " " << account.balances[i].symbol << " tokens available on " << account.balances[i].chain << " chain" << std::endl;
	}
}
