#include "precompiled_headers.h"
#include "libinstall/DownloadStep.h"
#include "libinstall/DownloadManager.h"
#include "libinstall/Decompress.h"
#include "libinstall/tstring.h"
#include "libinstall/InstallStep.h"
#include "libinstall/WcharMbcsConverter.h"
#include "libinstall/DirectLinkSearch.h"
#include "libinstall/ProxyInfo.h"
#include "libinstall/ModuleInfo.h"

using namespace std;

DownloadStep::DownloadStep(const TCHAR *url, const TCHAR *filename)
{
    _url = url;

    if (filename)
        _filename = filename;
}

StepStatus DownloadStep::perform(tstring &basePath, TiXmlElement* forGpup,
                                 std::function<void(const TCHAR*)> setStatus,
                                 std::function<void(const int)> stepProgress,
                                 const ModuleInfo* moduleInfo,
                                 CancelToken &cancelToken)
{
    DownloadManager downloadManager(cancelToken);



    tstring downloadFilename;

    if (_filename.empty())
    {
        TCHAR tDownloadFilename[MAX_PATH];
        ::GetTempFileName(basePath.c_str(), _T("download"), 0, tDownloadFilename);
        downloadFilename = tDownloadFilename;
    }
    else
    {
        TCHAR tDownloadPath[MAX_PATH];
        PathCombine(tDownloadPath, basePath.c_str(), _filename.c_str());
        downloadFilename = tDownloadPath;
    }

    // Set the status
    tstring status = _T("Downloading ");
    status.append(_url);
    setStatus(status.c_str());

    // Link up the progress callback
    downloadManager.setProgressFunction(stepProgress);


    tstring contentType;

    if (downloadManager.getUrl(_url.c_str(), downloadFilename, contentType, moduleInfo))
    {
        if (contentType == _T("text/html"))
        {
            DirectLinkSearch linkSearch(downloadFilename.c_str());
            std::shared_ptr<TCHAR> realLink;
            if (!_filename.empty())
            {
                realLink = linkSearch.search(_filename.c_str());
            }
            else
            {
                tstring::size_type lastQuestionMark = _url.find_last_of(_T('?'));
                tstring::size_type lastSlash = _url.find_last_of(_T('/'));
                if (lastSlash != tstring::npos)
                {
                    lastSlash++;
                }

                tstring::size_type length = lastQuestionMark;
                if (length != tstring::npos)
                {
                    length = length - lastSlash;
                }


                tstring filenameInUrl = _url.substr(lastSlash, length);
                realLink = linkSearch.search(filenameInUrl.c_str());
            }

            if (realLink.get())
            {
                _url = realLink.get();
                return perform(basePath, forGpup, setStatus, stepProgress, moduleInfo, cancelToken);
            }
            else
                return STEPSTATUS_FAIL;
        } else {
            // Attempt to unzip file into basePath
            // Assume it is a zip file - if unzipping fails, then check if the filename is filled in
            // - if it is, then just leave the file as it is (ie. direct download)
            //   the file will be available for copying or installing.
            if (Decompress::unzip(downloadFilename, basePath) || !_filename.empty())
            {
                return STEPSTATUS_SUCCESS;
            }
        }
    }

    return STEPSTATUS_FAIL;
}