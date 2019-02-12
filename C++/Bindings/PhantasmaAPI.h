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

{{#each types}}
struct {{#fix-type Key}}Result
{
	{{#each Value}} {{#fix-type FieldType.Name}}{{#if FieldType.IsArray}}* {{/if}}{{Name}}; //{{Key.Description}}
	{{/each}}
};


class PhantasmaAPI
{
public:
	PhantasmaAPI();
	~PhantasmaAPI();

	{{#each methods}}
	//{{Info.Description}}
	{{Info.ReturnType.Name}} {{Info.Name}} ({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}} {{#if !@last}}, {{/if}}{{/each}})
};

