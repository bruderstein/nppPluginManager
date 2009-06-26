#ifndef _COPYSTEP_H
#define _COPYSTEP_H
#include <windows.h>
#include <string>
#include "InstallStep.h"
#include <tchar.h>
#include "tstring.h"


class CopyStep : public InstallStep
{
public:
	CopyStep(const TCHAR* from, const TCHAR* to, BOOL attemptReplace);
	~CopyStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		boost::function<void(const TCHAR*)> setStatus,
	 boost::function<void(const int)> stepProgress);

private:
	tstring	_from;
	tstring _to;
	BOOL    _failIfExists;
};

#endif
