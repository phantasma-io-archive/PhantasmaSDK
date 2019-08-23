#pragma once
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with JSON features provided by the 
//  rapidjson library (http://rapidjson.org/) and HTTP features provided by the 
//  CURL library (https://curl.haxx.se/)
//------------------------------------------------------------------------------
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "curl/curl.h"
#include "curl/easy.h"

#ifndef PHANTASMA_STRING
#include <string>
#define PHANTASMA_STRING std::string
#endif

class ReallocBuffer // buffer class for CURL to write responses into
{
public:
	ReallocBuffer(size_t initialCapacity = 1024)
		: memory((char*)malloc(initialCapacity))
		, size(0), capactiy(initialCapacity) {}
	~ReallocBuffer() { free(memory); }
	void   clear() { size = 0; }
	char*  begin() { return memory; }
	char*  end() { return memory + size; }
	size_t bytes() const { return size; }
	size_t capacity() const { return capactiy; }
	void append(const void* data, size_t dataSize)
	{
		size_t newCapacity = size + dataSize;
		if (newCapacity > capactiy)
			memory = (char*)realloc(memory, capactiy = newCapacity);
		memcpy(end(), data, dataSize);
		size += dataSize;
	}
	static size_t CurlWrite(void *contents, size_t size, size_t nmemb, void *userp)
	{
		size_t realsize = size * nmemb;
		ReallocBuffer* mem = (ReallocBuffer*)userp;
		mem->append(contents, realsize);
		return realsize;
	}
private:
	char*  memory;
	size_t size;
	size_t capactiy;
};

class CurlClient // Very simple wrapper around CURL
{
	void* m_curl = 0;
public:
	rapidjson::Document doc;
	ReallocBuffer result;
	const std::string host = "http://localhost:7077";

	CurlClient()
	{
		m_curl = curl_easy_init();
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, ReallocBuffer::CurlWrite);
	}
	~CurlClient()
	{
		curl_easy_cleanup(m_curl);
	}
	CURLcode Post(const char* data, size_t dataLen, const char* url)
	{
		result.clear();
		std::string fullUrl = host + url;
		curl_easy_setopt(m_curl, CURLOPT_URL, fullUrl.c_str());
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)&result);
		curl_easy_setopt(m_curl, CURLOPT_POST, 1);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, dataLen);
		CURLcode result = curl_easy_perform(m_curl);
		return result;
	}
};

struct RapidJsonBufferWriter 
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> w;
	RapidJsonBufferWriter() : w(buf) {}
};

namespace phantasma { 
	namespace json {
		inline const rapidjson::Value& Parse(const rapidjson::Document& d) { return d; }
		inline bool                    LookupBool(   const rapidjson::Value& v, const char* field, bool& out_error) { return v[field].GetBool(); }
		inline int32_t                 LookupInt32(  const rapidjson::Value& v, const char* field, bool& out_error) { return v[field].GetInt(); }
		inline uint32_t                LookupUInt32( const rapidjson::Value& v, const char* field, bool& out_error) { return (uint32_t)v[field].GetInt(); }
		inline PHANTASMA_STRING        LookupString( const rapidjson::Value& v, const char* field, bool& out_error) { return (PHANTASMA_STRING)(v[field].GetString()); }
		inline const rapidjson::Value& LookupValue(  const rapidjson::Value& v, const char* field, bool& out_error) { return v[field]; }
		inline const rapidjson::Value& LookupArray(  const rapidjson::Value& v, const char* field, bool& out_error) { return v[field]; }
		inline bool                    HasField(     const rapidjson::Value& v, const char* field, bool& out_error) { return v.HasMember(field); }
		inline bool                    HasArrayField(const rapidjson::Value& v, const char* field, bool& out_error) { return v[field].IsArray(); }
		inline bool                    AsBool(       const rapidjson::Value& v,                    bool& out_error) { return v.GetBool(); }
		inline int32_t                 AsInt32(      const rapidjson::Value& v,                    bool& out_error) { return v.GetInt(); }
		inline uint32_t                AsUInt32(     const rapidjson::Value& v,                    bool& out_error) { return (uint32_t)v.GetInt(); }
		inline PHANTASMA_STRING        AsString(     const rapidjson::Value& v,                    bool& out_error) { return (PHANTASMA_STRING)(v.GetString()); }
		inline const rapidjson::Value& AsArray(      const rapidjson::Value& v,                    bool& out_error) { return v; }
		inline bool                    IsArray(      const rapidjson::Value& v,                    bool& out_error) { return v.IsArray(); }
		inline bool                    IsObject(     const rapidjson::Value& v,                    bool& out_error) { return v.IsObject(); }

		inline int                     ArraySize( const rapidjson::Value& v,            bool& out_error) { return v.Size(); }
		inline const rapidjson::Value& IndexArray(const rapidjson::Value& v, int index, bool& out_error) { return v[index]; }
	
		typedef RapidJsonBufferWriter Builder;
		inline void BeginObject(Builder& b)                                   { b.w.StartObject(); }
		inline void EndObject(Builder& b)                                     { b.w.EndObject();}
		inline void AddString(Builder& b, const char* key, const char* value) { b.w.String(key); b.w.String(value); }
		inline void AddValues(Builder& ar)                                    {}
		inline void AddValues(Builder& b, const char* arg)                    { b.w.String(arg); }
		inline void AddValues(Builder& b, int32_t arg)                        { b.w.Int(arg); }
		inline void AddValues(Builder& b, uint32_t arg)                       { b.w.Int(arg); }
		template<class T, class... Args> void AddValues(Builder& b, T arg0, Args... args) 
		{
			AddValues(b, arg0);
			AddValues(b, args...);
		}
		template<class... Args> void AddArray(Builder& b, const char* key, Args... args)
		{
			b.w.String(key);
			b.w.StartArray();
			AddValues(b, args...);
			b.w.EndArray();
		}
	}
	static rapidjson::Document& HttpPost(CurlClient& client, const char* uri, const RapidJsonBufferWriter& data)
	{
		std::string url = client.host + uri;
		const char* request = data.buf.GetString();
		client.Post(request, strlen(request), url.c_str());
		client.result.append("\0", 1);
		return client.doc.ParseInsitu<0>(client.result.begin());
	}
}

typedef const rapidjson::Value& RapidJsonValueRef;
#define PHANTASMA_JSONVALUE        RapidJsonValueRef
#define PHANTASMA_JSONARRAY        rapidjson::Value
#define PHANTASMA_JSONDOCUMENT     rapidjson::Document
#define PHANTASMA_JSONBUILDER      RapidJsonBufferWriter
#define PHANTASMA_HTTPCLIENT       CurlClient
#include "PhantasmaAPI.h"
