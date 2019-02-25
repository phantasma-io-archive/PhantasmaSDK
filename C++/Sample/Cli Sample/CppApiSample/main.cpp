// C++SpookTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
#include "PhantasmaAPI.h"
#include <Windows.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace web::json;                  // JSON library

int main()
{
	std::string host = "http://localhost:7077";
	PhantasmaAPI api(host);

	//std::wstring address = L"P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr";	//genesis address
	//std::wstring address = L"NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";	//nft address

	std::string wif = "NztsEZP7dtrzRBagogUYVp6mgEFbhjZfvHMVkd2bYWJfE";

	Account account = api.GetAccount(wif);

	printf("Balance description for address %s", wif);

	for (int i = 0; i < account.balances.size(); i++) 
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wostringstream os;
		os << converter.from_bytes(account.balances[i].amount) << " " << converter.from_bytes(account.balances[i].symbol) << " tokens available on " << converter.from_bytes(account.balances[i].chain) << " chain" << std::endl;
		OutputDebugString((os.str().c_str()));
	}
}
