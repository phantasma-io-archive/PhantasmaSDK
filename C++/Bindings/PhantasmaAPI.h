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

{{#each types}}
struct {{#fix-type Key}}{{#parse-lines false}}{{#new-line}}
{
{{#new-line}}
{{#each Value}}
	{{#if FieldType.IsArray}}std::vector<{{/if}}{{#fix-type FieldType.Name}}{{#if FieldType.IsArray}}>{{/if}} {{Name}};//{{Key.Description}}
{{#new-line}}
{{/each}}
{{#parse-lines true}}};
{{/each}}

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

	{{#each types}}{{#fix-type Key}} Deserialize{{#fix-type Key}}(json::value json);
	{{/each}}

public:
	PhantasmaAPI(std::string host);
	~PhantasmaAPI();

	{{#each methods}}//{{Info.Description}}
	{{#if Info.IsPaginated==true}}//paginated call
	{{/if}}{{#if Info.ReturnType.IsArray}}std::vector<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} {{Info.Name}}({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}}{{#if !@last}}, {{/if}}{{/each}});
	
	
	{{/each}}
};

