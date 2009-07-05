#include <string>
#include <fstream>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "DownloadStep.h"
#include "DownloadManager.h"
#include "Decompress.h"
#include "tstring.h"
#include "Common.h"
#include "InstallStep.h"
#include "WcharMbcsConverter.h"
#include "DirectLinkSearch.h"

using namespace std;
using namespace boost;

DownloadStep::DownloadStep(const TCHAR *url, const TCHAR *filename, const char* proxy, long proxyPort)
{
	_url = url;

	_proxy = proxy;
	_proxyPort = proxyPort;

	if (filename)
		_filename = filename;
}

StepStatus DownloadStep::perform(tstring &basePath, TiXmlElement* forGpup, 
								 boost::function<void(const TCHAR*)> setStatus,
								 boost::function<void(const int)> stepProgress)
{
	DownloadManager downloadManager;

	tstring fullPath = basePath;
	
	TCHAR tempPath[MAX_PATH];
	::GetTempPath(MAX_PATH, tempPath);

	TCHAR tDownloadFilename[MAX_PATH];
	::GetTempFileName(basePath.c_str(), _T("download"), 0, tDownloadFilename);
	
	// Set the status 
	tstring downloadFilename = tDownloadFilename;
	tstring status = _T("Downloading ");
	status.append(_url);
	setStatus(status.c_str());

	// Link up the progress callback
	downloadManager.setProgressFunction(stepProgress);


	tstring contentType;
	downloadManager.getUrl(_url.c_str(), downloadFilename, contentType, _proxy.c_str(), _proxyPort);

	if (contentType == _T("application/zip") 
		|| contentType == _T("application/octet-stream"))
	{
		// Attempt to unzip file into basePath
		Decompress::unzip(downloadFilename, basePath);
		
	}
	else if (contentType == _T("text/html"))
	{
	    DirectLinkSearch linkSearch(downloadFilename.c_str());
		shared_ptr<TCHAR> realLink = linkSearch.search(_filename.c_str());
		

		if (realLink.get())
		{
			_url = realLink.get();
			return perform(basePath, forGpup, setStatus, stepProgress);
		}
		else
			return STEPSTATUS_FAIL;
	}
	
	return STEPSTATUS_SUCCESS;
}