#ifndef _PLUGINVERSION_H
#define _PLUGINVERSION_H

#include <tchar.h>
#include <string.h>
#include "tstring.h"



class PluginVersion
{
public:
	PluginVersion(void);
	PluginVersion(const char *version);
	PluginVersion(const TCHAR *version);
	PluginVersion(std::string version);
	PluginVersion(tstring version);
	PluginVersion(int major, int minor, int revision, int build);


	PluginVersion& operator= (const char *rhs);
	PluginVersion& operator= (const TCHAR *rhs);
	PluginVersion& operator= (std::string &rhs);
	PluginVersion& operator= (tstring &rhs);

	bool		operator<	(PluginVersion &rhs);
	bool		operator<=	(PluginVersion &rhs);
	bool		operator>	(PluginVersion &rhs);
	bool		operator>=	(PluginVersion &rhs);
	bool		operator==	(PluginVersion &rhs);
	bool		operator!=  (PluginVersion &rhs);

	TCHAR* getDisplayString();

private:

	/* Private version members */

	int _major, _minor, _revision, _build;

	int compare(PluginVersion &lhs, PluginVersion &rhs);
	void parseString(const char *version);
	void parseString(const TCHAR *version);


	TCHAR* _displayString;

};


#endif