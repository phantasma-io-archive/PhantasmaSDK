#pragma once
#ifdef PHANTASMA_API_INCLUDED
#error "Include HTTP API adaptors immediately before including PhantasmaAPI.h"
#endif 
//------------------------------------------------------------------------------
// This header supplies the Phantasma API with HTTP features provided by the 
//  CURL library (https://curl.haxx.se/)
//------------------------------------------------------------------------------
#include "curl/curl.h"
#include "curl/easy.h"

namespace phantasma { 

#ifndef PHANTASMA_STRING
# ifdef _UNICODE
#  define PHANTASMA_STRING std::wstring
# else
#  define PHANTASMA_STRING std::string
# endif
#endif

#ifndef PHANTASMA_CHAR
#else
# ifdef _UNICODE
#  define PHANTASMA_CHAR wchar_t
# else
#  define PHANTASMA_CHAR char
# endif
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

#define PHANTASMA_HTTPCLIENT       CurlClient

}
