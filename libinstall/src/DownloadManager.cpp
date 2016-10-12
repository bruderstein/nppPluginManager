#include "precompiled_headers.h"



#include "libinstall/DownloadManager.h"
#include "InternetDownload.h"
#include "libinstall/WcharMbcsConverter.h"
#include "libinstall/ModuleInfo.h" 
using namespace std;

tstring DownloadManager::_userAgent(_T("Plugin-Manager"));

DownloadManager::DownloadManager(CancelToken& cancelToken)
    : m_cancelToken(cancelToken),
      m_disableCache(FALSE)
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

void DownloadManager::setProgressFunction(std::function<void(int)> progressFunction)
{
    _progressFunction = progressFunction;
    _progressFunctionSet = TRUE;
}

void DownloadManager::disableCache() {
    m_disableCache = TRUE;
}

BOOL DownloadManager::getUrl(CONST TCHAR *url, tstring& filename, tstring& contentType, const ModuleInfo *moduleInfo)
{
    InternetDownload download(moduleInfo->getHParent(), _userAgent, url, m_cancelToken, _progressFunction);
    if (m_disableCache) {
        download.disableCache();
    }

    BOOL downloadSuccess = download.saveToFile(filename);
    contentType.append(download.getContentType());

    return downloadSuccess;
}



BOOL DownloadManager::getUrl(CONST TCHAR *url, string& result, const ModuleInfo *moduleInfo)
{
    InternetDownload download(moduleInfo->getHParent(), _userAgent, url, m_cancelToken, _progressFunction);
    if (m_disableCache) {
        download.disableCache();
    }
    result.append( download.getContent());
    return !result.empty();
}

void DownloadManager::cancelDownload() 
{
    m_cancelToken.triggerCancel();
}


