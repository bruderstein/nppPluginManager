#ifndef _INSTALLSTEP_H
#define _INSTALLSTEP_H

#include <windows.h>
#include <boost/function.hpp>
#include "tstring.h"
#include "tinyxml/tinyxml.h"


enum StepStatus 
{
	STEPSTATUS_SUCCESS,
	STEPSTATUS_NEEDGPUP,
	STEPSTATUS_FAIL
};



class InstallStep
{
public:
	InstallStep() {};
	~InstallStep() {};

	virtual StepStatus perform(tstring &/*basePath*/, TiXmlElement* /*forGpup*/, 
		boost::function<void(const TCHAR*)> setStatus,
		boost::function<void(const int)> stepProgress) { return STEPSTATUS_SUCCESS; };

protected:
//	void setTstring(const char *src, tstring &dest);

};

#endif