#pragma once
#include <tchar.h>

#include "Plugin.h"
#include "PluginManager.h"



class PluginList
{
public:
	PluginList(void);
	~PluginList(void);

	BOOL parsePluginFile(TCHAR *filename);
	BOOL checkInstalledPlugins(TCHAR *nppDirectory);
	PluginListContainer& getInstalledPlugins();
	PluginListContainer& getUpdateablePlugins();
	PluginListContainer& getAvailablePlugins();
private:
	/* Plugin filename map */
	PluginContainer _plugins;

	/* Lists of plugins */
	PluginListContainer		_installedPlugins;
	PluginListContainer	    _updateablePlugins;
	PluginListContainer		_availablePlugins;




	void		setInstalledVersion(tstring filename, Plugin* plugin);
	tstring		getPluginName(tstring filename);

};
