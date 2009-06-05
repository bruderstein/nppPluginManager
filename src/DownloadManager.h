#pragma once

#include <TCHAR.H>
#include "curl/curl.h"

class DownloadManager
{
public:
	DownloadManager(void);
	~DownloadManager(void);
	BOOL getUrl(char *url, TCHAR *filename);
	static size_t curlWriteCallback(void *ptr, size_t size, size_t nmemb, void *stream);

private:
	CURL	*_curl;
};
