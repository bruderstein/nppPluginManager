#include "precompiled_headers.h"


#include <curl/curl.h>

#include "libinstall/DownloadManager.h"
#include "libinstall/WcharMbcsConverter.h"
#include "libinstall/ProxyInfo.h"
#include "libinstall/ProxyCredentialsDlg.h"
#include "libinstall/ModuleInfo.h" 
using namespace std;

string DownloadManager::_userAgent("Plugin-Manager");

DownloadManager::DownloadManager(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_curl = curl_easy_init();
	curl_easy_setopt(_curl, CURLOPT_USERAGENT, DownloadManager::_userAgent.c_str());
	_progressFunctionSet = FALSE;
}

DownloadManager::~DownloadManager(void)
{
	curl_easy_cleanup(_curl);
}


void DownloadManager::setUserAgent(const TCHAR* userAgent)
{
	std::tr1::shared_ptr<char> ua = WcharMbcsConverter::tchar2char(userAgent);
	_userAgent = ua.get();
}

void DownloadManager::setProgressFunction(boost::function<void(int)> progressFunction)
{
	_progressFunction = progressFunction;
	_progressFunctionSet = TRUE;
}

BOOL DownloadManager::getUrl(CONST TCHAR *url, tstring& filename, tstring& contentType, ProxyInfo *proxyInfo, const ModuleInfo *moduleInfo)
{
	std::tr1::shared_ptr<char> charUrl = WcharMbcsConverter::tchar2char(url);
	curl_easy_setopt(_curl, CURLOPT_URL, charUrl.get());

	FILE *fp;
	if (0 != _tfopen_s(&fp, filename.c_str(), _T("wb")))
	{
		// error opening file
		return FALSE;
	}

	long httpCode = 0;
	CURLcode code;
	bool cancelClicked = false;

	do
	{
		proxyInfo->setCurlOptions(_curl);
	
		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, DownloadManager::curlWriteCallback);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, DownloadManager::curlProgressCallback);
		curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1);
		code = curl_easy_perform(_curl);
	

		curl_easy_getinfo(_curl, CURLINFO_HTTP_CODE, &httpCode);
	
		if (httpCode == 407)
		{
			ProxyCredentialsDlg proxyCreds;
			if (!proxyCreds.getCredentials(moduleInfo, proxyInfo))
			{
				cancelClicked = true;
			}
		}

	} while(cancelClicked == false && (code == CURLE_LOGIN_DENIED || httpCode == 407));
	

	
	

	fclose(fp);
	
	if (cancelClicked == false && CURLE_OK == code)
	{
		// Get the content type
	
		char *contentTypeBuffer = NULL;
		curl_easy_getinfo(_curl, CURLINFO_CONTENT_TYPE, &contentTypeBuffer);
		if (contentTypeBuffer && *contentTypeBuffer)
		{
			std::tr1::shared_ptr<TCHAR> tContentTypeBuffer = WcharMbcsConverter::char2tchar(contentTypeBuffer);
		
			contentType = tContentTypeBuffer.get();
			tstring::size_type pos = contentType.find(_T(';'));
			if (pos != tstring::npos)
				contentType.erase(pos);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



BOOL DownloadManager::getUrl(CONST TCHAR *url, string& result, ProxyInfo *proxyInfo, const ModuleInfo *moduleInfo)
{
	std::tr1::shared_ptr<char> charUrl = WcharMbcsConverter::tchar2char(url);
	curl_easy_setopt(_curl, CURLOPT_URL, charUrl.get());
	CURLcode code;
	int httpCode = 0;
	bool cancelClicked = false;

	do
	{
		proxyInfo->setCurlOptions(_curl);

	
		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, DownloadManager::curlWriteStringCallback);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, DownloadManager::curlProgressCallback);
		curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1);
		code = curl_easy_perform(_curl);
	
		curl_easy_getinfo(_curl, CURLINFO_HTTP_CODE, &httpCode);
	
		if (httpCode == 407)
		{
			ProxyCredentialsDlg proxyCreds;
			if (!proxyCreds.getCredentials(moduleInfo, proxyInfo))
			{
				cancelClicked = true;
			}
		}
	
	} while(cancelClicked == false && (code == CURLE_LOGIN_DENIED || httpCode == 407));

	if (CURLE_OK == code && cancelClicked == false)
		return TRUE;
	else
		return FALSE;
}



size_t DownloadManager::curlWriteCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, reinterpret_cast<FILE*>(stream));
}


size_t DownloadManager::curlWriteStringCallback(void *ptr, size_t size, size_t nmemb, void *str)
{
	size_t pos = 0;
	size_t total = size * nmemb;
	char* tptr = (char*)ptr;

	// Don't allow result strings over 1k
	if (reinterpret_cast<string*>(str)->size() > 1024)
		return 0;

	while (pos < total)
	{
		reinterpret_cast<string*>(str)->push_back(*tptr);
		tptr++;
		++pos;
	}
	return total;
	
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

size_t DownloadManager::curlHeaderCallback(void * /*ptr*/, size_t size, size_t nmemb, void * /*vContentType*/)
{
//	tstring *contentType = 	reinterpret_cast<tstring *>(vContentType);
	return size * nmemb;
}