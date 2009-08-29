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

#ifndef PLUGIN_TEMPLATE_H
#define PLUGIN_TEMPLATE_H

#define WIN32_LEAN_AND_MEAN
//#include "SysMsg.h"
#include "PluginInterface.h"
#include <TCHAR.H>
#include <map>
#include <string>
#include <list>
#include "Plugin.h"
#include "tstring.h"




#define PLUGINS_MD5_URL     _T("http://notepad-plus.sourceforge.net/commun/pluginManager/plugins.md5.txt")
#define PLUGINS_URL         _T("http://notepad-plus.sourceforge.net/commun/pluginManager/plugins.xml")
#define VALIDATE_URL         _T("http://notepad-plus.sourceforge.net/commun/pluginManager/validate.php?md5=")






/* ini file name */
CONST TCHAR PLUGINMANAGER_INI[]	= _T("\\PluginManager.ini");


#define SETTINGS_GROUP     _T("Settings")
#define KEY_NOTIFYUPDATES  _T("NotifyUpdates")
#define KEY_PROXY          _T("Proxy")
#define KEY_PROXYPORT      _T("ProxyPort")
#define KEY_LASTCHECK	   _T("LastCheck")
#define KEY_SHOWUNSTABLE   _T("ShowUnstable")
#ifdef ALLOW_OVERRIDE_XML_URL
#define KEY_OVERRIDEMD5URL  _T("md5url")
#define KEY_OVERRIDEURL     _T("xmlurl")
#endif





typedef std::map<tstring, Plugin*>		PluginContainer;
typedef std::list<Plugin*>				PluginListContainer;


/* load and save properties from/into ini file */
void loadSettings(void);
void saveSettings(void);

/* menu functions */
void doAboutDlg(void);
void doPluginManagerDlg(void);

extern HANDLE g_hModule;
extern BOOL   g_isUnicode;

struct Options
{
	std::string proxy;
	long proxyPort;
	BOOL notifyUpdates;
	time_t lastCheck;
	BOOL showUnstable;
#ifdef ALLOW_OVERRIDE_XML_URL
	tstring downloadMD5Url;
	tstring downloadUrl;
#endif
};

extern Options			g_options;



#endif	/* PLUGIN_TEMPLATE_H */