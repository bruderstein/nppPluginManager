#include "DownloadManager.h"
#include "Common.h"

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

BOOL DownloadManager::getUrl(CONST TCHAR *url, tstring& filename, tstring& contentType)
{
#ifdef _UNICODE
	WcharMbcsConvertor *wcharConverter = WcharMbcsConvertor::getInstance();
	const char *charUrl = wcharConverter->wchar2char(url, 65001);
	curl_easy_setopt(_curl, CURLOPT_URL, charUrl);
	delete[] charUrl;
#else
	curl_easy_setopt(_curl, CURLOPT_URL, url);
#endif
	FILE *fp = _tfopen(filename.c_str(), _T("wb"));
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, DownloadManager::curlWriteCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &contentType);
	curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, DownloadManager::curlHeaderCallback);
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

size_t DownloadManager::curlHeaderCallback(void *ptr, size_t size, size_t nmemb, void *vContentType)
{
	tstring *contentType = 	reinterpret_cast<tstring *>(vContentType);
	return size * nmemb;
}