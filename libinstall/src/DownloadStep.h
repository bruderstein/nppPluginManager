#ifndef _DOWNLOADSTEP_H
#define _DOWNLOADSTEP_H
#include <windows.h>
#include <string>
#include "InstallStep.h"
#include <tchar.h>
#include "tstring.h"


class DownloadStep : public InstallStep
{
public:
	DownloadStep(const TCHAR* url, const TCHAR* filename);
	~DownloadStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		boost::function<void(const TCHAR*)> setStatus,
    	boost::function<void(const int)> stepProgress);

private:
	tstring	_url;
	tstring _filename;
};

#endif
