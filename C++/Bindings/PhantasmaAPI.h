#include "pch.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/ws_client.h>                  // WebSocket client

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace http;						// Common HTTP functionality
using namespace client;						// HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace json;						// JSON library

#pragma once

{{#each types}}{{#if Key!='PaginatedResult'}}
struct {{#fix-type Key}}
{
	{{#each Value}} {{#fix-type FieldType.Name}}{{#if FieldType.IsArray}}*{{/if}} {{Name}};//{{Key.Description}}
	{{/each}}
};
{{#else}}// TODO paginated struct: {{#fix-type Key}}{{/if}}
{{/each}}

class PhantasmaAPI
{
public:
	std::wstring apiHost;
	PhantasmaAPI(std::wstring host);
	~PhantasmaAPI();

	{{#each methods}}//{{Info.Description}}
	{{#if Info.IsPaginated==true}}// TODO paginated api call: {{Info.Name}}
	{{#else}}{{#fix-type Info.ReturnType.Name}} {{Info.Name}} ({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}} {{#if !@last}}, {{/if}}{{/each}}){{/if}}
	
	{{/each}}
};

