#pragma once

#include <TCHAR.H>
#include "curl/curl.h"
#include "tstring.h"
#include <boost/function.hpp>

class DownloadManager
{
public:
	DownloadManager(void);
	~DownloadManager(void);
	BOOL getUrl(const TCHAR *url, tstring& filename, tstring& contentType);
	void setProgressFunction(boost::function<void(int)> progressFunction);


	static size_t curlWriteCallback(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t curlHeaderCallback(void *ptr, size_t size, size_t nmemb, void *stream);
	static int DownloadManager::curlProgressCallback(void *ptr, double dltotal, double dlnow, 
										  double /*ultotal*/, double /*ulnow*/);

private:
	CURL	*_curl;
	boost::function<void(int)> _progressFunction;
	BOOL					   _progressFunctionSet;
};
