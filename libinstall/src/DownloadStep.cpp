#include <string>
#include <fstream>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "DownloadStep.h"
#include "DownloadManager.h"
#include "Decompress.h"
#include "tstring.h"
#include "Common.h"
#include "WcharMbcsConverter.h"
#include "DirectLinkSearch.h"

using namespace std;
using namespace boost;

DownloadStep::DownloadStep(const TCHAR *url, const TCHAR *filename)
{
	_url = url;

	if (filename)
		_filename = filename;
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
		shared_ptr<char> cFilename = WcharMbcsConverter::tchar2char(_filename.c_str());
		shared_ptr<char> cDownloadFilename = WcharMbcsConverter::tchar2char(downloadFilename.c_str());
	    DirectLinkSearch linkSearch(cDownloadFilename.get());
		shared_ptr<char> realLink = linkSearch.search(cFilename.get());
		

		if (realLink.get())
		{
			shared_ptr<TCHAR> tRealLink = WcharMbcsConverter::char2tchar(realLink.get());
			_url = tRealLink.get();
			return Perform(basePath);
		}
	}
	
	return TRUE;
}