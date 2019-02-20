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

{{#each types}}
{{#fix-type Key}} PhantasmaAPI::Deserialize{{#fix-type Key}}(json::value json)
{
{{#parse-lines false}}
{{#each Value}}
{{#if FieldType.Name=='String[]'}}{{#parse-lines true}}
	std::vector<{{#fix-type FieldType.Name}}> {{Name}}Vector;
	if(json.has_array_field(U("{{Name}}")))
	{
		json::array {{Name}}JsonArray = json.at(U("{{Name}}")).as_array();

		for(int i = 0; i < {{Name}}JsonArray.size(); i++)
		{
			{{Name}}Vector.push_back(conversions::to_utf8string({{Name}}JsonArray[i].as_string()));
		}
	}{{#parse-lines false}}
{{#else}}
{{#if FieldType.Name contains '[]'}}{{#parse-lines true}}
	std::vector<{{#fix-type FieldType.Name}}> {{Name}}Vector;
	if(json.has_array_field(U("{{Name}}")))
	{
		json::array {{Name}}JsonArray = json.at(U("{{Name}}")).as_array();

		for(int i = 0; i < {{Name}}JsonArray.size(); i++)
		{
			{{Name}}Vector.push_back(Deserialize{{#fix-type FieldType.Name}}({{Name}}JsonArray[i]));
		}
	}{{#parse-lines false}}
{{#else}}
{{#if FieldType.Name=='UInt32'}}
	uint32_t {{Name}} = FieldToNumber(json, U("{{Name}}"));{{#new-line}}

{{#else}}
{{#if FieldType.Name=='Int32'}}
	uint32_t {{Name}} = FieldToNumber(json, U("{{Name}}"));{{#new-line}}

{{#else}}
{{#if FieldType.Name=='String'}}
	std::string {{Name}} = FieldToString(json, U("{{Name}}"));{{#new-line}}
{{#else}}
{{#if FieldType.Name=='IAPIResult'}}
	json::value {{Name}} = json.at(U("result"));{{#new-line}}
{{#else}}
	"Variable type {{FieldType.Name}} isnt currently handled by the template system"{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}
{{/each}}
{{#parse-lines true}}
	return {{#fix-type Key}} { {{#each Value}}{{Name}}{{#if FieldType.Name contains '[]'}}Vector{{/if}}{{#if !@last}}, {{/if}}{{/each}} };
}
{{/each}}

{{#each methods}}//{{#if Info.IsPaginated}}Paginated api call: {{/if}}{{Info.Description}}
{{#if Info.ReturnType.IsArray}}std::vector<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} PhantasmaAPI::{{Info.Name}}({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}}{{#if !@last}}, {{/if}}{{/each}})
{
	RPCRequest body("{{#camel-case Info.Name}}");
	{{#if Info.Parameters}}body.AddParameter({{#each Info.Parameters}}{{Name}}{{#if !@last}}, {{/if}}{{/each}});{{/if}}
	json::value request = body.BuildRequest();
	json::value jsonResponse = SendRequest(request);
	{{#if Info.ReturnType.IsArray}}std::vector<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} output;

	{{#if Info.IsPaginated}}Paginated pageStruct = DeserializePaginated(jsonResponse);

{{#parse-lines false}}
{{#if Info.ReturnType.IsArray}}
	for(int i = 0; i < pageStruct.result.size(); i++){{#new-line}}
	{
{{#new-line}}
{{#if Info.ReturnType.Name=='UInt32'}}
		output.push_back(pageStruct.result[i].as_number().to_uint32());{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='Int32'}}
		output.push_back(pageStruct.result[i].as_number().to_uint32());{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
		output.push_back(conversions::to_utf8string(pageStruct.result[i].as_string()));{{#new-line}}
{{#else}}
		output.push_back(Deserialize{{#fix-type Info.ReturnType.Name}}(pageStruct.result[i]));{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
	}{{#new-line}}
{{/if}}
{{#if Info.ReturnType.IsArray==false}}
	output = Deserialize{{#fix-type Info.ReturnType.Name}}(pageStruct.result);{{#new-line}}
{{/if}}
{{#new-line}}
	return output;{{#new-line}}
}
{{#parse-lines true}}{{/if}}{{#if Info.IsPaginated==false}}{{#parse-lines false}}
{{#if Info.ReturnType.IsArray}}
if (!jsonResponse.is_array()){{#new-line}}
		throw json_exception("Malformed response: No JSON array on the \"result\" node");{{#new-line}}
{{#new-line}}
	json::array resultArray = jsonResponse.as_array();{{#new-line}}
	for(int i = 0; i < resultArray.size(); i++){{#new-line}}
	{
{{#new-line}}
{{#if Info.ReturnType.Name=='UInt32'}}
		output.push_back(resultArray[i].as_number().to_uint32());{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='Int32'}}
		output.push_back(resultArray[i].as_number().to_uint32());{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
		output.push_back(conversions::to_utf8string(resultArray[i].as_string()));{{#new-line}}
{{#else}}
		output.push_back(Deserialize{{#fix-type Info.ReturnType.Name}}(resultArray[i]));{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
	}{{#new-line}}
{{/if}}

{{#if Info.ReturnType.IsArray==false}}
{{#if Info.ReturnType.Name=='UInt32'}}
output = jsonResponse.as_number().to_uint32();{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='Int32'}}
output = jsonResponse.as_number().to_uint32();{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
output = conversions::to_utf8string(jsonResponse.as_string());{{#new-line}}
{{#else}}
output = Deserialize{{#fix-type Info.ReturnType.Name}}(jsonResponse);{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}
{{#new-line}}
	return output;{{#new-line}}
}
{{#parse-lines true}}

{{/if}}
{{/each}}