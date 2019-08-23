#pragma once
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with HTTP and JSON features provided
//  by the cpprest library (https://github.com/Microsoft/cpprestsdk).
//------------------------------------------------------------------------------

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cstdint>
#include <locale>
#include <codecvt>

namespace phantasma { 
	namespace json {
		inline web::json::value Parse(const web::json::value& d) { return d; }

		inline bool             LookupBool(   const web::json::value& v, const wchar_t* field, bool& out_error) { return v.at(field).as_bool(); }
		inline int32_t          LookupInt32(  const web::json::value& v, const wchar_t* field, bool& out_error) { return v.at(field).as_integer(); }
		inline uint32_t         LookupUInt32( const web::json::value& v, const wchar_t* field, bool& out_error) { return (uint32_t)v.at(field).as_integer(); }
		inline std::wstring     LookupString( const web::json::value& v, const wchar_t* field, bool& out_error) { return v.at(field).as_string(); }
		inline web::json::value LookupValue(  const web::json::value& v, const wchar_t* field, bool& out_error) { return v.at(field); }
		inline web::json::array LookupArray(  const web::json::value& v, const wchar_t* field, bool& out_error) { return v.at(field).as_array(); }
		inline bool             HasField(     const web::json::value& v, const wchar_t* field, bool& out_error) { return v.has_field(field); }
		inline bool             HasArrayField(const web::json::value& v, const wchar_t* field, bool& out_error) { return v.has_array_field(field); }
		inline bool             AsBool(       const web::json::value& v,                       bool& out_error) { return v.as_bool(); }
		inline int32_t          AsInt32(      const web::json::value& v,                       bool& out_error) { return v.as_integer(); }
		inline uint32_t         AsUInt32(     const web::json::value& v,                       bool& out_error) { return (uint32_t)v.as_integer(); }
		inline std::wstring     AsString(     const web::json::value& v,                       bool& out_error) { return v.as_string(); }
		inline web::json::array AsArray(      const web::json::value& v,                       bool& out_error) { return v.as_array(); }
		inline bool             IsArray(      const web::json::value& v,                       bool& out_error) { return v.is_array(); }
		inline bool             IsObject(     const web::json::value& v,                       bool& out_error) { return v.is_object(); }
		inline int              ArraySize(    const web::json::array& a,                       bool& out_error) { return (int)a.size(); }
		inline web::json::value IndexArray(   const web::json::array& a, int index,            bool& out_error) { return a.at((size_t)index); }

		inline void BeginObject(web::json::value&) {}
		inline void EndObject(web::json::value&){}
		inline void AddString(web::json::value& root, const wchar_t* key, const wchar_t* value) { root[key] = web::json::value::string(value); }
		inline void AddValues(int idx, web::json::value& ar)                                    {}
		inline void AddValues(int idx, web::json::value& ar, const wchar_t* arg)                { ar[idx] = web::json::value::string(arg); }
		template<class T> void AddValues(int idx, web::json::value& ar, T arg)                  { ar[idx] = web::json::value::number(arg); }
		template<class T, class... Args> void AddValues(int idx, web::json::value& ar, T arg0, Args... args) 
		{
			AddValues(idx, ar, arg0);
			AddValues(idx+1, ar, args...);
		}
		template<class... Args> void AddArray(web::json::value& root, const wchar_t* key, Args... args)
		{
			web::json::value& ar = (root[key] = web::json::value::array());
			AddValues(0, ar, args...);
		}
	}

	inline web::json::value HttpPost(web::http::client::http_client& client, const wchar_t* uri, const web::json::value& data)
	{
		web::uri_builder builder(uri);
		return client.request(web::http::methods::POST, builder.to_string(), data)
			.then([&](web::http::http_response response) -> web::json::value
		{
			const auto statusCode = response.status_code();
			if (statusCode != 200)
				throw web::http::http_exception("Malformed RPC request or endpoint: response status = " + statusCode);
			return response.content_ready().get().extract_json(true).get();
		}).get();
	}
}

#define PHANTASMA_JSONVALUE                          web::json::value
#define PHANTASMA_JSONARRAY                          web::json::array
#define PHANTASMA_JSONDOCUMENT                       web::json::value
#define PHANTASMA_JSONBUILDER                        web::json::value
#define PHANTASMA_HTTPCLIENT                         web::http::client::http_client
#ifdef _UNICODE
#define PHANTASMA_CHAR                               wchar_t
#define PHANTASMA_LITERAL(x)                         L ## x
#define PHANTASMA_STRING                             std::wstring
#define PHANTASMA_EXCEPTION_MESSAGE(message, string) throw std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(string))
#else
#define PHANTASMA_EXCEPTION_MESSAGE(message, string) throw std::runtime_error(string)
#endif
#define PHANTASMA_EXCEPTION(message)                 throw std::runtime_error(message)
#include "PhantasmaAPI.h"
