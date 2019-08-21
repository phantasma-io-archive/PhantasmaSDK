#pragma once
#ifndef PHANTASMA_API_INCLUDED
#define PHANTASMA_API_INCLUDED
//------------------------------------------------------------------------------
// Low-level API
//------------------------------------------------------------------------------
//  The PhantasmaJsonAPI namespace can construct JSON requests and parse JSON responses,
//   but you are responsible for sending/receiving these messages via HTTP on your own.
//   You can call PhantasmaJsonAPI::Uri() to determine where to send them.
//
// void PhantasmaJsonAPI::Make{Message}Request(JSONBuilder&, {Parameters});
// bool PhantasmaJsonAPI::Parse{Message}Response(const JSONValue&, {Output});
//
//------------------------------------------------------------------------------
// High-level API
//------------------------------------------------------------------------------
//  If you have defined PHANTASMA_HTTPCLIENT, then you can construct a PhantasmaAPI object,
//   which provides a simplified API that hides the internal JSON messaging.
//
// PhantasmaAPI phantasmaAPI(httpClient);
// {Output} phantasmaAPI->{Message}({Parameters});
//
//------------------------------------------------------------------------------
// API configuration
//------------------------------------------------------------------------------
// As different C++ projects may use different primitive types, you can use the 
//  following #defines (BEFORE including phantasma.h) to override the default types.
//
// #define                | typedef                 | Default           | Notes
// PHANTASMA_BYTE         | phantasma::Byte         | uint8_t           |
// PHANTASMA_INT32        | phantasma::Int32        | int32_t           |
// PHANTASMA_UINT32       | phantasma::UInt32       | uint32_t          |
// PHANTASMA_INT64        | phantasma::Int64        | int64_t           |
// PHANTASMA_UINT64       | phantasma::UInt64       | uint64_t          |
// PHANTASMA_CHAR         | phantasma::Char         | char              | See Unicode section
// PHANTASMA_STRING       | phantasma::String       | std::string       | Must support construction from `const phantasma::Char*`
// PHANTASMA_STRINGBUILDER| phantasma::StringBuilder| std::stringstream | 
// PHANTASMA_VECTOR       |                         | std::vector       | Must support `push_back` and `size` members
// PHANTASMA_JSONVALUE    | phantasma::JSONValue    | std::string_view  | See JSON and Adaptors section
// PHANTASMA_JSONARRAY    | phantasma::JSONArray    | JSONValue         | See JSON and Adaptors section
// PHANTASMA_JSONDOCUMENT | phantasma::JSONDocument | std::string       | See JSON and Adaptors section
// PHANTASMA_JSONBUILDER  | phantasma::JSONBuilder  | std::stringstream*| See JSON and Adaptors section
// PHANTASMA_HTTPCLIENT   | phantasma::HttpClient   |                   | See HTTP and Adaptors section
//
// The behavior of this header can further be modified by using the following 
//  #defines (BEFORE including phantasma.h)
// 
// #define                                      | Notes                   
// PHANTASMA_EXCEPTION(message)                 | See Exceptions section
// PHANTASMA_EXCEPTION_MESSAGE(message, String) | See Exceptions section
// PHANTASMA_LITERAL(x)                         | See Unicode section
// PHANTASMA_FUNCTION                           | See Integration section         
// PHANTASMA_IMPLEMENTATION                     | See Integration section
//
//------------------------------------------------------------------------------
// Integration
//------------------------------------------------------------------------------
// The core of API is provided in the "single header" style to support simple and 
//  flexible integration into your project 
//  (see https://github.com/nothings/single_file_libs).
// The implementation of function bodies will be excluded unless you define
//  PHANTASMA_IMPLEMENTATION before including phantasma.h.
//
// See the "Extended/Advanced usage" section, below for details on what is excluded
//  from this single header file.
//
// Typical linking:
//  In one CPP file, before including phantasma.h:
//   #define PHANTASMA_IMPLEMENTATION
// 
// Inline linking:
//  In every CPP file that uses the API, before including phantasma.h:
//   #define PHANTASMA_IMPLEMENTATION
//   #define PHANTASMA_FUNCTION inline
//
// Aside from PHANTASMA_IMPLEMENTATION / PHANTASMA_FUNCTION, you should take care
//  to ensure that every other PHANTASMA_* macro is defined to the same value in
//  all of yoru CPP files that use the phantasma API.
//
//------------------------------------------------------------------------------
// Exceptions
//------------------------------------------------------------------------------
// Support for C++ exceptions is opt-in. Before including phantasma.h, define
//  the following to enable exceptions:
//
// #define PHANTASMA_EXCEPTION_ENABLE
//
// Alternatively, you can customize the exact type that is thrown by defining:
//
// #define PHANTASMA_EXCEPTION(message)                 throw std::runtime_error(message)
// #define PHANTASMA_EXCEPTION_MESSAGE(message, string) throw std::runtime_error(string)
//
//------------------------------------------------------------------------------
// Unicode
//------------------------------------------------------------------------------
// To build a wide-character version of the API, define the following before
//  including phantasma.h:
//
// #define PHANTASMA_CHAR          wchar_t
// #define PHANTASMA_LITERAL(x)    L ## x
// #define PHANTASMA_STRING        std::wstring
// #define PHANTASMA_STRINGBUILDER std::wstringstream
//
// Alternatively, if '_UNICODE' is defined, then the above macros will be defined
//  automatically.
//
// You should also provide a JSON and HTTP library with wide-character support.
//
//------------------------------------------------------------------------------
// Adaptors
//------------------------------------------------------------------------------
// Parts of the Phantasma SDK are designed to plug into external features, such
//  as HTTP communications, JSON encoding and advanced cryptography.
// You can configure the SDK to connect to your own implemenations, or existing
//  libraries.
// To make integration easier, we provide several "adaptor" header files that 
//  contain the required configuration to connect the Phantasma SDK to existing
//  popular open source libraries for different features:
//
// Library   | Features     | #include file                     | Library URL
// C++ REST  | HTTP + JSON  | Adapters/PhantasmaAPI_cpprest.h   | https://github.com/microsoft/cpprestsdk 
// libcurl   | HTTP         | Adapters/PhantasmaAPI_curl.h      | https://curl.haxx.se/libcurl/        
// RapidJSON | JSON         | Adapters/PhantasmaAPI_rapidjson.h | http://rapidjson.org/ 
// Sodium    | Cryptography | Adapters/PhantasmaAPI_sodium.h    | https://libsodium.org
//
//------------------------------------------------------------------------------
// JSON
//------------------------------------------------------------------------------
// This header contains JSON parsing and building code, but it is written to be
//  as simple as possible (approx 200 lines of code) and is not high-performance
//  or highly robust.
//
// The CPP REST and RapidJSON adaptors implement these macros.
//
// It is recommended that you supply another JSON-parsing API, by defining the
//  following macros before including phantasma.h:
// #define PHANTASMA_JSONVALUE    Your_Json_Value_Type
// #define PHANTASMA_JSONARRAY    Your_Json_Array_Type
// #define PHANTASMA_JSONDOCUMENT Your_JSON_Document_Type
// #define PHANTASMA_JSONBUILDER  Your_Json_Serializer_Type
//
// Also, this header uses the following procedural API to interact with these types.
// If you have supplied your own JSON types, you must implement the following functions:
//
// namespace phantasma { namespace json {
//
//    JSONValue Parse(const JSONDocument&);
//
//    bool      LookupBool(   const JSONValue&, const Char* field, bool& out_error);
//    Int32     LookupInt32(  const JSONValue&, const Char* field, bool& out_error);
//    UInt32    LookupUInt32( const JSONValue&, const Char* field, bool& out_error);
//    String    LookupString( const JSONValue&, const Char* field, bool& out_error);
//    JSONValue LookupValue(  const JSONValue&, const Char* field, bool& out_error);
//    JSONArray LookupArray(  const JSONValue&, const Char* field, bool& out_error);
//    bool      HasField(     const JSONValue&, const Char* field, bool& out_error);
//    bool      HasArrayField(const JSONValue&, const Char* field, bool& out_error);
//
//    bool      AsBool(       const JSONValue&,                    bool& out_error);
//    Int32     AsInt32(      const JSONValue&,                    bool& out_error);
//    UInt32    AsUInt32(     const JSONValue&,                    bool& out_error);
//    String    AsString(     const JSONValue&,                    bool& out_error);
//    JSONArray AsArray(      const JSONValue&,                    bool& out_error);
//    bool      IsArray(      const JSONValue&,                    bool& out_error);
//    bool      IsObject(     const JSONValue&,                    bool& out_error);
//    
//    int       ArraySize(    const JSONArray&,                    bool& out_error);
//    JSONValue IndexArray(   const JSONArray&, int index,         bool& out_error);
//
//                           void BeginObject(JSONBuilder&);
//                           void AddString  (JSONBuilder&, const Char* key, const Char* value);
//   template<class... Args> void AddArray   (JSONBuilder&, const Char* key, Args...);
//                           void EndObject  (JSONBuilder&);
// }}
//
//------------------------------------------------------------------------------
// HTTP
//------------------------------------------------------------------------------
// This header does not contain a HTTP client, nor a dependency on any specific
//  HTTP client library. If you do not supply a HTTP client library, then only
//  the Low-level phantasma API (PhantasmaJsonAPI) is available.
// The CPP REST and libcurl adaptors implement these macros.
//
// To enable the PhantasmaAPI class, defining the following macro before 
//  including phantasma.h:
// #define PHANTASMA_HTTPCLIENT   Your_HTTP_Client_Type
//
// Also, this header uses the following procedural API to interact with this type.
// If you have defined PHANTASMA_HTTPCLIENT, you must implement the following,
//  function, which should perform a HTTP POST request and return the result:
//
// namespace phantasma {
//  JSONDocument HttpPost(HttpClient&, const Char* uri, const JSONBuilder&);
// }
//
//------------------------------------------------------------------------------
// Extended/Advanced usage
//------------------------------------------------------------------------------
// This header file contains the entirety of the RPC API requried to communicate 
//  with a Phantasma node. If you are not trying to create and transactions, this 
//  may be enough for you.
// However, for advanced usage, such as creating and signing transactions, much
//  more code is required, including cryptography, N-bit ingeger arithmetic, etc.
// The other header files that are included in this distribution, in sub-folders
//  provide these extra features, listed below:
//
//  Directory     | Features
//   Adapters     | Configuration for this library to communicate with 3rd party libraries
//   Blockchain   | Transactions
//   Cryptography | Public/Private keys, Signatures, Random numbers, Encryption
//   Numerics     | N-bit integer implementation. Base58 ASCII encoding.
//   Security     | Practical memory protection.
//
//------------------------------------------------------------------------------
// - Extended/Advanced usage - Security configuration
//------------------------------------------------------------------------------
//   To securely process transactions and private keys, it is strongly advised to 
//   pair the PhantasmaAPI with strong 3rd party security library.
//   The libSodium adaptor implements these macros.
//   
//   #define                    | 
//   PHANTASMA_RANDOMBYTES      | Fill a memory range with cryptographically secure pseudo-random numbers
//   PHANTASMA_WIPEMEM          | Fill a memory range with 0's in a way that won't be "optimized away"
//                              | 
//   PHANTASMA_LOCKMEM          | Pin the memory pages containing this range, and otherwise inform the OS that it contains secrets.
//   PHANTASMA_UNLOCKMEM        | Undo the actions of PHANTASMA_LOCKMEM, but also fill the memory range with 0's as with PHANTASMA_WIPEMEM.
//                              | 
//   PHANTASMA_SECURE_ALLOC     | Similar to malloc, but should return dedicated pages that can have their access permissions modified.
//   PHANTASMA_SECURE_FREE      | Similar to free - used with allocations returned from PHANTASMA_SECURE_ALLOC
//   PHANTASMA_SECURE_NOACCESS  | Used with allocations returned from PHANTASMA_SECURE_ALLOC. Mark the pages as non-readable.
//   PHANTASMA_SECURE_READONLY  | Used with allocations returned from PHANTASMA_SECURE_ALLOC. Mark the pages as read only.
//   PHANTASMA_SECURE_READWRITE | Used with allocations returned from PHANTASMA_SECURE_ALLOC. Mark the pages as writable.
//
//------------------------------------------------------------------------------
// - Extended/Advanced usage - Cryptography configuration
//------------------------------------------------------------------------------
//  To create or validate transactions, an EdDSA Ed25519 implementation is requied.
//   The libSodium adaptor implements these macros.
//   
//   #define                              | 
//   PHANTASMA_Ed25519_PublicKeyFromSeed  | Generate a 32 byte public key from a 32 byte seed.
//   PHANTASMA_Ed25519_PrivateKeyFromSeed | Generate a 64 byte public key from a 32 byte seed.
//   PHANTASMA_Ed25519_SignDetached       | Generate a 64 byte signature from a message and a private key.
//   PHANTASMA_Ed25519_ValidateDetached   | Validate a 64 byte signature using a public key.
//
//

