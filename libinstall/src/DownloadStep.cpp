#include <string>
#include "DownloadStep.h"
#include "DownloadManager.h"
#include "Decompress.h"

DownloadStep::DownloadStep(const TCHAR *url)
{
	_url = url;
}

BOOL DownloadStep::Perform(tstring &basePath)
{
	DownloadManager downloadManager;

	tstring fullPath = basePath;
	
	TCHAR tempPath[MAX_PATH];
	::GetTempPath(MAX_PATH, tempPath);

	TCHAR tDownloadFilename[MAX_PATH];
	::GetTempFileName(basePath.c_str(), _T("download"), 0, tDownloadFilename);
	
	tstring downloadFilename = tDownloadFilename;

	// need to get content type, if html we can search for 
	tstring contentType;
	downloadManager.getUrl(_url.c_str(), downloadFilename, contentType);

	if (contentType == _T("application/zip"))
	{
		// Unzip file into basePath
		Decompress::unzip(downloadFilename, basePath);
		
	}
	else if (contentType == _T("text/html"))
	{
		// Read HTML and search for real filename, then re-perform
		
	}
	
	return TRUE;
}