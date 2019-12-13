#pragma once
#ifdef PHANTASMA_API_INCLUDED
#error "Include JSON/HTTP API adaptors immediately before including PhantasmaAPI.h"
#endif 
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with HTTP and JSON features provided
//  by the cpprest library (https://github.com/Microsoft/cpprestsdk).
//------------------------------------------------------------------------------

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cstdint>

namespace phantasma { 

#ifndef PHANTASMA_STRING
# ifdef _UNICODE
#  define PHANTASMA_STRING std::wstring
# else
#  define PHANTASMA_STRING std::string
# endif
#endif

#ifndef PHANTASMA_CHAR
# ifdef _UNICODE
#  define PHANTASMA_CHAR wchar_t
# else
#  define PHANTASMA_CHAR char
# endif
#endif

namespace json {
typedef PHANTASMA_CHAR Char;
typedef PHANTASMA_STRING String;
inline web::json::value Parse(const web::json::value& d) { return d; }

inline bool             LookupBool(   const web::json::value& v, const Char* field, bool& out_error) { return v.at(field).as_bool(); }
inline int32_t          LookupInt32(  const web::json::value& v, const Char* field, bool& out_error) { return v.at(field).as_integer(); }
inline uint32_t         LookupUInt32( const web::json::value& v, const Char* field, bool& out_error) { return (uint32_t)v.at(field).as_integer(); }
inline String           LookupString( const web::json::value& v, const Char* field, bool& out_error) { return v.at(field).as_string(); }
inline web::json::value LookupValue(  const web::json::value& v, const Char* field, bool& out_error) { return v.at(field); }
inline web::json::array LookupArray(  const web::json::value& v, const Char* field, bool& out_error) { return v.at(field).as_array(); }
inline bool             HasField(     const web::json::value& v, const Char* field, bool& out_error) { return v.has_field(field); }
inline bool             HasArrayField(const web::json::value& v, const Char* field, bool& out_error) { return v.has_array_field(field); }
inline bool             AsBool(       const web::json::value& v,                    bool& out_error) { return v.as_bool(); }
inline int32_t          AsInt32(      const web::json::value& v,                    bool& out_error) { return v.as_integer(); }
inline uint32_t         AsUInt32(     const web::json::value& v,                    bool& out_error) { return (uint32_t)v.as_integer(); }
inline String           AsString(     const web::json::value& v,                    bool& out_error) { return v.as_string(); }
inline web::json::array AsArray(      const web::json::value& v,                    bool& out_error) { return v.as_array(); }
inline bool             IsArray(      const web::json::value& v,                    bool& out_error) { return v.is_array(); }
inline bool             IsObject(     const web::json::value& v,                    bool& out_error) { return v.is_object(); }
inline int              ArraySize(    const web::json::array& a,                    bool& out_error) { return (int)a.size(); }
inline web::json::value IndexArray(   const web::json::array& a, int index,         bool& out_error) { return a.at((size_t)index); }

inline void BeginObject(web::json::value&) {}
inline void EndObject(web::json::value&){}
inline void AddString(web::json::value& root, const Char* key, const Char* value)       { root[key] = web::json::value::string(value); }
inline void AddValues(int idx, web::json::value& ar)                                    {}
inline void AddValues(int idx, web::json::value& ar, const Char* arg)                   { ar[idx] = web::json::value::string(arg); }
template<class T> void AddValues(int idx, web::json::value& ar, T arg)                  { ar[idx] = web::json::value::number(arg); }
template<class T, class... Args> void AddValues(int idx, web::json::value& ar, T arg0, Args... args) 
{
	AddValues(idx, ar, arg0);
	AddValues(idx+1, ar, args...);
}
template<class... Args> void AddArray(web::json::value& root, const Char* key, Args... args)
{
	web::json::value& ar = (root[key] = web::json::value::array());
	AddValues(0, ar, args...);
}
}

namespace rpc {
struct PhantasmaError;
void OnHttpError(PhantasmaError&, const PHANTASMA_CHAR*);
}
inline web::json::value HttpPost(web::http::client::http_client& client, const json::Char* uri, const web::json::value& data, rpc::PhantasmaError* err)
{
	web::uri_builder builder(uri);
	return client.request(web::http::methods::POST, builder.to_string(), data)
		.then([&](web::http::http_response response) -> web::json::value
	{
		const auto statusCode = response.status_code();
		if (statusCode != 200)
		{
			utility::stringstream_t msg;
			msg << U("Malformed RPC request or endpoint: response status = ");
			msg << statusCode;
			const auto& str = msg.str();
			if(err)
				rpc::OnHttpError(*err, str.c_str());
			throw web::http::http_exception(str);
		}
		return response.content_ready().get().extract_json(true).get();
	}).get();
}

#ifndef  PHANTASMA_EXCEPTION_ENABLE
# define PHANTASMA_EXCEPTION_ENABLE
#endif

#define PHANTASMA_JSONVALUE     web::json::value
#define PHANTASMA_JSONARRAY     web::json::array
#define PHANTASMA_JSONDOCUMENT  web::json::value
#define PHANTASMA_JSONBUILDER   web::json::value
#define PHANTASMA_HTTPCLIENT    web::http::client::http_client

}
