#ifndef _PLUGIN_H
#define _PLUGIN_H

#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <memory>
#include "tstring.h"
#include "PluginVersion.h"
#include "InstallStep.h"
#include <boost/shared_ptr.hpp>

class Plugin
{
public:
	Plugin(void);
    ~Plugin(void);

	enum InstallStatus {
		INSTALL_SUCCESS,
		INSTALL_NEEDRESTART,
		INSTALL_FAIL
	};

	/* Setters */
	void	setName			(const TCHAR* name);
	void	setName			(tstring name);
    void	setVersion	(PluginVersion &version);
	void	setDescription	(const TCHAR* description);
	void	setFilename		(const TCHAR* filename);
	void	setFilename		(tstring filename);
	void	setInstalledVersion(PluginVersion &version);
	void	setInstalledVersionFromHash(tstring &hash);


	/* Getters */
	tstring&		getName();
	PluginVersion	getVersion();
	tstring&		getDescription();
	tstring&		getFilename();
	PluginVersion	getInstalledVersion();


	/* General methods */
	BOOL			isInstalled();
	void			addVersion(const TCHAR* hash, PluginVersion &version);
	void			addInstallStep(boost::shared_ptr<InstallStep> step);
	InstallStatus   install();

private:
	tstring					_name;
	tstring					_unicodeUrl;
	tstring					_ansiUrl;
	PluginVersion			_version;      
	tstring					_description;
	tstring					_filename;
	PluginVersion			_installedVersion;
	//DependencyContainer	_dependencies;
	BOOL					_isInstalled;

	
	std::map<tstring, PluginVersion>  _versionMap;
	/* TCHAR Conversion function */
	void   setTstring(const char *src, tstring &dest);

	typedef std::list<boost::shared_ptr<InstallStep> > InstallStepContainer;

	InstallStepContainer	_installSteps;
};

#endif