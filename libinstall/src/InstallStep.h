#ifndef _INSTALLSTEP_H
#define _INSTALLSTEP_H

#include <windows.h>
#include "tstring.h"



class InstallStep
{
public:
	InstallStep() {};
	~InstallStep() {};
	virtual BOOL Perform() { return TRUE; };

protected:
	void setTstring(const char *src, tstring &dest);

};

#endif