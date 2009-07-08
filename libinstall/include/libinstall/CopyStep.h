#ifndef _COPYSTEP_H
#define _COPYSTEP_H
#include <windows.h>
#include <string>
#include "InstallStep.h"
#include <tchar.h>
#include "tstring.h"

#define VALIDATE_BASEURL          _T("http://localhost:100/validate.php?md5=")
#define VALIDATE_RESULT_OK        "ok"
#define VALIDATE_RESULT_UNKNOWN   "unknown"
#define VALIDATE_RESULT_BANNED    "banned"

enum ValidateStatus
{
	VALIDATE_OK,
	VALIDATE_UNKNOWN,
	VALIDATE_BANNED
};



class CopyStep : public InstallStep
{
public:
	CopyStep(const TCHAR* from, const TCHAR* to, BOOL attemptReplace, BOOL validate,
		const char* proxy, long proxyPort);

	~CopyStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		 boost::function<void(const TCHAR*)> setStatus,
	     boost::function<void(const int)> stepProgress);

private:
	
	ValidateStatus Validate(tstring& file);


	tstring	_from;
	tstring _to;
	
	std::string _proxy;
	long    _proxyPort;
	
	BOOL    _failIfExists;
	BOOL    _validate;
};

#endif
