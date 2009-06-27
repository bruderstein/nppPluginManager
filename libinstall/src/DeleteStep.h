#ifndef _DELETESTEP_H
#define _DELETESTEP_H
#include <windows.h>
#include <string>
#include "InstallStep.h"
#include <tchar.h>
#include "tstring.h"


class DeleteStep : public InstallStep
{
public:
	DeleteStep(const TCHAR* file);
	~DeleteStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		boost::function<void(const TCHAR*)> setStatus,
		boost::function<void(const int)> stepProgress);

private:
	tstring	_file;
	
};

#endif
