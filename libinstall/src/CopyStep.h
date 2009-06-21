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
	CopyStep(const TCHAR* from, const TCHAR* to);
	~CopyStep() {};
	
	BOOL Perform(tstring& basePath);

private:
	tstring	_from;
	tstring _to;
};

#endif
