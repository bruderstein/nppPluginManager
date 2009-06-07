#include <string>
#include "DownloadStep.h"
#include "DownloadManager.h"

DownloadStep::DownloadStep(const char *url)
{
	_url = url;
}

BOOL DownloadStep::Perform(tstring &basePath)
{
	DownloadManager downloadManager;

	tstring fullPath = basePath;
	
	TCHAR tempPath[MAX_PATH];
	::GetTempPath(MAX_PATH, tempPath);

	TCHAR downloadFilename[MAX_PATH];
	::GetTempFileName(basePath.c_str(), _T("download"), 0, downloadFilename);

	// need to get content type, if html we can search
	downloadManager.getUrl(_url.c_str(), downloadFilename);
	if (_url.substr(_url.size() - 4, 4) == ".zip")
	{
		// Unzip file into basePath

	}
	
	return TRUE;
}