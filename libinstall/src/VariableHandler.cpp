#include "VariableHandler.h"
#include "tstring.h"

using namespace std;


VariableHandler::VariableHandler(const TCHAR *nppDir, const TCHAR *pluginDir, const TCHAR *configDir)
	 : _nppDir(nppDir), 
	   _pluginDir(pluginDir),
	   _configDir(configDir),
	   VAR_NPPDIR(_T("$NPPDIR$")),
	   VAR_PLUGINDIR(_T("$PLUGINDIR$")),
	   VAR_CONFIGDIR(_T("$CONFIGDIR$"))
{
}


void VariableHandler::replaceVariables(tstring &source)
{
	tstring::size_type p = source.find(VAR_PLUGINDIR);
	if (p != tstring::npos)
		source.replace(p, VAR_PLUGINDIR.size(), _pluginDir);

	p = source.find(VAR_NPPDIR);
	if (p != string::npos)
		source.replace(p, VAR_NPPDIR.size(), _nppDir);

	p = source.find(VAR_CONFIGDIR);
	if (p != string::npos)
		source.replace(p, VAR_CONFIGDIR.size(), _configDir);
}

const tstring& VariableHandler::getConfigDir()
{
	return _configDir;
}

const tstring& VariableHandler::getNppDir()
{
	return _nppDir;
}
	
