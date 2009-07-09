#pragma once
#include <tchar.h>

#include "PluginManager.h"
#include "libinstall/VariableHandler.h"
#include "tinyxml/tinyxml.h"
#include "Plugin.h"
#include "ProgressDialog.h"
#include "PluginListView.h"

class PluginList
{
public:
	PluginList(void);
	~PluginList(void);

	void init(NppData *nppData);
	void downloadList();

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

	/* Installs or updates given list of plugins, and also includes dependencies 
	 * Warns user with messageboxes about intended actions
	 * Restarts using GPUP
	 */
	

	void startInstall(HWND hMessageBoxParent, 
							  ProgressDialog* progressDialog, 
							  PluginListView *pluginListView, 
							  BOOL isUpdate);


    void startRemove(HWND hMessageBoxParent, 
							  ProgressDialog* progressDialog, 
							  PluginListView *pluginListView);


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
	
	void installPlugins(HWND hMessageBoxParent, ProgressDialog* progressDialog, PluginListView* pluginListView, BOOL isUpgrade);
	void removePlugins(HWND hMessageBoxParent, ProgressDialog* progressDialog, PluginListView* pluginListView);

	static UINT installThreadProc(LPVOID param);
	static UINT removeThreadProc(LPVOID param);

	VariableHandler* _variableHandler;
	NppData*         _nppData;
};
