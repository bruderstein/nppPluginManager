/*
This file is part of Plugin Manager Plugin for Notepad++
Copyright (C)2009-2010 Dave Brotherstone <davegb@pobox.com>

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

#include "precompiled_headers.h"

#include "PluginManager.h"
#include "PluginManagerVersion.h"
#include "AboutDialog.h"
#include "PluginManagerDialog.h"
#include "NotifyUpdatesDialog.h"
#include "SettingsDialog.h"
#include "Plugin.h"
#include "Utility.h"
#include "WcharMbcsConverter.h"
#include "libinstall/DownloadManager.h"

/* information for notepad */

#ifdef ALLOW_OVERRIDE_XML_URL
CONST INT   nbFunc  = 3;
#else
CONST INT	nbFunc	= 2;
#endif

CONST TCHAR	PLUGIN_NAME[] = _T("Plugin Manager");

/* global values */
HANDLE				g_hModule			= NULL;
NppData				nppData;
FuncItem			funcItem[nbFunc];

#ifdef _UNICODE
BOOL				g_isUnicode			= TRUE;
#else
BOOL				g_isUnicode			= FALSE;
#endif

#ifdef _WIN64
BOOL				g_isX64 = TRUE;
#else
BOOL				g_isX64 = FALSE;
#endif

Options				g_options;
SettingsDialog		g_settingsDlg;
PluginList          *g_pluginList       = NULL;

winVer				g_winVer;

/* dialog classes */
AboutDialog			aboutDlg;
PluginManagerDialog	pluginManagerDlg;
NotifyUpdatesDialog notifyUpdatesDlg;

/* settings */
TCHAR				configPath[MAX_PATH];
TCHAR				iniFilePath[MAX_PATH];


UINT startupChecks(LPVOID param); 
void doReloadXml();


using namespace std;


/* main function of dll */
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID /*lpReserved*/ )
{
    g_hModule = hModule;
    g_options.moduleInfo.setHModule(static_cast<HMODULE>(hModule));

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
            funcItem[0]._pShKey = NULL;
            funcItem[1]._pShKey = NULL;
#ifdef ALLOW_OVERRIDE_XML_URL
            funcItem[2]._pFunc = doReloadXml;
            _tcscpy_s(funcItem[2]._itemName, 64, _T("&Refresh XML"));
            funcItem[2]._pShKey = NULL;
#endif

            DownloadManager::setUserAgent(_T("Notepad++/Plugin-Manager;v") _T(PLUGINMANAGERVERSION_STRING));
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
    g_options.moduleInfo.setHParent(nppData._nppHandle);
    g_winVer = static_cast<winVer>(::SendMessage(nppData._nppHandle, NPPM_GETWINDOWSVERSION, 0, 0));
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
             Guess what, we do now.
             */
#ifdef ALLOW_OVERRIDE_XML_URL
            ::MessageBox(nppData._nppHandle, _T("The Plugin Manager is running as a special build that allows override of the XML download URL.  This version should be used for testing purposes ONLY."), _T("Plugin Manager"), MB_ICONEXCLAMATION);
#endif
            (void)::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startupChecks, NULL, 0, 0);
            break;

    }
}

/***
 *	messageProc()
 *
 *	This function is called, if a notification from Notepad occurs
 */
extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{

    
    return TRUE;
}


#ifdef _UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return g_isUnicode;
}
#endif



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

    g_options.notifyUpdates = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_NOTIFYUPDATES, 1, iniFilePath);

    g_options.showUnstable = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_SHOWUNSTABLE, 0, iniFilePath);

    g_options.forceHttp = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_FORCEHTTP, 0, iniFilePath);
    g_options.useDevPluginList = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_USEDEVPLUGINLIST, 0, iniFilePath);


    g_options.daysToCheck = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_DAYSTOCHECK, DAYSCHECK_DEFAULT, iniFilePath);
    if (g_options.daysToCheck < DAYSCHECK_MIN)
    {
        g_options.daysToCheck = DAYSCHECK_MIN;
    }

    g_options.installLocation = static_cast<INSTALLLOCATION>(::GetPrivateProfileInt(SETTINGS_GROUP, KEY_INSTALLLOCATION, 2, iniFilePath));
    g_options.appDataPluginsSupported = static_cast<BOOL>(::SendMessage(nppData._nppHandle, NPPM_GETAPPDATAPLUGINSALLOWED, 0, 0));
    // TODO Test



    // If AppData plugins are not supported, then reset the install location.
    if (FALSE == g_options.appDataPluginsSupported)
    {
        if (g_options.installLocation == INSTALLLOC_APPDATA
            || g_options.installLocation == INSTALLLOC_ALLUSERS)
        {
            g_options.installLocation = INSTALLLOC_ALLUSERSNOAPPDATA;
        }
    }
    // Else, if we've "upgraded" to support for all users, assume we want appdata plugins
    else if (TRUE == g_options.appDataPluginsSupported
        && g_options.installLocation == INSTALLLOC_ALLUSERSNOAPPDATA)
    {
        g_options.installLocation = INSTALLLOC_APPDATA;
    }

    TCHAR tmpLastCheck[20];
    ::GetPrivateProfileString(SETTINGS_GROUP, KEY_LASTCHECK, _T("0"), tmpLastCheck, 20, iniFilePath);

    g_options.lastCheck = (time_t)_ttol(tmpLastCheck);

