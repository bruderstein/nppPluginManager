#ifndef _PLUGIN_H
#define _PLUGIN_H

#include <windows.h>
#include <string>
#include "tstring.h"
#include "PluginVersion.h"

class Plugin
{
public:
	Plugin(void);
    ~Plugin(void);

	/* Setters */
	void	setName			(const char* name);
	void	setName			(tstring name);
    void	setUnicodeUrl	(const char* unicodeUrl);
	void	setAnsiUrl		(const char* ansiUrl);
	void	setVersion	(PluginVersion &version);
	void	setDescription	(const char* description);
	void	setFilename		(const char* filename);
	void	setFilename		(tstring filename);
	void	setInstalledVersion(PluginVersion &version);
	
	/* Getters */
	tstring&		getName();
	tstring&		getUnicodeUrl();
	tstring&		getAnsiUrl();
	PluginVersion	getVersion();
	tstring&		getDescription();
	tstring&		getFilename();
	PluginVersion	getInstalledVersion();


	/* General methods */
	BOOL			isInstalled();

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

	/* TCHAR Conversion function */
	void   setTstring(const char *src, tstring &dest);
};

#endif