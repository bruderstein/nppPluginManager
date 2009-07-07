#pragma once
#include <tchar.h>

#include "PluginManager.h"
#include "libinstall/VariableHandler.h"
#include "tinyxml/tinyxml.h"
#include "Plugin.h"

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
	
	Plugin*				 getPlugin(tstring name);
	VariableHandler*     getVariableHandler();

	// Returns true if the plugin is installable or upgradable 
    BOOL				 isInstallOrUpgrade(const tstring& pluginName);

/* Checks dependencies on a list of plugins
 *  Any dependencies are added to the list, and a list of names of plugins added is returned
 */
	boost::shared_ptr< std::list<tstring> > calculateDependencies(boost::shared_ptr< std::list<Plugin*> > selectedPlugins);

private:
	/* Plugin name map */
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
