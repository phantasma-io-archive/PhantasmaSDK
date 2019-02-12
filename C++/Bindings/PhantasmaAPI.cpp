#include "pch.h"
#include "PhantasmaAPI.h"

PhantasmaAPI::PhantasmaAPI()
{
}


PhantasmaAPI::~PhantasmaAPI()
{
}

std::string FieldToString(value json, string_t field)
{
	return utility::conversions::to_utf8string(json.at(field).as_string().c_str());
}

uint16_t FieldToNumber(value json, string_t field)
{
	return json.at(field).as_integer();
}

{{#each types}}
struct {{#fix-type Key}}
{
	{{#each Value}} {{#fix-type FieldType.Name}} {{#if FieldType.IsArray}}* {{/if}} {{Name}}; //{{Key.Description}}
};
{{/each}}

class PhantasmaAPI
{
public:
	PhantasmaAPI(std::string host);
	~PhantasmaAPI();

	{{#each methods}}
	//{{Info.Description}}
	{{Info.ReturnType.Name}} {{Info.Name}} ({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}})
};

