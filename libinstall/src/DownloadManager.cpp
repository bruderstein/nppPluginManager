#include "precompiled_headers.h"



#include "libinstall/DownloadManager.h"
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
    HINTERNET hInternet = ::InternetOpen(_userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL /* proxy*/ , NULL /* proxy bypass */, 0/* INTERNET_FLAG_ASYNC */);


    FILE *fp;
    if (0 != _tfopen_s(&fp, filename.c_str(), _T("wb")))
    {
        // error opening file
        return FALSE;
    }

    long httpCode = 0;

    bool cancelClicked = false;

    HINTERNET hConnect = InternetConnect(hInternet, 
        _T("www.local"), 
        INTERNET_DEFAULT_HTTP_PORT, 
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        0, /* flags */
        reinterpret_cast<DWORD_PTR>(this));

    PCTSTR rgpszAcceptTypes[] = {_T("*/*"), NULL}; 

    HINTERNET hHttp = HttpOpenRequest(
        hConnect, 
        _T("GET"),
        _T("/test/cheese"),
        NULL, /* http version - defaults to "HTTP/1.1" */
        NULL, /* referrer - NULL means no referrer header sent */
        rgpszAcceptTypes, /* Accept types */
        0, /* Flags - none required */
        reinterpret_cast<DWORD_PTR>(this));
    
    BOOL sendRequestResult = HttpSendRequest(hHttp, 
        NULL, /*lpszHeaders */
        -1L, /* dwHeadersLength */
        NULL, /* lpOptional  (POST data) */
        0);    /* dwOptionalLength */


    // HttpEndRequest(hHttp, NULL, 0, NULL);

    DWORD bytesAvailable;
    BOOL dataAvailableResponse = InternetQueryDataAvailable(hHttp, &bytesAvailable, 0, NULL);
    if (bytesAvailable == 0) {
        bytesAvailable = 1;
    }

    if (dataAvailableResponse && bytesAvailable) {
        BYTE buffer[4096];
        DWORD bytesRead;
        DWORD bytesToRead = bytesAvailable;
        while (bytesToRead > 0) {
            if (bytesToRead > 4095) {
                bytesToRead = 4095;
            }
            InternetReadFile(hHttp, buffer, bytesToRead, &bytesRead);
            fwrite(buffer, bytesRead, 1, fp);
            bytesAvailable -= bytesRead;
            bytesToRead = bytesAvailable;
        }
    }

    fclose(fp);
    InternetCloseHandle(hHttp);
    InternetCloseHandle(hInternet);
    return TRUE;
}



BOOL DownloadManager::getUrl(CONST TCHAR *url, string& result, const ModuleInfo *moduleInfo)
{
    result.append("000000000000111100000000000011100000000000011111");
        return TRUE;
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