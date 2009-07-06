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


#include "stdafx.h"
#include <shlwapi.h>
#include <windows.h>
#include <boost/shared_ptr.hpp>

#include "PluginManager.h"
#include "AboutDialog.h"
#include "PluginManagerDialog.h"
#include "SettingsDialog.h"
#include "Plugin.h"
#include "Utility.h"
#include "WcharMbcsConverter.h"

/* information for notepad */
CONST INT	nbFunc	= 2;
CONST TCHAR	PLUGIN_NAME[] = _T("Plugin Manager");

/* global values */
HANDLE				g_hModule			= NULL;
NppData				nppData;
FuncItem			funcItem[nbFunc];
BOOL				g_isUnicode;
Options				g_options;
SettingsDialog		g_settingsDlg;

/* dialog classes */
AboutDialog			aboutDlg;
PluginManagerDialog	pluginManagerDlg;

/* settings */
TCHAR				configPath[MAX_PATH];
TCHAR				iniFilePath[MAX_PATH];


UINT startupChecks(LPVOID param); 



using namespace std;
using namespace boost;


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
			_tcscpy_s(funcItem[0]._itemName, 64, _T("&Show Plugin Manager"));
			_tcscpy_s(funcItem[1]._itemName, 64, _T("&About"));

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

	switch(notifyCode->nmhdr.code)
	{
		case NPPN_READY:
			/* When Notepad++ is ready, we'll kick off a thread to 
			 * check if there's anything left for gpup to do.
			 * If there is, then we can invite the user if they want to 
			 * do it now.
			 * Eventually, we'll do the plugins.xml download in this new thread
			 * and notify the user of any updates too.
			 */
			::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startupChecks, NULL, 0, 0);
			break;

	}
}

/***
 *	messageProc()
 *
 *	This function is called, if a notification from Notepad occurs
 */
extern "C" __declspec(dllexport) LRESULT messageProc(UINT message, WPARAM wParam, LPARAM lParam)
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

	_tcscpy_s(iniFilePath, MAX_PATH, configPath);
	_tcscat_s(iniFilePath, MAX_PATH, PLUGINMANAGER_INI);
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CloseHandle(::CreateFile(iniFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	}

	TCHAR proxy[MAX_PATH];
	::GetPrivateProfileString(SETTINGS_GROUP, KEY_PROXY, _T(""), proxy, MAX_PATH, iniFilePath);

	g_options.proxy = WcharMbcsConverter::tchar2char(proxy).get();

	g_options.proxyPort = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_PROXYPORT, 0, iniFilePath);

	g_options.notifyUpdates = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_NOTIFYUPDATES, 1, iniFilePath);
	
}

/***
 *	saveSettings()
 *
 *	Saves the parameters of plugin
 */
void saveSettings(void)
{
	TCHAR	temp[16];

	_itot_s(g_options.proxyPort, temp, 16, 10);
	::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYPORT, temp, iniFilePath);
	if (g_options.proxy.empty())
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXY, _T(""), iniFilePath);
	else
	{
		
		shared_ptr<TCHAR> tproxy = WcharMbcsConverter::char2tchar(g_options.proxy.c_str());
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXY, tproxy.get(), iniFilePath);
	}

	_itot_s(g_options.notifyUpdates, temp, 16, 10);
	::WritePrivateProfileString(SETTINGS_GROUP, KEY_NOTIFYUPDATES, temp, iniFilePath);

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



UINT startupChecks(LPVOID /*param*/)
{
	TCHAR tConfigPath[MAX_PATH];
	TCHAR tNppPath[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(tConfigPath));
	::SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(tNppPath));


	tstring configPath = tConfigPath;
	configPath.append(_T("\\PluginManagerGpup.xml"));
	TiXmlDocument gpupDoc(configPath);
	if (gpupDoc.LoadFile() 
		&& gpupDoc.FirstChildElement(_T("install")) 
		&& !gpupDoc.FirstChildElement(_T("install"))->NoChildren())
	{
			int mbResult = ::MessageBox(nppData._nppHandle, _T("There are still some pending actions to complete installing or removing some plugins.  Would you like to do these now (Notepad++ will be restarted)?"),
				_T("Notepad++ Plugin Manager"), MB_YESNO | MB_ICONEXCLAMATION);
			if (mbResult == IDYES)
			{
				configPath.insert(0, _T("-a \""));
				configPath.append(_T("\""));
				Utility::startGpup(tNppPath, configPath.c_str());
			}

	}
	else
	{
		// Clean up temp directory
		tstring tempDir = tConfigPath;
		tempDir.append(_T("\\plugin_install_temp"));
		Utility::removeDirectory(tempDir.c_str());
		
		// Remove the Gpup file (if it exists, doesn't matter if it doesn't)
		::DeleteFile(configPath.c_str());
	}

	return 0;
}