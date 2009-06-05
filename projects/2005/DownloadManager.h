#pragma once

#include <TCHAR.H>
#include "curl/curl.h"

class DownloadManager
{
public:
	DownloadManager(void);
	~DownloadManager(void);
	BOOL getUrl(TCHAR *url, TCHAR *filename);

private:
	CURL	*_curl;
};
