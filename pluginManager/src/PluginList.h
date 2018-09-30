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
#pragma once
#include "PluginManager.h"
#include "libinstall/VariableHandler.h"
#include "tinyxml/tinyxml.h"
#include "Plugin.h"
#include "ProgressDialog.h"
#include "PluginListView.h"

enum InstallOrRemove
{
	INSTALL,
	REMOVE
};

class PluginList
{
public:
	PluginList(void);
	~PluginList(void);

	void init(NppData *nppData);
	void downloadList();
	void reparseFile(const tstring& pluginsListFilename);

	BOOL parsePluginFile(CONST TCHAR *filename);
	BOOL checkInstalledPlugins();

	PluginListContainer& getInstalledPlugins();
	PluginListContainer& getUpdateablePlugins();
	PluginListContainer& getAvailablePlugins();

	Plugin*				 getPlugin(tstring name);
	VariableHandler*     getVariableHandler();

	// Returns true if the plugin is installable or upgradable
	BOOL				 isInstallOrUpgrade(const tstring& pluginName);

/* Checks dependencies on a list of plugins
 *  Any dependencies are added to the list, and a list of names of plugins added is returned
 */
	std::shared_ptr< std::list<tstring> > calculateDependencies(std::shared_ptr< std::list<Plugin*> > selectedPlugins);

	/* Installs or updates given list of plugins, and also includes dependencies
	 * Warns user with messageboxes about intended actions
	 * Restarts using GPUP
	 */


	void startInstall(HWND hMessageBoxParent,
							  ProgressDialog* progressDialog,
							  PluginListView *pluginListView,
							  BOOL isUpdate,
							  CancelToken& cancelToken);


	void startRemove(HWND hMessageBoxParent,
							  ProgressDialog* progressDialog,
							  PluginListView *pluginListView,
							  CancelToken& cancelToken);

	/* Waits until the list has been downloaded and processed */
	void waitForListsAvailable();

	/* Returns true if the lists have been downloaded and processed already */
	BOOL listsAvailable();

private:
	/* Plugin name map */
	PluginContainer _plugins;
	PluginContainer _libraries;

	/* Hashes to real names, for plugins that dynamically report their names */
	std::map<tstring, tstring> _pluginRealNames;

	/* Aliases of plugins with different names for different versions */
	std::map<tstring, tstring> _aliases;

	/* Lists of plugins */
	PluginListContainer		_installedPlugins;
	PluginListContainer	    _updateablePlugins;
	PluginListContainer		_availablePlugins;


	/* Event for list being available */
	HANDLE		_hListsAvailableEvent;

	/* Notepad++ handles */
	NppData*    _nppData;

	/* Object that knows variable definitions - e.g. $PLUGINDIR$ */
	VariableHandler* _variableHandler;

	/* Notepad++ Version */
	PluginVersion _nppVersion;

	void        addInstallSteps(Plugin* plugin, TiXmlElement* installElement);
	BOOL		setInstalledVersion(tstring filename, Plugin* plugin);
	tstring		getPluginName(tstring filename);


	TiXmlDocument* getGpupDocument(const TCHAR* filename);

	BOOL checkInstalledPlugins(const TCHAR *nppDirectory, BOOL allUsers);
	BOOL checkInstalledPluginsInSubdirs(const TCHAR *pluginPath, BOOL allUsers);
	void addAvailablePlugins();

	void installPlugins(HWND hMessageBoxParent, ProgressDialog* progressDialog, PluginListView* pluginListView, BOOL isUpgrade, CancelToken& cancelToken);
	void removePlugins(HWND hMessageBoxParent, ProgressDialog* progressDialog, PluginListView* pluginListView, CancelToken& cancelToken);
	void addPluginNames(TiXmlElement* pluginNamesElement);

	static UINT installThreadProc(LPVOID param);
	static UINT removeThreadProc(LPVOID param);

	void clearPluginList();


	void addSteps(Plugin* plugin, TiXmlElement* installElement, InstallOrRemove ior);

	TCHAR *getPluginsUrl();
	TCHAR *getPluginsMd5Url();
	TCHAR *getValidateUrl();
};
