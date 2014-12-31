#include "precompiled_headers.h"



#include "libinstall/DownloadManager.h"
#include "InternetDownload.h"
#include "libinstall/WcharMbcsConverter.h"
#include "libinstall/ModuleInfo.h" 
using namespace std;

tstring DownloadManager::_userAgent(_T("Plugin-Manager"));

DownloadManager::DownloadManager(void)
{
    _progressFunctionSet = FALSE;
}

DownloadManager::~DownloadManager(void)
{
}


void DownloadManager::setUserAgent(const TCHAR* userAgent)
{
    _userAgent = userAgent;
}

void DownloadManager::setProgressFunction(boost::function<void(int)> progressFunction)
{
    _progressFunction = progressFunction;
    _progressFunctionSet = TRUE;
}

BOOL DownloadManager::getUrl(CONST TCHAR *url, tstring& filename, tstring& contentType, const ModuleInfo *moduleInfo)
{
    InternetDownload download(_userAgent, url);
    contentType.append(_T("application/zip"));
    return download.saveToFile(filename);
}



BOOL DownloadManager::getUrl(CONST TCHAR *url, string& result, const ModuleInfo *moduleInfo)
{
    InternetDownload download(_userAgent, url);
    result.append( download.getContent());
    return !result.empty();
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