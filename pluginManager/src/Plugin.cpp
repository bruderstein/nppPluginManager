/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009-2010 Dave Brotherstone <davegb@pobox.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "precompiled_headers.h"
#include "Plugin.h"
#include "PluginVersion.h"
#include "libinstall/VariableHandler.h"
#include "libinstall/ModuleInfo.h"

#include "tinyxml/tinyxml.h"

using namespace std;


Plugin::Plugin(void)
: _isInstalled(FALSE),
  _detailsAdded(FALSE),
  _updateDetailsAdded(FALSE),
  _installedForAllUsers(FALSE),
  _isLibrary(FALSE)
{
}

Plugin::~Plugin(void)
{

}


/* Setters */

void Plugin::setDescription(const TCHAR* description)
{
	_description = description;
	replaceNewlines(_description);
}

void Plugin::setFilename(const TCHAR* filename)
{
	_filename = filename;;
}

void Plugin::setFilename(const tstring& filename)
{
	_filename = filename;
}

void Plugin::setName(const TCHAR* name)
{
	_name = name;
}

void Plugin::setName(const tstring& name)
{
	_name = name;
}

void Plugin::setAuthor(const TCHAR* author)
{
	_author = author;
}

void Plugin::setCategory(const TCHAR* category)
{
	_category = category;
}

void Plugin::setVersion(const PluginVersion &version)
{
	_version = version;
}

void Plugin::setHomepage(const TCHAR* homepage)
{
	_homepage = homepage;
}

void Plugin::setSourceUrl(const TCHAR* sourceUrl)
{
	_sourceUrl = sourceUrl;
}

void Plugin::setLatestUpdate(const TCHAR* latestUpdate)
{
	_latestUpdate = latestUpdate;
	replaceNewlines(_latestUpdate);
}

void Plugin::setStability(const TCHAR* stability)
{
	_stability = stability;
}


void Plugin::setInstalledVersion(const PluginVersion &version)
{
	_installedVersion = version;
	if (_badVersionMap.find(_installedVersion) != _badVersionMap.end())
		_installedVersion.setIsBad(true);

	_isInstalled = TRUE;
}

void Plugin::setInstalledVersionFromHash(const tstring &hash)
{
	if (_versionMap.find(hash) != _versionMap.end())
	{
		_installedVersion = _versionMap[hash];
		if (_badVersionMap.find(_installedVersion) != _badVersionMap.end())
			_installedVersion.setIsBad(true);

	}
	_isInstalled = TRUE;
}

void Plugin::setInstalledForAllUsers(BOOL installedForAllUsers)
{
	_installedForAllUsers = installedForAllUsers;
}

/* Getters */

tstring& Plugin::getDescription()
{
	if (!_detailsAdded)
	{
		if (_stability != _T("Good"))
		{
			_description.append(_T("\r\nStability: "));
			_description.append(_stability);
		}

		if (!_author.empty())
		{
			_description.append(_T("\r\nAuthor: "));
			_description.append(_author);
		}
		if (!_sourceUrl.empty())
		{
			_description.append(_T("\r\nSource: "));
			_description.append(_sourceUrl);
		}
		if (!_homepage.empty())
		{
			_description.append(_T("\r\nHomepage: "));
			_description.append(_homepage);
		}

		if (!_latestUpdate.empty())
		{
			_description.append(_T("\r\nLatest update: "));
			_description.append(_latestUpdate);
		}

		_detailsAdded = TRUE;
	}

	return _description;
}

tstring& Plugin::getUpdateDescription()
{
	if (!_updateDetailsAdded)
	{
		if (isInstalled() && _installedVersion.getIsBad())
		{
			_updateDescription.append(_T("The version of this plugin that is installed has been marked as unstable.  "));
			if (!_badVersionMap[_installedVersion].empty())
			{
				_updateDescription.append(_badVersionMap[_installedVersion]);
			}
			_updateDescription.append(_T("\r\n"));
		}

		if (!_latestUpdate.empty())
		{
			
			_updateDescription.append(_T("Latest update: "));
			_updateDescription.append(_latestUpdate);
			_updateDescription.append(_T("\r\n"));
		}

		if (_stability != _T("Good"))
		{
			_updateDescription.append(_T("Stability: "));
			_updateDescription.append(_stability);
			_updateDescription.append(_T("\r\n"));
		}

		// If there's nothing, just add the description
		if (_updateDescription.empty())
		{
			_updateDescription.append(_description);
		}

		_updateDetailsAdded = true;
	}

	return _updateDescription;
}

tstring& Plugin::getFilename()
{
	return _filename;
}

tstring& Plugin::getName()
{
	return _name;
}



PluginVersion& Plugin::getVersion()
{
	return _version;
}


PluginVersion& Plugin::getInstalledVersion()
{
	return _installedVersion;
}

	
tstring& Plugin::getAuthor()
{
	return _author;
}

