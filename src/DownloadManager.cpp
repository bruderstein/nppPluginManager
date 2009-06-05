#include "DownloadManager.h"


#include "curl/curl.h"

DownloadManager::DownloadManager(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_curl = curl_easy_init();
	curl_easy_setopt(_curl, CURLOPT_USERAGENT, "Notepad++/Plugin-Manager");
}

DownloadManager::~DownloadManager(void)
{
	curl_easy_cleanup(_curl);
}

BOOL DownloadManager::getUrl(char *url, TCHAR *filename)
{
	curl_easy_setopt(_curl, CURLOPT_URL, url);
	FILE *fp = _tfopen(filename, _T("wb"));
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, DownloadManager::curlWriteCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, fp);
	CURLcode code = curl_easy_perform(_curl);
	
	fclose(fp);
	
	if (0 == code)
		return TRUE;
	else
		return FALSE;
}

size_t DownloadManager::curlWriteCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, reinterpret_cast<FILE*>(stream));
}