#include "Plugin.h"
#include <tchar.h>
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "tinyxml.h"

using namespace std;
using namespace boost;


Plugin::Plugin(void)
{
	_isInstalled = FALSE;
}

Plugin::~Plugin(void)
{

}


/* Setters */

void Plugin::setDescription(const TCHAR* description)
{
	_description = description; 
}

void Plugin::setFilename(const TCHAR* filename)
{
	_filename = filename;;
}

void Plugin::setFilename(tstring filename)
{
	_filename = filename;
}

void Plugin::setName(const TCHAR* name)
{
	_name = name;
}

void Plugin::setName(tstring name)
{
	_name = name;
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
	_isInstalled = TRUE;
}

/* Getters */

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

void Plugin::addVersion(const TCHAR* hash, PluginVersion &version)
{
	tstring *hashString = new tstring;
	*hashString = hash;

	_versionMap[(*hashString)] = PluginVersion(version);
}

void Plugin::addInstallStep(shared_ptr<InstallStep> step)
{
	_installSteps.push_back(step);
}


Plugin::InstallStatus Plugin::install(tstring& basePath, TiXmlElement* forGpup, 
									  boost::function<void(const TCHAR*)> setStatus,
									  boost::function<void(const int)> stepProgress,
									  boost::function<void()> stepComplete)
{
	InstallStatus status = INSTALL_SUCCESS;

	InstallStepContainer::iterator stepIterator = _installSteps.begin();
	 
	
	StepStatus stepStatus;

	while (stepIterator != _installSteps.end())
	{
		
		stepStatus = (*stepIterator)->perform(basePath, forGpup, setStatus, stepProgress);

		switch(stepStatus)
		{
			case STEPSTATUS_FAIL:
				// Stop processing
				return INSTALL_FAIL;

			case STEPSTATUS_NEEDGPUP:
				status = INSTALL_NEEDRESTART;
				break;
		}
		stepComplete();
		++stepIterator;
	}


	return status;
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