tstring& Plugin::getCategory()
{
	return _category;
}

tstring& Plugin::getStability()
{
	return _stability;
}

tstring& Plugin::getLatestUpdate()
{
	return _latestUpdate;
}

BOOL Plugin::getInstalledForAllUsers()
{
	return _installedForAllUsers;
}


BOOL Plugin::isInstalled()
{
	return _isInstalled;
}

void Plugin::addVersion(const TCHAR* hash, const PluginVersion &version)
{
	_versionMap[hash] = version;
}

void Plugin::addBadVersion(const PluginVersion &version, const TCHAR* report)
{
	_badVersionMap[version] = report;
}


void Plugin::addInstallStep(std::shared_ptr<InstallStep> step)
{
	_installSteps.push_back(step);
}

void Plugin::addRemoveStep(std::shared_ptr<InstallStep> step)
{
	_removeSteps.push_back(step);
}



size_t Plugin::getInstallStepCount()
{
	return _installSteps.size();
}

size_t Plugin::getRemoveStepCount()
{
	// Add 1 for removal of plugin dll file
	return _removeSteps.size() + 1;
}


InstallStatus Plugin::install(tstring& basePath, TiXmlElement* forGpup, 
									  std::function<void(const TCHAR*)> setStatus,
									  std::function<void(const int)> stepProgress,
									  std::function<void()> stepComplete,
									  const ModuleInfo* moduleInfo,
									  VariableHandler* variableHandler,
                                      CancelToken& cancelToken)
{
	

	return runSteps(_installSteps, basePath, forGpup, setStatus, stepProgress, stepComplete, moduleInfo, variableHandler, cancelToken);
}


InstallStatus Plugin::remove(tstring& basePath, TiXmlElement* forGpup, 
									  std::function<void(const TCHAR*)> setStatus,
									  std::function<void(const int)> stepProgress,
									  std::function<void()> stepComplete,
									  const ModuleInfo* moduleInfo,
									  VariableHandler* variableHandler,
                                      CancelToken& cancelToken)
{
	
	TiXmlElement* deleteElement = new TiXmlElement(_T("delete"));
	
	
	// Save a copy of the current plugin dir
	const tstring& origPluginDir = variableHandler->getVariable(_T("PLUGINDIR"));

	// replace PLUGINDIR with the plugin dir of this plugin
	if (this->getInstalledForAllUsers())
	{
		variableHandler->setVariable(_T("PLUGINDIR"), (variableHandler->getVariable(_T("ALLUSERSPLUGINDIR")).c_str()));
	}
	else
	{
		variableHandler->setVariable(_T("PLUGINDIR"), (variableHandler->getVariable(_T("USERPLUGINDIR")).c_str()));
	}

	tstring fullFilename(variableHandler->getVariable(_T("PLUGINDIR")));
	fullFilename.push_back(_T('\\'));
	fullFilename.append(getFilename());
	deleteElement->SetAttribute(_T("file"), fullFilename.c_str());

	forGpup->LinkEndChild(deleteElement);	
	
	runSteps(_removeSteps, basePath, forGpup, setStatus, stepProgress, stepComplete, moduleInfo, variableHandler, cancelToken);

	// restore the original plugin dir
	variableHandler->setVariable(_T("PLUGINDIR"), origPluginDir.c_str());

	return INSTALL_NEEDRESTART;
}



InstallStatus Plugin::runSteps(InstallStepContainer steps, tstring& basePath, TiXmlElement* forGpup, 
									  std::function<void(const TCHAR*)> setStatus,
									  std::function<void(const int)> stepProgress,
									  std::function<void()> stepComplete,
									  const ModuleInfo* moduleInfo,
									  VariableHandler* variableHandler,
                                      CancelToken& cancelToken)
{
	InstallStatus status = INSTALL_SUCCESS;

	InstallStepContainer::iterator stepIterator = steps.begin();
	 
	variableHandler->setVariable(_T("PLUGINFILENAME"), getFilename().c_str());
	
	StepStatus stepStatus;

	while (stepIterator != steps.end())
	{

		if (variableHandler)
			(*stepIterator)->replaceVariables(variableHandler);

		stepStatus = (*stepIterator)->perform(basePath, forGpup, setStatus, stepProgress, moduleInfo, cancelToken);

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



void Plugin::addDependency(const TCHAR* pluginName)
{
	_dependencies.push_back(pluginName);
}


BOOL Plugin::hasDependencies()
{
	return !_dependencies.empty();
}

const list<tstring>& Plugin::getDependencies()
{
	return _dependencies;
}

void Plugin::replaceNewlines(tstring &str)
{
	tstring::size_type pos = 0;
	
	do {
		pos = str.find(TEXT("\\n"), pos);
		if (pos != tstring::npos)
		{
			str.replace(pos, 2, TEXT("\r\n"));
		}
	} while (pos != tstring::npos);
}
