#ifndef _VARIABLEHANDLER_H
#define _VARIABLEHANDLER_H

#include "tstring.h"

class VariableHandler
{
public:
	VariableHandler(const TCHAR* nppDir, const TCHAR* pluginDir, const TCHAR* configDir);
		
	void replaceVariables(tstring &source);

	const tstring& getConfigDir();
	const tstring& getNppDir();

private:
	tstring _nppDir;
	tstring _pluginDir;
	tstring _configDir;
	tstring VAR_NPPDIR;
	tstring VAR_PLUGINDIR;
	tstring VAR_CONFIGDIR;
};



#endif