#if !defined(PHANTASMA_STRING) || !defined(PHANTASMA_JSONDOCUMENT) || !defined(PHANTASMA_JSONVALUE)
# include <string>
#endif

#if !defined(PHANTASMA_JSONVALUE) && __cplusplus > 201402L
# include <string_view>
#endif

#if !defined(PHANTASMA_JSONBUILDER) || !defined(PHANTASMA_STRINGBUILDER)
# include <sstream>
#endif

#if !defined(PHANTASMA_VECTOR)
#define PHANTASMA_VECTOR std::vector
# include <vector>
#endif

#if !defined(PHANTASMA_S32) || !defined(PHANTASMA_U32)
# include <cstdint>
#endif

#if !defined(PHANTASMA_MAX) || !defined(PHANTASMA_COPY) || !defined(PHANTASMA_EQUAL) || !defined(PHANTASMA_SWAP)
# include <algorithm>
#endif

#if defined(_UNICODE) && !defined(PHANTASMA_EXCEPTION) && defined(PHANTASMA_EXCEPTION_ENABLE)
# include <locale>
# include <codecvt>
#endif

#ifndef PHANTASMA_MIN
# define PHANTASMA_MIN(a, b) std::min(a, b)
#endif

#ifndef PHANTASMA_MAX
# define PHANTASMA_MAX(a, b) std::max(a, b)
#endif

