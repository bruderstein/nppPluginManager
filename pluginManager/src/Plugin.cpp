/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009 Dave Brotherstone <davegb@pobox.com>

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

#include "Plugin.h"
#include <tchar.h>
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "libinstall/VariableHandler.h"
#include "tinyxml/tinyxml.h"

using namespace std;
using namespace boost;


Plugin::Plugin(void)
{
	_isInstalled = FALSE;
	_detailsAdded = FALSE;
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



void Plugin::setInstalledVersion(const PluginVersion &version)
{
	_installedVersion = version;
	_isInstalled = TRUE;
}

void Plugin::setInstalledVersionFromHash(const tstring &hash)
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
	if (!_detailsAdded)
	{		
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

		_detailsAdded = TRUE;
	}

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

BOOL Plugin::isInstalled()
{
	return _isInstalled;
}

void Plugin::addVersion(const TCHAR* hash, const PluginVersion &version)
{
	tstring *hashString = new tstring;
	*hashString = hash;

	_versionMap[(*hashString)] = version;
}

void Plugin::addInstallStep(shared_ptr<InstallStep> step)
{
	_installSteps.push_back(step);
}


size_t Plugin::getInstallStepCount()
{
	return _installSteps.size();
}



InstallStatus Plugin::install(tstring& basePath, TiXmlElement* forGpup, 
									  boost::function<void(const TCHAR*)> setStatus,
									  boost::function<void(const int)> stepProgress,
									  boost::function<void()> stepComplete,
									  const HWND windowParent,
									  VariableHandler* variableHandler)
{
	InstallStatus status = INSTALL_SUCCESS;

	InstallStepContainer::iterator stepIterator = _installSteps.begin();
	 
	variableHandler->setVariable(_T("PLUGINFILENAME"), getFilename().c_str());
	
	StepStatus stepStatus;

	while (stepIterator != _installSteps.end())
	{

		if (variableHandler)
			(*stepIterator)->replaceVariables(variableHandler);

		stepStatus = (*stepIterator)->perform(basePath, forGpup, setStatus, stepProgress, windowParent);

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
	tstring plugin = pluginName;
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


