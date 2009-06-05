/*
This file is part of Plugin Template Plugin Plugin for Notepad++
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

/* include files */
#include "stdafx.h"
#include "PluginManager.h"
#include "AboutDialog.h"
#include "PluginManagerDialog.h"
#include <shlwapi.h>
#include "Plugin.h"


/* information for notepad */
CONST INT	nbFunc	= 2;
CONST TCHAR	PLUGIN_NAME[] = _T("&Plugin Manager");

/* global values */
HANDLE				g_hModule			= NULL;
NppData				nppData;
FuncItem			funcItem[nbFunc];
BOOL				g_isUnicode;

/* dialog classes */
AboutDialog			aboutDlg;
PluginManagerDialog	pluginManagerDlg;

/* settings */
TCHAR				configPath[MAX_PATH];
TCHAR				iniFilePath[MAX_PATH];




/* main function of dll */
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID lpReserved )
{
	g_hModule = hModule;

	switch (reasonForCall)
	{
		case DLL_PROCESS_ATTACH:
		{
			/* Set function pointers */
			funcItem[0]._pFunc = doPluginManagerDlg;
			funcItem[1]._pFunc = doAboutDlg;
		    	
			/* Fill menu names */
			_tcscpy(funcItem[0]._itemName, _T("&Show Plugin Manager"));
			_tcscpy(funcItem[1]._itemName, _T("&About"));

			/* Set shortcuts */
			funcItem[0]._pShKey = new ShortcutKey;
			funcItem[0]._pShKey->_isAlt		= true;
			funcItem[0]._pShKey->_isCtrl	= true;
			funcItem[0]._pShKey->_isShift	= true;
			funcItem[0]._pShKey->_key		= 'T';
			funcItem[1]._pShKey = NULL;
			break;
		}	
		case DLL_PROCESS_DETACH:
		{
			delete funcItem[0]._pShKey;

			/* save settings */
			saveSettings();
			break;
		}
		case DLL_THREAD_ATTACH:
			break;
			
		case DLL_THREAD_DETACH:
			break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	/* stores notepad data */
	nppData = notpadPlusData;

	/* load data of plugin */
	loadSettings();

	/* initial dialogs */
	//TemplateDlg.init((HINSTANCE)g_hModule, nppData, &pluginProp);
	aboutDlg.init((HINSTANCE)g_hModule, nppData);
	pluginManagerDlg.init((HINSTANCE)g_hModule, nppData);
}

extern "C" __declspec(dllexport) CONST TCHAR * getName()
{
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(INT *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

/***
 *	beNotification()
 *
 *	This function is called, if a notification in Scantilla/Notepad++ occurs
 */
extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	if (notifyCode->nmhdr.hwndFrom == nppData._nppHandle)
	{
		/* on this notification code you can register your plugin icon in Notepad++ toolbar */
		if (notifyCode->nmhdr.code == NPPN_TBMODIFICATION)
		{
			
		}
	}
}

/***
 *	messageProc()
 *
 *	This function is called, if a notification from Notepad occurs
 */
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}



extern "C" __declspec(dllexport) BOOL isUnicode()
{
	#ifdef _UNICODE
	g_isUnicode = TRUE;
	#else
	g_isUnicode = FALSE;
	#endif

	return g_isUnicode;
}



/***
 *	loadSettings()
 *
 *	Load the parameters of plugin
 */
void loadSettings(void)
{
	/* initialize the config directory */
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)configPath);

	/* Test if config path exist */
	if (PathFileExists(configPath) == FALSE) {
		::CreateDirectory(configPath, NULL);
	}

	_tcscpy(iniFilePath, configPath);
	_tcscat(iniFilePath, PLUGINTEMP_INI);
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CloseHandle(::CreateFile(iniFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	}

}

/***
 *	saveSettings()
 *
 *	Saves the parameters of plugin
 */
void saveSettings(void)
{
	TCHAR	temp[16];

	
}

/**************************************************************************
 *	Interface functions
 */

void doAboutDlg(void)
{
	aboutDlg.doDialog();
}

void doPluginManagerDlg()
{
	pluginManagerDlg.doDialog();
}


