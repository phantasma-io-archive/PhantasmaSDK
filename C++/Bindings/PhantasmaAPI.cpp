#include "pch.h"
#include "PhantasmaAPI.h"

{{#each types}}
{{#fix-type Key}} Deserialize{{#fix-type Key}}(json::value json)
{
	{{#each Value}}
		{{#if FieldType.Name=='String[]'}}
			{{#fix-type FieldType.Name}} *{{Name}}Ptr = nullptr;
			if(json.has_array_field(U("{{Name}}")))
			{
				json::array {{Name}}JsonArray = json.at(U("{{Name}}")).asArray();
				{{Name}}Ptr = new {{#fix-type FieldType.Name}}[{{Name}}JsonArray.size()];

				for(int i = 0; i < {{Name}}JsonArray.size(); i++)
				{
					{{Name}}Ptr[i] = {{Name}}JsonArray[i];
				}
			}
		{{#else}}
		{{#if FieldType.Name contains '[]'}}
			{{#fix-type FieldType.Name}} *{{Name}}Ptr = nullptr;
			if(json.has_array_field(U("{{Name}}")))
			{
				json::array {{Name}}JsonArray = json.at(U("{{Name}}")).asArray();
				{{Name}}Ptr = new {{#fix-type FieldType.Name}}[{{Name}}JsonArray.size()];

				for(int i = 0; i < {{Name}}JsonArray.size(); i++)
				{
					{{Name}}Ptr[i] = Deserialize{{#fix-type FieldType.Name}}(JsonArray[i]);
				}
			}
		{{#else}}
		{{#if FieldType.Name=='UInt32'}}
			uint32_t {{Name}} = FieldToNumber(json, U("{{Name}}"));

		{{#else}}
		{{#if FieldType.Name=='Int32'}}
			uint32_t {{Name}} = FieldToNumber(json, U("{{Name}}"));

		{{#else}}
		{{#if FieldType.Name=='String'}}
			std::string {{Name}} = FieldToString(json, U("{{Name}}"));

		{{#else}}
			"Variable type {{FieldType.Name}} isnt currently handled by the template system"
		{{/if}}
		{{/if}}
		{{/if}}
		{{/if}}
		{{/if}}
	{{/each}}

	return new {{#fix-type Key}} { {{#each Value}}{{Name}}{{#if FieldType.Name contains '[]'}}Ptr{{/if}}{{#if !@last}}, {{/if}}{{/each}} };
};
{{/each}}


PhantasmaAPI::PhantasmaAPI(std::string host)
{
	apiHost = converter.from_bytes(host);
}

PhantasmaAPI::~PhantasmaAPI()= default;

std::string FieldToString(value json, const string_t field)
{
	if(!json.has_string_field(field))
		throw new std::exception("Unexpected JSON format: missing string field " + field);

	return conversions::to_utf8string(json.at(field).as_string());
}

uint32_t FieldToNumber(value json, const string_t field)
{
	return json.at(field).as_number().to_uint32();
}

{{#each methods}}//{{Info.Description}}
{{#if Info.IsPaginated==true}}// TODO paginated api call: {{Info.Name}}
{{#else}}{{#fix-type Info.ReturnType.Name}} {{Info.Name}} ({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}} {{#if !@last}}, {{/if}}{{/each}}){{/if}}

{{/each}}