#ifdef ALLOW_OVERRIDE_XML_URL	
    TCHAR tmpUrl[MAX_PATH];
    ::GetPrivateProfileString(SETTINGS_GROUP, KEY_OVERRIDEMD5URL, _T(""), tmpUrl, MAX_PATH, iniFilePath);
    g_options.downloadMD5Url = tmpUrl;

    ::GetPrivateProfileString(SETTINGS_GROUP, KEY_OVERRIDEURL, _T(""), tmpUrl, MAX_PATH, iniFilePath);
    g_options.downloadUrl = tmpUrl;
#endif


}


/***
 *	saveSettings()
 *
 *	Saves the parameters of plugin
 */
void saveSettings(void)
{
    TCHAR	temp[16];

    
    // Install location
    _itot_s(g_options.installLocation, temp, 16, 10);
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_INSTALLLOCATION, temp, iniFilePath);
    
    if (g_options.daysToCheck != DAYSCHECK_DEFAULT)
    {
        _itot_s(g_options.daysToCheck, temp, 16, 10);
        ::WritePrivateProfileString(SETTINGS_GROUP, KEY_DAYSTOCHECK, temp, iniFilePath);
    }


    
    _itot_s(g_options.notifyUpdates, temp, 16, 10);
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_NOTIFYUPDATES, temp, iniFilePath);

    _itot_s(g_options.forceHttp, temp, 16, 10);
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_FORCEHTTP, temp, iniFilePath);

    _itot_s(g_options.useDevPluginList, temp, 16, 10);
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_USEDEVPLUGINLIST, temp, iniFilePath);

    _itot_s(g_options.showUnstable, temp, 16, 10);
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_SHOWUNSTABLE, temp, iniFilePath);

    // Remove any leftover proxy credentials from previous versions 
    // (password encrypted, but they are no longer needed, so clear them out if they're there)
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYUSERNAME, NULL, iniFilePath);
    ::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYPASSWORD, NULL, iniFilePath);

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


    tstring configPathVar = tConfigPath;
    configPathVar.append(_T("\\PluginManagerGpup.xml"));
    TiXmlDocument gpupDoc(configPathVar);
    if (gpupDoc.LoadFile() 
        && gpupDoc.FirstChildElement(_T("install")) 
        && !gpupDoc.FirstChildElement(_T("install"))->NoChildren())
    {
            int mbResult = ::MessageBox(nppData._nppHandle, _T("There are still some pending actions to complete installing or removing some plugins.  Would you like to do these now (Notepad++ will be restarted)?"),
                _T("Notepad++ Plugin Manager"), MB_YESNO | MB_ICONEXCLAMATION);
            if (mbResult == IDYES)
            {
                configPathVar.insert(0, _T("-a \""));
                configPathVar.append(_T("\""));
                Utility::startGpup(nppData._nppHandle, tNppPath, configPathVar.c_str(), TRUE);
            }

    }
    else
    {
        // Clean up temp directory
        tstring tempDir = tConfigPath;
        tempDir.append(_T("\\plugin_install_temp"));
        Utility::removeDirectory(tempDir.c_str());
        
        // Remove the Gpup file (if it exists, doesn't matter if it doesn't)
        ::DeleteFile(configPathVar.c_str());
    }


    time_t ltime;
    time(&ltime);

    if (g_options.notifyUpdates && ((ltime - g_options.lastCheck) > (86400 * g_options.daysToCheck)))
    {
        // Store the check time
        TCHAR tmp[20];
        _ltot_s((long)ltime, tmp, 20, 10);
        ::WritePrivateProfileString(SETTINGS_GROUP, KEY_LASTCHECK, tmp, iniFilePath);

        g_pluginList = new PluginList();
        g_pluginList->init(&nppData);
        pluginManagerDlg.setPluginList(g_pluginList);

        g_pluginList->downloadList();
        if (!g_pluginList->getUpdateablePlugins().empty())
        {
            notifyUpdatesDlg.init((HINSTANCE)g_hModule, nppData, g_pluginList);
            
            // Check if there updates that we are not ignoring
            if (notifyUpdatesDlg.updatesAvailable())
                notifyUpdatesDlg.doModal();
        }
    }


    return 0;
}

#ifdef ALLOW_OVERRIDE_XML_URL
void doReloadXml()
{
    if (NULL == g_pluginList)
    {
        g_pluginList = new PluginList();
        g_pluginList->init(&nppData);
        pluginManagerDlg.setPluginList(g_pluginList);
    }

    g_pluginList->downloadList();

    pluginManagerDlg.refreshLists();
    ::MessageBox(nppData._nppHandle, _T("XML Refreshed"), _T("Plugin Manager"), MB_OK |MB_ICONINFORMATION);

}
#endif