#ifndef PHANTASMA_SWAP
# define PHANTASMA_SWAP(a, b) std::swap(a, b)
#endif 

#ifndef PHANTASMA_COPY
# define PHANTASMA_COPY(src, src_end, dst) std::copy(src, src_end, dst)
#endif

#ifndef PHANTASMA_EQUAL
# define PHANTASMA_EQUAL(a, b, c) std::equal(a, b, c)
#endif

#if !defined(PHANTASMA_EXCEPTION)
# ifdef PHANTASMA_EXCEPTION_ENABLE
#  ifdef _UNICODE
#   define PHANTASMA_EXCEPTION(literal)                 throw std::runtime_error(literal)
#   define PHANTASMA_EXCEPTION_MESSAGE(literal, string) throw std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(string))
#  else
#   define PHANTASMA_EXCEPTION(literal)                 throw std::runtime_error(literal)
#   define PHANTASMA_EXCEPTION_MESSAGE(literal, string) throw std::runtime_error(string)
#  endif
# else
#   define PHANTASMA_EXCEPTION(literal)                
#   define PHANTASMA_EXCEPTION_MESSAGE(literal, string)
# endif
#endif

#if !defined(PHANTASMA_TRY)
# if !defined(PHANTASMA_EXCEPTION)
#  define PHANTASMA_TRY        if(true)
#  define PHANTASMA_CATCH( x ) else
# else
#  define PHANTASMA_TRY		  try
#  define PHANTASMA_CATCH( x )  catch(x)
# endif
#endif

#if !defined(PHANTASMA_LITERAL)
# ifdef _UNICODE
#  define PHANTASMA_LITERAL(x) L ## x
# else
#  define PHANTASMA_LITERAL(x) x
# endif
#endif

#if !defined(PHANTASMA_FUNCTION)
# define PHANTASMA_FUNCTION
#endif

