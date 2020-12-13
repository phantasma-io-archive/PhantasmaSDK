#pragma once
#ifdef PHANTASMA_API_INCLUDED
#error "Include JSON API adaptors immediately before including PhantasmaAPI.h"
#endif 
#define PHANTASMA_RAPIDJSON
#ifdef PHANTASMA_CURL
#pragma message("Please include the libCurl adapter AFTER the RapidJSON adapter")
#endif
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with JSON features provided by the 
//  rapidjson library (http://rapidjson.org/) 
//------------------------------------------------------------------------------
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#ifndef PHANTASMA_STRING
# include <string>
# include <sstream>
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

namespace phantasma { 

struct RapidJsonBufferWriter 
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> w;
	RapidJsonBufferWriter() : w(buf) {}
};

namespace json {
	typedef PHANTASMA_STRING String;
	typedef PHANTASMA_CHAR   Char;
	inline const rapidjson::Value& Parse(const rapidjson::Document& d) { return d; }
	inline bool                    LookupBool(   const rapidjson::Value& v, const Char* field, bool& out_error) { return v[field].IsBool()   ? v[field].GetBool() : (out_error=true, false); }
	inline int32_t                 LookupInt32(  const rapidjson::Value& v, const Char* field, bool& out_error) { return v[field].IsString() ? ( int32_t)std::strtoll(v[field].GetString(), 0, 10) : (v[field].IsInt()  ? v[field].GetInt()  : (out_error=true, 0)); }
	inline uint32_t                LookupUInt32( const rapidjson::Value& v, const Char* field, bool& out_error) { return v[field].IsString() ? (uint32_t)std::strtoll(v[field].GetString(), 0, 10) : (v[field].IsUint() ? v[field].GetUint() : (out_error=true, 0)); }
	inline String                  LookupString( const rapidjson::Value& v, const Char* field, bool& out_error) { return (String)( v[field].IsString() ? v[field].GetString() : ""); }
	inline const rapidjson::Value& LookupValue(  const rapidjson::Value& v, const Char* field, bool& out_error) { return v[field]; }
	inline const rapidjson::Value& LookupArray(  const rapidjson::Value& v, const Char* field, bool& out_error) { return v[field]; }
	inline bool                    HasField(     const rapidjson::Value& v, const Char* field, bool& out_error) { return v.HasMember(field); }
	inline bool                    HasArrayField(const rapidjson::Value& v, const Char* field, bool& out_error) { return v[field].IsArray(); }
	inline bool                    AsBool(       const rapidjson::Value& v,                    bool& out_error) { return v.IsBool() ? v.GetBool() : (out_error=true, 0); }
	inline int32_t                 AsInt32(      const rapidjson::Value& v,                    bool& out_error) { return v.IsString() ? ( int32_t)std::strtoll(v.GetString(), 0, 10) : (v.IsInt() ? v.GetInt() : (out_error=true, 0)); }
	inline uint32_t                AsUInt32(     const rapidjson::Value& v,                    bool& out_error) { return v.IsString() ? (uint32_t)std::strtoll(v.GetString(), 0, 10) : (v.IsUint() ? v.GetUint() : (out_error=true, 0)); }
	inline String                  AsString(     const rapidjson::Value& v,                    bool& out_error) { return (String)(v.IsString() ? v.GetString() : ""); }
	inline const rapidjson::Value& AsArray(      const rapidjson::Value& v,                    bool& out_error) { return v; }
	inline bool                    IsArray(      const rapidjson::Value& v,                    bool& out_error) { return v.IsArray(); }
	inline bool                    IsObject(     const rapidjson::Value& v,                    bool& out_error) { return v.IsObject(); }

	inline int                     ArraySize( const rapidjson::Value& v,            bool& out_error) { return v.Size(); }
	inline const rapidjson::Value& IndexArray(const rapidjson::Value& v, int index, bool& out_error) { return v[index]; }
	
	typedef RapidJsonBufferWriter Builder;
	inline void BeginObject(Builder& b)                                   { b.w.StartObject(); }
	inline void EndObject(Builder& b)                                     { b.w.EndObject();}
	inline void AddString(Builder& b, const Char* key, const Char* value) { b.w.String(key); b.w.String(value); }
	inline void AddValues(Builder& ar)                                    {}
	inline void AddValues(Builder& b, const Char* arg)                    { b.w.String(arg); }
	inline void AddValues(Builder& b, int32_t arg)                        { b.w.Int(arg); }
	inline void AddValues(Builder& b, uint32_t arg)                       { b.w.Int(arg); }
	inline void AddValues(Builder& b, bool arg)                           { b.w.Bool(arg); }
	template<class T, class... Args> void AddValues(Builder& b, T arg0, Args... args) 
	{
		AddValues(b, arg0);
		AddValues(b, args...);
	}
	template<class... Args> void AddArray(Builder& b, const Char* key, Args... args)
	{
		b.w.String(key);
		b.w.StartArray();
		AddValues(b, args...);
		b.w.EndArray();
	}
}

typedef const rapidjson::Value&    RapidJsonValueRef;
#define PHANTASMA_JSONVALUE        RapidJsonValueRef
#define PHANTASMA_JSONARRAY        rapidjson::Value
#define PHANTASMA_JSONDOCUMENT     rapidjson::Document
#define PHANTASMA_JSONBUILDER      RapidJsonBufferWriter

}