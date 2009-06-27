#include "DownloadManager.h"
#include "Common.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "WcharMbcsConverter.h"
#include "curl/curl.h"

using namespace std;
using namespace boost;


DownloadManager::DownloadManager(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_curl = curl_easy_init();
	curl_easy_setopt(_curl, CURLOPT_USERAGENT, "Notepad++/Plugin-Manager");
	_progressFunctionSet = FALSE;
}

DownloadManager::~DownloadManager(void)
{
	curl_easy_cleanup(_curl);
}

void DownloadManager::setProgressFunction(function<void(int)> progressFunction)
{
	_progressFunction = progressFunction;
	_progressFunctionSet = TRUE;
}

BOOL DownloadManager::getUrl(CONST TCHAR *url, tstring& filename, tstring& contentType)
{
	shared_ptr<char> charUrl = WcharMbcsConverter::tchar2char(url);
	curl_easy_setopt(_curl, CURLOPT_URL, charUrl.get());

	FILE *fp = _tfopen(filename.c_str(), _T("wb"));
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, DownloadManager::curlWriteCallback);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, DownloadManager::curlProgressCallback);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
	curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0);
	CURLcode code = curl_easy_perform(_curl);
	
	// Get the content type
	char *contentTypeBuffer;
	CURLcode contentTypeCode = curl_easy_getinfo(_curl, CURLINFO_CONTENT_TYPE, &contentTypeBuffer);
	shared_ptr<TCHAR> tContentTypeBuffer = WcharMbcsConverter::char2tchar(contentTypeBuffer);
	contentType = tContentTypeBuffer.get();
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

int DownloadManager::curlProgressCallback(void *ptr, double dltotal, double dlnow, 
										  double /*ultotal*/, double /*ulnow*/)
{
	DownloadManager* mgr = reinterpret_cast<DownloadManager*>(ptr);
	if(mgr->_progressFunctionSet)
	{
		int percentage = (int)((dlnow / dltotal) * 100);
		mgr->_progressFunction(percentage);
	}

	return 0;
}

size_t DownloadManager::curlHeaderCallback(void *ptr, size_t size, size_t nmemb, void *vContentType)
{
	tstring *contentType = 	reinterpret_cast<tstring *>(vContentType);
	return size * nmemb;
}