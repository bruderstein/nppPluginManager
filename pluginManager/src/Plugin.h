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
#ifndef _PLUGIN_H
#define _PLUGIN_H

#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "tstring.h"
#include "tinyxml/tinyxml.h"
#include "PluginVersion.h"
#include "libinstall/InstallStep.h"


enum InstallStatus {
		INSTALL_SUCCESS,
		INSTALL_NEEDRESTART,
		INSTALL_FAIL
};

class Plugin
{
public:
	Plugin(void);
    ~Plugin(void);

	

	/* Setters */
	void	setName			(const TCHAR* name);
	void	setName			(const tstring& name);
    void	setVersion	(const PluginVersion &version);
	void	setDescription	(const TCHAR* description);
	void	setFilename		(const TCHAR* filename);
	void	setFilename		(const tstring& filename);
	void	setInstalledVersion(const PluginVersion &version);
	void	setInstalledVersionFromHash(const tstring &hash);
	void	setAuthor(const TCHAR* author);
	void    setHomepage(const TCHAR* homepage);
	void    setSourceUrl(const TCHAR* sourceUrl);
	void	setCategory(const TCHAR* category);

	/* Getters */
	tstring&		getName();
	PluginVersion&	getVersion();
	tstring&		getDescription();
	tstring&		getFilename();
	PluginVersion&	getInstalledVersion();
	tstring&		getAuthor();
	tstring&		getCategory();


	/* General methods */
	BOOL			isInstalled();
	void			addVersion(const TCHAR* hash, const PluginVersion &version);

	/* installation */
	void			addInstallStep(boost::shared_ptr<InstallStep> step);
	size_t				getInstallStepCount();
	InstallStatus   install(tstring& basePath, TiXmlElement* forGpup, 
		boost::function<void(const TCHAR*)> setStatus,
		boost::function<void(const int)> stepProgress,
		boost::function<void()> stepComplete,
		const HWND windowParent);


	/* dependencies */
	void				addDependency(const TCHAR* pluginName);
	BOOL				hasDependencies();
	const std::list<tstring>& getDependencies();

private:
	tstring					_name;
	tstring					_unicodeUrl;
	tstring					_ansiUrl;
	PluginVersion			_version;      
	tstring					_description;
	tstring					_filename;
	PluginVersion			_installedVersion;
	tstring					_author;
	tstring					_category;
	tstring					_homepage;
	tstring					_sourceUrl;
	
	BOOL					_isInstalled;
	BOOL					_detailsAdded;
	/* Dependencies on other plugins */
	std::list<tstring>		_dependencies;

	std::map<tstring, PluginVersion>  _versionMap;
	
	typedef std::list<boost::shared_ptr<InstallStep> > InstallStepContainer;

	InstallStepContainer	_installSteps;
};

#endif