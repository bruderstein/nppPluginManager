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
	void	setName			(const tstring& name);
    void	setVersion	(const PluginVersion &version);
	void	setDescription	(const TCHAR* description);
	void	setFilename		(const TCHAR* filename);
	void	setFilename		(const tstring& filename);
	void	setInstalledVersion(const PluginVersion &version);
	void	setInstalledVersionFromHash(const tstring &hash);


	/* Getters */
	tstring&		getName();
	PluginVersion&	getVersion();
	tstring&		getDescription();
	tstring&		getFilename();
	PluginVersion&	getInstalledVersion();


	/* General methods */
	BOOL			isInstalled();
	void			addVersion(const TCHAR* hash, const PluginVersion &version);

	/* installation */
	void			addInstallStep(boost::shared_ptr<InstallStep> step);
	size_t				getInstallStepCount();
	InstallStatus   install(tstring& basePath, TiXmlElement* forGpup, 
		boost::function<void(const TCHAR*)> setStatus,
		boost::function<void(const int)> stepProgress,
		boost::function<void()> stepComplete);


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
	
	BOOL					_isInstalled;

	/* Dependencies on other plugins */
	std::list<tstring>		_dependencies;

	std::map<tstring, PluginVersion>  _versionMap;
	
	typedef std::list<boost::shared_ptr<InstallStep> > InstallStepContainer;

	InstallStepContainer	_installSteps;
};

#endif