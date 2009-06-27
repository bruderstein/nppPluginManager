#pragma once
#include <tchar.h>
#include "PluginInterface.h"
#include "Plugin.h"
#include "PluginManager.h"
#include "tinyxml.h"
#include "VariableHandler.h"

class PluginList
{
public:
	PluginList(void);
	~PluginList(void);

	void init(NppData *nppData);

	BOOL parsePluginFile(CONST TCHAR *filename);
	BOOL checkInstalledPlugins(TCHAR *nppDirectory);
	PluginListContainer& getInstalledPlugins();
	PluginListContainer& getUpdateablePlugins();
	PluginListContainer& getAvailablePlugins();

	VariableHandler*     getVariableHandler();


private:
	/* Plugin filename map */
	PluginContainer _plugins;

	/* Lists of plugins */
	PluginListContainer		_installedPlugins;
	PluginListContainer	    _updateablePlugins;
	PluginListContainer		_availablePlugins;



    void        addInstallSteps(Plugin* plugin, TiXmlElement* installElement);
	BOOL		setInstalledVersion(tstring filename, Plugin* plugin);
	tstring		getPluginName(tstring filename);
	
	VariableHandler* _variableHandler;
	NppData*         _nppData;
};
