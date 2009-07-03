/*
This file is part of tPluginProp Plugin for Notepad++
Copyright (C)2007 Jens Lorenz <jens.plugin.npp@gmx.de>

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
#include "SysMsg.h"
#include "PluginInterface.h"
#include <TCHAR.H>
#include <map>
#include <string>
#include <list>
#include "Plugin.h"
#include "tstring.h"


/* ini file name */
CONST TCHAR PLUGINMANAGER_INI[]	= _T("\\PluginManager.ini");


CONST TCHAR SETTINGS_GROUP[]    = _T("Settings");
CONST TCHAR KEY_NOTIFYUPDATES[] = _T("NotifyUpdates");
CONST TCHAR KEY_PROXY[]         = _T("Proxy");
CONST TCHAR KEY_PROXYPORT[]     = _T("ProxyPort");




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
};

extern Options			g_options;



#endif	/* PLUGIN_TEMPLATE_H */