namespace phantasma
{
#ifdef PHANTASMA_CHAR
typedef PHANTASMA_CHAR Char;
#else
# ifdef _UNICODE
typedef wchar_t Char;
# else
typedef char Char;
# endif
#endif

#ifdef PHANTASMA_BYTE
typedef PHANTASMA_BYTE Byte;
#else
typedef uint8_t Byte;
#endif

#ifdef PHANTASMA_INT32
typedef PHANTASMA_INT32 Int32;
#else
typedef int32_t Int32;
#endif

#ifdef PHANTASMA_UINT32
typedef PHANTASMA_UINT32 UInt32;
#else
typedef uint32_t UInt32;
#endif

#ifdef PHANTASMA_INT64
typedef PHANTASMA_INT64 Int64;
#else
typedef int64_t Int64;
#endif

#ifdef PHANTASMA_UINT64
typedef PHANTASMA_UINT64 UInt64;
#else
typedef uint64_t UInt64;
#endif

#ifdef PHANTASMA_STRING
typedef PHANTASMA_STRING String;
#else
# ifdef _UNICODE
typedef std::wstring String;
# else
typedef std::string String;
# endif
#endif

#ifdef PHANTASMA_STRINGBUILDER
typedef PHANTASMA_STRINGBUILDER StringBuilder;
#else
# ifdef _UNICODE
typedef std::wstringstream StringBuilder;
# else
typedef std::stringstream StringBuilder;
# endif
#endif

#ifdef PHANTASMA_JSONVALUE
typedef PHANTASMA_JSONVALUE JSONValue;
#elif __cplusplus > 201402L
# ifdef _UNICODE
typedef std::wstring_view JSONValue;
# else
typedef std::string_view JSONValue;
# endif
#else
typedef String JSONValue;
#endif

#ifdef PHANTASMA_JSONARRAY
typedef PHANTASMA_JSONARRAY JSONArray;
#else
typedef JSONValue JSONArray;
#endif

#ifdef PHANTASMA_JSONDOCUMENT
typedef PHANTASMA_JSONDOCUMENT JSONDocument;
#else
typedef String JSONDocument;
#endif


#ifdef PHANTASMA_JSONBUILDER
typedef PHANTASMA_JSONBUILDER JSONBuilder;
#else
struct JSONBuilder // A VERY simple json string builder. Highly recommended that you provide a real JSON library instead!
{
	StringBuilder s;
	bool empty = true;
	operator StringBuilder&() { return s; }
	void AddKey(const Char* key) { if(!empty) { s << ", "; } empty = false; s << '"' << key << "\": "; }
	void AddValues() {}
	void AddValues(const char* arg) { s << '"' << arg << '"'; }
	template<class T> void AddValues(T arg) { s << arg; }
	template<class T, class... Args> void AddValues(T arg0, Args... args) { AddValues(arg0); s << ", "; AddValues(args...); }
	
	void BeginObject() { s << "{"; }
	void AddString(const Char* key, const Char* value) { AddKey(key); s << '"' << value << '"'; }
	template<class... Args> void AddArray(const Char* key, Args... args) { AddKey(key); s << '['; AddValues(args...); s << ']'; }
	void EndObject() { s << "}"; }
};
#endif

#ifdef PHANTASMA_HTTPCLIENT
typedef PHANTASMA_HTTPCLIENT HttpClient;
//JSONDocument HttpPost(HttpClient&, const Char* uri, const JSONBuilder&);
#endif

//If providing a JSON library (highly recommended that you do!), then you must provide these functions yourself:
namespace json
{
#ifndef PHANTASMA_JSONBUILDER
    JSONValue Parse(const JSONDocument&);
	bool LookupBool(const JSONValue&, const Char* field, bool& out_error);
	Int32 LookupInt32(const JSONValue&, const Char* field, bool& out_error);
	UInt32 LookupUInt32(const JSONValue&, const Char* field, bool& out_error);
	String LookupString(const JSONValue&, const Char* field, bool& out_error);
	JSONValue LookupValue(const JSONValue&, const Char* field, bool& out_error);
	JSONArray LookupArray(const JSONValue&, const Char* field, bool& out_error);
	bool HasField(const JSONValue&, const Char* field, bool& out_error);
	bool HasArrayField(const JSONValue&, const Char* field, bool& out_error);
	bool   AsBool(const JSONValue&, bool& out_error);
	Int32  AsInt32(const JSONValue&, bool& out_error);
	UInt32 AsUInt32(const JSONValue&, bool& out_error);
	String AsString(const JSONValue&, bool& out_error);
	JSONArray AsArray(const JSONValue&, bool& out_error);
	bool IsArray(const JSONValue&, bool& out_error);
	bool IsObject(const JSONValue&, bool& out_error);

	int ArraySize(const JSONArray&, bool& out_error);
	JSONValue IndexArray(const JSONArray&, int index, bool& out_error);

