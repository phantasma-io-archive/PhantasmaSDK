#pragma once
#ifdef PHANTASMA_API_INCLUDED
#error "Include HTTP API adaptors immediately before including PhantasmaAPI.h"
#endif 
#define PHANTASMA_CURL
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with HTTP features provided by the 
//  CURL library (https://curl.haxx.se/)
// 
// Include this file AFTER PhantasmaAPI_rapidjson.h to interop libCurl and RapidJSON.
//------------------------------------------------------------------------------
#include "curl/curl.h"
#include "curl/easy.h"

#ifndef PHANTASMA_STRING
# include <string>
# ifdef _UNICODE
#  define PHANTASMA_STRING std::wstring
# else
#  define PHANTASMA_STRING std::string
# endif
#endif

#ifndef PHANTASMA_STRINGBUILDER
# include <sstream>
# ifdef _UNICODE
#  define PHANTASMA_STRINGBUILDER std::wstringstream
# else
#  define PHANTASMA_STRINGBUILDER std::stringstream
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
namespace rpc { 
struct PhantasmaError;
}

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
	char* append(const void* data, size_t dataSize)
	{
		size_t newCapacity = size + dataSize;
		if (newCapacity > capactiy)
			memory = (char*)realloc(memory, capactiy = newCapacity);
		char* dst = end();
		if( data )
			memcpy(dst, data, dataSize);
		size += dataSize;
		return dst;
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
	const PHANTASMA_STRING host;
	ReallocBuffer result;
#ifdef PHANTASMA_RAPIDJSON
	rapidjson::Document doc;
#endif

	CurlClient(const PHANTASMA_STRING& host = "http://localhost:7077")
		: host(host)
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
		PHANTASMA_STRING fullUrl = host + url;
		curl_easy_setopt(m_curl, CURLOPT_URL, fullUrl.c_str());
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)&result);
		curl_easy_setopt(m_curl, CURLOPT_POST, 1);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, dataLen);
		CURLcode code = curl_easy_perform(m_curl);
		return code;
	}
	CURLcode Get(const char* url)
	{
		result.clear();
		PHANTASMA_STRING fullUrl = host + url;
		curl_easy_setopt(m_curl, CURLOPT_URL, fullUrl.c_str());
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)&result);
		curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1);
		CURLcode code = curl_easy_perform(m_curl);
		return code;
	}
};

#ifdef PHANTASMA_RAPIDJSON
namespace rpc {
struct PhantasmaError;
void OnHttpError(PhantasmaError&, const char*);
}

template<class CurlClient>
static rapidjson::Document& HttpPost(CurlClient& client, const json::Char* uri, const RapidJsonBufferWriter& data, rpc::PhantasmaError* err)
{
	const char* request = data.buf.GetString();
	CURLcode code = client.Post(request, strlen(request), uri);
	client.result.append("\0", 1);
	if(err && code != CURLE_OK)
		rpc::OnHttpError(*err, curl_easy_strerror(code));
	return client.doc.ParseInsitu<0>(client.result.begin());
}
#else
template<class CurlClient>
static PHANTASMA_STRING HttpPost(CurlClient& client, const PHANTASMA_CHAR* uri, const PHANTASMA_STRINGBUILDER& data, rpc::PhantasmaError* err)
{
	const PHANTASMA_STRING& request = data.str();
	CURLcode code = client.Post(request.c_str(), request.length(), uri);
	client.result.append("\0", 1);
	if(err && code != CURLE_OK)
		rpc::OnHttpError(*err, curl_easy_strerror(code));
	return { client.result.begin() };
}
#endif

#define PHANTASMA_HTTPCLIENT       CurlClient

}
