
#define PHANTASMA_IMPLEMENTATION
#include "cpprest_PhantasmaAPI.h"

int main()
{
	std::wstring host = L"http://localhost:7077";
	web::http::client::http_client http(host);
	phantasma::PhantasmaAPI api(http);

	//std::wstring address = L"P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr";	//genesis address
	//std::wstring address = L"NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";	//nft address

	const wchar_t* wif = L"NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";

	try
	{
		phantasma::Account account = api.GetAccount(wif);

		std::wcout << L"Balance description for address " << wif << std::endl;

		for (int i = 0; i < account.balances.size(); i++) 
		{
			std::wcout << account.balances[i].amount << " " << account.balances[i].symbol << " tokens available on " << account.balances[i].chain << " chain" << std::endl;
		}
	}
	catch(std::exception& e)
	{
		std::wcout << e.what();
	}
}