	void BeginObject(JSONBuilder&);
	void AddString(JSONBuilder&, const Char* key, const Char* value);
	template<class... Args>
	void AddArray(JSONBuilder&, const Char* key, Args... args);
	void EndObject(JSONBuilder&);
#endif
}

{{#each types}}
struct {{#fix-type Key}}{{#parse-lines false}}{{#new-line}}
{
{{#new-line}}
{{#each Value}}
	{{#if FieldType.IsArray}}PHANTASMA_VECTOR<{{/if}}{{#fix-type FieldType.Name}}{{#if FieldType.IsArray}}>{{/if}} {{Name}};//{{Key.Description}}
{{#new-line}}
{{/each}}
{{#parse-lines true}}};
{{/each}}

class PhantasmaJsonAPI
{
public:
	static const Char* Uri() { return PHANTASMA_LITERAL("/rpc"); }

	{{#each methods}}// {{Info.Description}} {{#if Info.IsPaginated==true}}(paginated call){{/if}}
	static void Make{{Info.Name}}Request(JSONBuilder&{{#each Info.Parameters}}, {{#fix-ref Type.Name}} {{Name}}{{/each}});
	static bool Parse{{Info.Name}}Response(const JSONValue&, {{#if Info.ReturnType.IsArray}}PHANTASMA_VECTOR<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}}& out);
	{{/each}}

private:
	static JSONValue CheckResponse(JSONValue response, bool& out_error);
	{{#each types}}static {{#fix-type Key}} Deserialize{{#fix-type Key}}(const JSONValue& json, bool& out_error);
	{{/each}}

	static bool Deserializebool(const JSONValue& json, bool& out_error);
};

#if defined(PHANTASMA_HTTPCLIENT)
class PhantasmaAPI
{
public:
	PhantasmaAPI(HttpClient& client) // client must have a longer lifetime than this API object
		: m_httpClient(client)
	{}

	{{#each methods}}// {{Info.Description}} {{#if Info.IsPaginated==true}}(paginated call){{/if}}
	{{#if Info.ReturnType.IsArray}}PHANTASMA_VECTOR<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} {{Info.Name}}({{#each Info.Parameters}}{{#fix-ref Type.Name}} {{Name}}, {{/each}}bool* out_error = nullptr);
	{{/each}}
private:
	HttpClient& m_httpClient;
};
#endif
	
#if defined(PHANTASMA_IMPLEMENTATION)
PHANTASMA_FUNCTION bool PhantasmaJsonAPI::Deserializebool(const JSONValue& value, bool& err)
{
	return json::AsBool(value, err);
}

{{#each types}}
PHANTASMA_FUNCTION {{#fix-type Key}} PhantasmaJsonAPI::Deserialize{{#fix-type Key}}(const JSONValue& value, bool& err)
{ {{#parse-lines false}}
{{#each Value}}
{{#if FieldType.Name=='String[]'}}{{#parse-lines true}}
	PHANTASMA_VECTOR<{{#fix-type FieldType.Name}}> {{Name}}Vector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("{{Name}}"), err))
	{
		const JSONArray& {{Name}}JsonArray = json::LookupArray(value, PHANTASMA_LITERAL("{{Name}}"), err);
		int size = json::ArraySize({{Name}}JsonArray, err);
		{{Name}}Vector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			{{Name}}Vector.push_back(json::AsString(json::IndexArray({{Name}}JsonArray, i, err), err));
		}
	}{{#parse-lines false}}
{{#else}}
{{#if FieldType.Name contains '[]'}}{{#parse-lines true}}
	PHANTASMA_VECTOR<{{#fix-type FieldType.Name}}> {{Name}}Vector;
	if(json::HasArrayField(value, PHANTASMA_LITERAL("{{Name}}"), err))
	{
		const JSONArray& {{Name}}JsonArray = json::LookupArray(value, PHANTASMA_LITERAL("{{Name}}"), err);
		int size = json::ArraySize({{Name}}JsonArray, err);
		{{Name}}Vector.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			{{Name}}Vector.push_back(Deserialize{{#fix-type FieldType.Name}}(json::IndexArray({{Name}}JsonArray, i, err), err));
		}
	}{{#parse-lines false}}
{{/if}}
{{/if}}
{{/each}}
	{{#new-line}}
	return {{#fix-type Key}} { {{#new-line}}
{{#each Value}}
{{#if FieldType.Name contains '[]'}}
		{{Name}}Vector
{{#else}}
{{#if FieldType.Name=='bool'}}
		json::LookupBool(value, PHANTASMA_LITERAL("{{Name}}"), err)
{{#else}}
{{#if FieldType.Name=='UInt32'}}
		json::LookupUInt32(value, PHANTASMA_LITERAL("{{Name}}"), err)
{{#else}}
{{#if FieldType.Name=='Int32'}}
		json::LookupInt32(value, PHANTASMA_LITERAL("{{Name}}"), err)
{{#else}}
{{#if FieldType.Name=='String'}}
		json::LookupString(value, PHANTASMA_LITERAL("{{Name}}"), err)
{{#else}}
{{#if FieldType.Name=='IAPIResult'}}
		json::LookupValue(value, PHANTASMA_LITERAL("result"), err)
{{#else}}
		(err=true, "Variable type {{FieldType.Name}} isnt currently handled by the template system")
{{/if}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}{{#if !@last}}, {{/if}}{{#new-line}}
{{/each}}
	};{{#parse-lines true}}
}
{{/each}}


PHANTASMA_FUNCTION JSONValue PhantasmaJsonAPI::CheckResponse(JSONValue response, bool& out_error)
{
	if( !json::IsObject(response, out_error) )
	{
		PHANTASMA_EXCEPTION("Failed to parse JSON");
		out_error = true;
		return response;
	}
	if( json::HasField(response, PHANTASMA_LITERAL("error"), out_error) )
	{
		String msg = json::LookupString(response, PHANTASMA_LITERAL("error"), out_error);
		PHANTASMA_EXCEPTION_MESSAGE("Server returned error: %s", msg);
		out_error = true;
		return response;
	}
	if( !json::HasField(response, PHANTASMA_LITERAL("result"), out_error) )
	{
		PHANTASMA_EXCEPTION("Malformed response: No \"result\" node on the JSON body");
		out_error = true;
		return response;
	}
	return json::LookupValue(response, PHANTASMA_LITERAL("result"), out_error);
}

{{#each methods}}
// {{Info.Description}} {{#if Info.IsPaginated}}(Paginated){{/if}}
PHANTASMA_FUNCTION void PhantasmaJsonAPI::Make{{Info.Name}}Request(JSONBuilder& request{{#each Info.Parameters}}, {{#fix-ref Type.Name}} {{Name}}{{/each}})
{
	json::BeginObject(request);
	json::AddString(request, PHANTASMA_LITERAL("jsonrpc"), PHANTASMA_LITERAL("2.0"));
	json::AddString(request, PHANTASMA_LITERAL("method"), PHANTASMA_LITERAL("{{#camel-case Info.Name}}"));
	json::AddString(request, PHANTASMA_LITERAL("id"), PHANTASMA_LITERAL("1"));
	json::AddArray(request, PHANTASMA_LITERAL("params"){{#each Info.Parameters}}, {{Name}}{{/each}});
	json::EndObject(request);{{#parse-lines true}}
}

PHANTASMA_FUNCTION bool PhantasmaJsonAPI::Parse{{Info.Name}}Response(const JSONValue& _jsonResponse, {{#if Info.ReturnType.IsArray}}PHANTASMA_VECTOR<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}}& output)
{
	bool err = false;
	JSONValue jsonResponse = PhantasmaJsonAPI::CheckResponse(_jsonResponse, err);
	if( err )
		return false;
	{{#if Info.IsPaginated}}Paginated pageStruct = DeserializePaginated(jsonResponse, err);
{{#parse-lines false}}
{{#if Info.ReturnType.IsArray}}
	if(!json::IsArray(pageStruct.result, err)){{#new-line}}
	{ {{#new-line}}
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");{{#new-line}}
		return false;{{#new-line}}
	} {{#new-line}}
	const JSONArray& pages = json::AsArray(pageStruct.result, err);{{#new-line}}
	int size = json::ArraySize(pages, err);{{#new-line}}
	output.reserve(size);{{#new-line}}
	for(int i = 0; i < size; ++i){{#new-line}}
	{
{{#new-line}}
{{#if Info.ReturnType.Name=='UInt32'}}
		output.push_back(json::AsUInt32(json::IndexArray(pages, i, err), err));{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='Int32'}}
		output.push_back(json::AsInt32(json::IndexArray(pages, i, err), err));{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
		output.push_back(json::AsString(json::IndexArray(pages, i, err), err));{{#new-line}}
{{#else}}
		output.push_back(Deserialize{{#fix-type Info.ReturnType.Name}}(json::IndexArray(pages, i, err), err));{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
	}{{#new-line}}
{{/if}}
{{#if Info.ReturnType.IsArray==false}}
	output = Deserialize{{#fix-type Info.ReturnType.Name}}(pageStruct.result, err);{{#new-line}}
{{/if}}
	return !err;{{#new-line}}
}
{{#parse-lines true}}{{/if}}{{#if Info.IsPaginated==false}}{{#parse-lines false}}
{{#if Info.ReturnType.IsArray}}
if (!json::IsArray(jsonResponse, err)){{#new-line}}
	{ {{#new-line}}
		PHANTASMA_EXCEPTION("Malformed response: No JSON array on the \"result\" node");{{#new-line}}
		return false;{{#new-line}}
	} {{#new-line}}
{{#new-line}}
	const JSONArray& resultArray = json::AsArray(jsonResponse, err);{{#new-line}}
	int resultArraySize = json::ArraySize(resultArray, err);{{#new-line}}
	output.reserve(resultArraySize);{{#new-line}}
	for(int i = 0; i < resultArraySize; ++i){{#new-line}}
	{
{{#new-line}}
{{#if Info.ReturnType.Name=='UInt32'}}
		output.push_back(json::AsUInt32(json::IndexArray(resultArray, i, err), err));{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='Int32'}}
		output.push_back(json::AsInt32(json::IndexArray(resultArray, i, err), err));{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
		output.push_back(json::AsString(json::IndexArray(resultArray, i, err), err));{{#new-line}}
{{#else}}
		output.push_back(Deserialize{{#fix-type Info.ReturnType.Name}}(json::IndexArray(resultArray, i, err), err));{{#new-line}}
		if( err ) return false;{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
	}{{#new-line}}
{{/if}}
{{#if Info.ReturnType.IsArray==false}}
{{#if Info.ReturnType.Name=='UInt32'}}
output = json::AsUInt32(jsonResponse, err);{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='Int32'}}
output = json::AsInt32(jsonResponse, err);{{#new-line}}
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
output = json::AsString(jsonResponse, err);{{#new-line}}
{{#else}}
output = Deserialize{{#fix-type Info.ReturnType.Name}}(jsonResponse, err);{{#new-line}}
		if( err ) return false;{{#new-line}}
{{/if}}
{{/if}}
{{/if}}
{{/if}}
	return !err;{{#new-line}}
}
{{#parse-lines true}}
{{/if}}
{{/each}}
	
#if defined(PHANTASMA_HTTPCLIENT)
{{#each methods}}
PHANTASMA_FUNCTION {{#if Info.ReturnType.IsArray}}PHANTASMA_VECTOR<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} PhantasmaAPI::{{Info.Name}}({{#each Info.Parameters}}{{#fix-ref Type.Name}} {{Name}}, {{/each}}bool* out_error)
{
	JSONBuilder request;
	PhantasmaJsonAPI::Make{{Info.Name}}Request(request{{#each Info.Parameters}}, {{Name}}{{/each}});
	const JSONDocument& response = HttpPost(m_httpClient, PhantasmaJsonAPI::Uri(), request);
	{{#if Info.ReturnType.IsArray}}PHANTASMA_VECTOR<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} output;
	bool success = PhantasmaJsonAPI::Parse{{Info.Name}}Response(json::Parse(response), output);
	if( !success && out_error )
		*out_error = true;
	return output;
}
{{/each}}
#endif

namespace json
{
#ifndef PHANTASMA_JSONVALUE
    JSONValue Parse(const JSONDocument& doc) { return doc; }

	inline size_t SkipNumber(const JSONValue& v, size_t i, bool& out_error)
	{
		size_t j = v.find_first_not_of("+-0123456789.eE", i);
		if( i==j ) { PHANTASMA_EXCEPTION("Invalid Number"); out_error = true; return i+1; }
		return j;
	}
	inline size_t SkipString(const JSONValue& v, size_t i, bool& out_error)
	{
		if( v[i] != '"' ) { PHANTASMA_EXCEPTION("Invalid String"); out_error = true; return i+1; }
		for(++i; i<v.size();)
		{
			i =  v.find_first_of("\"\\", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated String"); out_error = true; return i; }
			if( v[i] == '"' ) { break; }
			if( i+1 < v.size() )
			{
				switch(v[i+1])
				{
				case '\\': case 'b': case 'f': case 'n': case 'r': case 't': case '"': i = i+2; continue;
				case 'u': i = i+6; continue;
				}
			}
			PHANTASMA_EXCEPTION("Invalid Escape sequence"); out_error = true; ++i;
		}
		return i == JSONValue::npos ? i : i+1;
	}
	inline size_t SkipBoolNull(const JSONValue& v, size_t i, bool& out_error)
	{
		switch(v[i])
		{
		case 'f': if(v.compare(i, 5, "false")) { break; } return i+5;
		case 't': if(v.compare(i, 4, "true"))  { break; } return i+4;
		case 'n': if(v.compare(i, 4, "null"))  { break; } return i+4;
		}
		PHANTASMA_EXCEPTION("Invalid Value"); out_error = true; return i+1;
	}
	inline size_t SkipObject(const JSONValue& v, size_t i, bool& out_error);
	inline size_t SkipArray(const JSONValue& v, size_t i, bool& out_error)
	{
		if( v[i] != '[' ) { PHANTASMA_EXCEPTION("Invalid Array"); out_error = true; return i+1; }
		++i;
		for(; i<v.size();)
		{
			i = v.find_first_not_of(", \t\r\n\f\b", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated array"); out_error = true; return i; }
			switch( v[i] )
			{
			case ']': return i+1;
			case '{': i = SkipObject(v, i, out_error); break;
			case '[': i = SkipArray(v, i, out_error); break;
			case '"': i = SkipString(v, i, out_error); break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, i, out_error); break;
			}
		}
		PHANTASMA_EXCEPTION("Unterminated array");
		out_error = true;
		return i == JSONValue::npos ? i : i+1;
	}
	inline size_t SkipObject(const JSONValue& v, size_t i, bool& out_error)
	{
		if( v[i] != '{' ) { PHANTASMA_EXCEPTION("Invalid object"); out_error = true; return i+1; }
		for(; i<v.size();)
		{
			size_t keyBegin = v.find_first_of("\"}", i);
			if( keyBegin == JSONValue::npos || v[keyBegin] == '}' ) { break; }//no more keys
			size_t keyEnd = v.find_first_of('"', keyBegin+1);
			if( keyEnd == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated string"); out_error = true; break; }
			size_t valueBegin = v.find_first_not_of(" \t\r\n\f\b:", keyEnd+1);
			if( valueBegin == JSONValue::npos ) { PHANTASMA_EXCEPTION("No value following object key"); out_error = true; break; }
			char value0 = v[valueBegin];
			switch(value0)
			{
			case '{': i = SkipObject(v, valueBegin, out_error); break;
			case '[': i = SkipArray(v, valueBegin, out_error); break;
			case '"': i = SkipString(v, valueBegin, out_error); break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, valueBegin, out_error); break;
			}
		}
		return i == JSONValue::npos ? i : i+1;
	}

	PHANTASMA_FUNCTION bool LookupBool(const JSONValue& v, const Char* field, bool& out_error)       { return AsBool( LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION Int32 LookupInt32(const JSONValue& v, const Char* field, bool& out_error)     { return AsInt32( LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION UInt32 LookupUInt32(const JSONValue& v, const Char* field, bool& out_error)   { return AsUInt32(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION String LookupString(const JSONValue& v, const Char* field, bool& out_error)   { return AsString(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION JSONArray LookupArray(const JSONValue& v, const Char* field, bool& out_error) { return AsArray(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION bool HasField(const JSONValue& v, const Char* field, bool& out_error)         { return "" != LookupValue(v, field, out_error); }
	PHANTASMA_FUNCTION bool HasArrayField(const JSONValue& v, const Char* field, bool& out_error)    { return IsArray(LookupValue(v, field, out_error), out_error); }
	PHANTASMA_FUNCTION JSONValue LookupValue(const JSONValue& v, const Char* field, bool& out_error)
	{
		if( v.length() < 1 || v[0] != '{' ) { out_error = true; return ""; }
		size_t fieldLen = strlen(field);
		for(size_t i=0; i<v.size();)
		{
			size_t keyBegin = v.find_first_of("\"}", i);
			if( keyBegin == JSONValue::npos || v[keyBegin] == '}' ) { break; }//no more keys
			++keyBegin;
			size_t keyEnd = v.find_first_of('"', keyBegin);
			if( keyEnd == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated string"); out_error = true; break; }
			size_t valueBegin = v.find_first_not_of(" \t\r\n\f\b:", keyEnd+1);
			if( valueBegin == JSONValue::npos ) { PHANTASMA_EXCEPTION("No value following object key"); out_error = true; break; }
			size_t keyLen = keyEnd-keyBegin;
			bool correctKey = fieldLen == keyLen && 0==v.compare(keyBegin, keyLen, field);
			if( correctKey )
				return v.substr(valueBegin);//should really be using string views if this was a serious json parser...
			char value0 = v[valueBegin];
			switch(value0)
			{
			case '{': i = SkipObject(v, valueBegin, out_error); break;
			case '[': i = SkipArray( v, valueBegin, out_error); break;
			case '"': i = SkipString(v, valueBegin, out_error); break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, valueBegin, out_error); break;
			default:  i = SkipNumber(v, valueBegin, out_error); break;
			}
		}
		return "";
	}
	PHANTASMA_FUNCTION bool AsBool(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 ) { out_error = true; return false; }
		switch(v[0])
		{
		default: PHANTASMA_EXCEPTION("Casting non-bool value to bool"); out_error = true;
		case 'f': return false;
		case 't': return true;
		}
	}
	PHANTASMA_FUNCTION Int32  AsInt32(const JSONValue& v, bool& out_error)
	{
		const char* numeric = "-0123456789";
		size_t begin = v.find_first_of(numeric, 0);
		if( begin != 0 ) { PHANTASMA_EXCEPTION("Invalid number"); out_error = true; return 0; }
		size_t pos = v.find_first_not_of(numeric, 0);
		if( pos == 0 ) { PHANTASMA_EXCEPTION("Invalid number"); out_error = true; return 0; }
		JSONValue n = pos == JSONValue::npos ? v : v.substr(0, pos);
		return (Int32)std::strtol(n.data(), 0, 10);
	}
	PHANTASMA_FUNCTION UInt32 AsUInt32(const JSONValue& v, bool& out_error) { return (UInt32)AsInt32(v, out_error); }
	PHANTASMA_FUNCTION String AsString(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 || v[0] != '"' ) { PHANTASMA_EXCEPTION("Casting non-string value to string"); out_error = true; return String(""); }
		size_t pos = v.find('"', 1);
		if( pos == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated string"); out_error = true; return String(""); }
		return String(v.substr(1, pos-1));
	}
	PHANTASMA_FUNCTION JSONArray AsArray(const JSONValue& v, bool& out_error) { if(!IsArray(v, out_error)) { PHANTASMA_EXCEPTION("Casting non-array value to array"); out_error = true; } return v; }
	PHANTASMA_FUNCTION bool IsArray(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 ) { out_error = true; return false; }
		return v[0] == '[';
	}
	PHANTASMA_FUNCTION bool IsObject(const JSONValue& v, bool& out_error)
	{
		if( v.length() < 1 ) { out_error = true; return false; }
		return v[0] == '{';
	}

	PHANTASMA_FUNCTION int ArraySize(const JSONArray& v, bool& out_error)
	{
		if( v[0] != '[' ) { PHANTASMA_EXCEPTION("Invalid Array"); out_error = true; return 0; }
		int count = 0;
		for(size_t i=1; i<v.size();)
		{
			i = v.find_first_not_of(", \t\r\n\f\b", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated array"); out_error = true; return count; }
			switch( v[i] )
			{
			case ']': return count;
			case '{': i = SkipObject(v, i, out_error); count++; break;
			case '[': i = SkipArray(v, i, out_error);  count++; break;
			case '"': i = SkipString(v, i, out_error); count++; break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, i, out_error); count++; break;
			}
		}
		return count;
	}
	PHANTASMA_FUNCTION JSONValue IndexArray(const JSONArray& v, int index, bool& out_error)
	{
		if( v[0] != '[' ) { PHANTASMA_EXCEPTION("Invalid Array"); out_error = true; return ""; }
		int count = 0;
		for(size_t i=1; i<v.size();)
		{
			i = v.find_first_not_of(", \t\r\n\f\b", i);
			if( i == JSONValue::npos ) { PHANTASMA_EXCEPTION("Unterminated array"); out_error = true; return ""; }
			if( count == index )
				return v.substr(i);
			switch( v[i] )
			{
			case ']': goto err;
			case '{': i = SkipObject(v, i, out_error); count++; break;
			case '[': i = SkipArray(v, i, out_error);  count++; break;
			case '"': i = SkipString(v, i, out_error); count++; break;
			case 't': case 'f': case 'n': i = SkipBoolNull(v, i, out_error); break;
			default:  i = SkipNumber(v, i, out_error); count++; break;
			}
		}
	err:
		PHANTASMA_EXCEPTION("Array index out of bounds"); 
		out_error = true;
		return "";
	}
#endif
#ifndef PHANTASMA_JSONBUILDER
	PHANTASMA_FUNCTION void BeginObject(JSONBuilder& b)                                   { b.BeginObject(); }
	PHANTASMA_FUNCTION void AddString(JSONBuilder& b, const Char* key, const Char* value) { b.AddString(key, value); }
	template<class... Args>
	void AddArray(JSONBuilder& b, const Char* key, Args... args)                          { b.AddArray(key, args...); }
	PHANTASMA_FUNCTION void EndObject(JSONBuilder& b)                                     { b.EndObject(); }
#endif
}
#endif
}

#endif
