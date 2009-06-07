#include "Plugin.h"
#include <tchar.h>
#include <string>


using namespace std;
using namespace boost;


Plugin::Plugin(void)
{
}

Plugin::~Plugin(void)
{

}


/* Setters */

void Plugin::setAnsiUrl(const char* ansiUrl)
{
	setTstring(ansiUrl, _ansiUrl);
}

void Plugin::setDescription(const char* description)
{
	setTstring(description, _description);
}

void Plugin::setFilename(const char* filename)
{
	setTstring(filename, _filename);
}

void Plugin::setFilename(tstring filename)
{
	_filename = filename;
}

void Plugin::setName(const char *name)
{
	setTstring(name, _name);
}

void Plugin::setName(tstring name)
{
	_name = name;
}

void Plugin::setUnicodeUrl(const char *unicodeUrl)
{
	setTstring(unicodeUrl, _unicodeUrl);
}

void Plugin::setVersion(PluginVersion &version)
{
	_version = version;
}



void Plugin::setInstalledVersion(PluginVersion &version)
{
	_installedVersion = version;
	_isInstalled = TRUE;
}

void Plugin::setInstalledVersionFromHash(tstring &hash)
{
	if (_versionMap.find(hash) != _versionMap.end())
	{
		_installedVersion = _versionMap[hash];
	}
}

/* Getters */
tstring& Plugin::getAnsiUrl()
{
	return _ansiUrl;
}

tstring& Plugin::getDescription()
{
	return _description;
}

tstring& Plugin::getFilename()
{
	return _filename;
}

tstring& Plugin::getName()
{
	return _name;
}

tstring& Plugin::getUnicodeUrl()
{
	return _unicodeUrl;
}



PluginVersion Plugin::getVersion()
{
	return _version;
}


PluginVersion Plugin::getInstalledVersion()
{
	return _installedVersion;
}

	
BOOL Plugin::isInstalled()
{
	return _isInstalled;
}

void Plugin::addVersion(const char* hash, PluginVersion &version)
{
	tstring *hashString = new tstring;
	setTstring(hash, (*hashString));

	_versionMap[(*hashString)] = PluginVersion(version);
}

void Plugin::addInstallStep(shared_ptr<InstallStep> step)
{
	_installSteps.push_back(step);
}


void Plugin::setTstring(const char *src, tstring &dest)
{
#ifdef _UNICODE
	TCHAR *tmpBuf = new TCHAR[strlen(src) + 1];
	int len = strlen(src); 
    size_t newSize = mbstowcs(tmpBuf, src, len); 
	tmpBuf[newSize] = '\0';
	dest = tmpBuf;
	delete[] tmpBuf;
#else
	dest = src;
#endif
}
