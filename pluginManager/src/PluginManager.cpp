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
CONST INT	nbFunc	= 2;
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



using namespace std;


/* main function of dll */
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID /*lpReserved*/ )
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
			funcItem[0]._pShKey = NULL;
			funcItem[1]._pShKey = NULL;

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
			::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startupChecks, NULL, 0, 0);
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

	TCHAR tmp[MAX_PATH];
	::GetPrivateProfileString(SETTINGS_GROUP, KEY_PROXY, _T(""), tmp, MAX_PATH, iniFilePath);

	g_options.proxyInfo.setProxy(WcharMbcsConverter::tchar2char(tmp).get());

	g_options.proxyInfo.setProxyPort(::GetPrivateProfileInt(SETTINGS_GROUP, KEY_PROXYPORT, 0, iniFilePath));

	::GetPrivateProfileString(SETTINGS_GROUP, KEY_PROXYUSERNAME, _T(""), tmp, MAX_PATH, iniFilePath);
	g_options.proxyInfo.setUsername(WcharMbcsConverter::tchar2char(tmp).get());

	TCHAR encBuffer[1000];
	::GetPrivateProfileString(SETTINGS_GROUP, KEY_PROXYPASSWORD, _T(""), encBuffer, MAX_PATH, iniFilePath);
	
	//decrypt(encBuffer, tmp, MAX_PATH);
	g_options.proxyInfo.setPassword(WcharMbcsConverter::tchar2char(tmp).get());
	

	g_options.notifyUpdates = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_NOTIFYUPDATES, 1, iniFilePath);

	g_options.showUnstable = ::GetPrivateProfileInt(SETTINGS_GROUP, KEY_SHOWUNSTABLE, 0, iniFilePath);

	TCHAR tmpLastCheck[20];
	::GetPrivateProfileString(SETTINGS_GROUP, KEY_LASTCHECK, _T("0"), tmpLastCheck, 20, iniFilePath);

	g_options.lastCheck = (time_t)_ttol(tmpLastCheck);

#ifdef ALLOW_OVERRIDE_XML_URL	
	TCHAR tmpUrl[MAX_PATH];
	::GetPrivateProfileString(SETTINGS_GROUP, KEY_OVERRIDEMD5URL, PLUGINS_MD5_URL, tmpUrl, MAX_PATH, iniFilePath);
	g_options.downloadMD5Url = tmpUrl;

	::GetPrivateProfileString(SETTINGS_GROUP, KEY_OVERRIDEURL, PLUGINS_URL, tmpUrl, MAX_PATH, iniFilePath);
	g_options.downloadUrl = tmpUrl;
#endif


}

void generateKey(unsigned char *buffer, int bufferSize)
{
	HCRYPTPROV hProv;
	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
	CryptGenRandom(hProv, bufferSize, buffer);
	CryptReleaseContext(hProv, NULL);
}

int encryptKey(unsigned const char *keyBuffer, const int keyLength, unsigned char *encKeyBuffer, const int encKeyBufferLength)
{
	int retVal = 0;

	DATA_BLOB pDataOut;
	DATA_BLOB pDataIn;
	pDataIn.cbData = keyLength;
	pDataIn.pbData = static_cast<BYTE *>(const_cast<unsigned char *>(keyBuffer));

	CryptProtectData(&pDataIn, NULL, NULL, NULL, NULL, NULL, &pDataOut);
	if (encKeyBufferLength >= static_cast<int>(pDataOut.cbData))
	{
		memcpy(encKeyBuffer, pDataOut.pbData, pDataOut.cbData);
		retVal = pDataOut.cbData;
	}

	LocalFree(pDataOut.pbData);
	return retVal;
}

bool convertToHex(const unsigned char *source, int sourceLength, TCHAR *hex, int bufferLength)
{
	if (bufferLength < ((sourceLength * 2) + 1))
	{
		return false;
	}

	for (int pos = 0; pos < sourceLength; pos++)
	{
		_stprintf_s((hex + (pos*2)), bufferLength, _T("%02x"), source[pos]);
	}

	hex[sourceLength * 2] = '\0';
	return true;
}

void convertFromHex(const TCHAR *hex, int hexLength, unsigned char *result, int resultLength)
{
	
	for(int pos = 0, resultPos = 0; pos < hexLength && resultPos < resultLength; pos += 2)
	{
		result[resultPos] = ((hex[pos] & 0x0F) + ((hex[pos] & 0x40) ? 9 : 0)) << 4;
		result[resultPos++] |= (hex[pos+1] & 0x0F) + ((hex[pos+1] & 0x40) ? 9 : 0);
	}

}



void decryptKey(const unsigned char *encKeyBuffer, unsigned char *keyBuffer, int keyLength)
{

}

bool getKey(unsigned char *buffer, int bufferSize)
{
	TCHAR encKeyBufferHex[1000];
	unsigned char encKeyBuffer[500];
	unsigned char keyBuffer[16];

	int len = ::GetPrivateProfileString(SETTINGS_GROUP, KEY_KEY, _T(""), encKeyBufferHex, 1000, iniFilePath);
	if (len == 0)
	{
		generateKey(keyBuffer, 16);
		int encryptedSize = encryptKey(keyBuffer, 16, encKeyBuffer, 500);
		convertToHex(encKeyBuffer, encryptedSize, encKeyBufferHex, 1000);
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_KEY, encKeyBufferHex, iniFilePath);
	}
	else
	{
		convertFromHex(encKeyBufferHex, _tcslen(encKeyBufferHex), encKeyBuffer, 500);
		decryptKey(encKeyBuffer, keyBuffer, 16);	
	}

	if (bufferSize >= 16)
	{
		memcpy(buffer, keyBuffer, 16);
		return true;
	}
	else
	{
		return false;
	}
}


bool encrypt(const char *dataIn, TCHAR *output, int inputLength)
{
	//strcpy_s(output, inputLength, static_cast<const char *>(dataIn));
	return false;
}

/***
 *	saveSettings()
 *
 *	Saves the parameters of plugin
 */
void saveSettings(void)
{
	TCHAR	temp[16];

	_itot_s(g_options.proxyInfo.getProxyPort(), temp, 16, 10);
	::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYPORT, temp, iniFilePath);
	const char *proxy = g_options.proxyInfo.getProxy();
	if (proxy && *proxy)
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXY, _T(""), iniFilePath);
	else
	{
		
		std::tr1::shared_ptr<TCHAR> tproxy = WcharMbcsConverter::char2tchar(proxy);
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXY, tproxy.get(), iniFilePath);
	}

	std::tr1::shared_ptr<TCHAR> username = WcharMbcsConverter::char2tchar(g_options.proxyInfo.getUsername());
	::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYUSERNAME, username.get(), iniFilePath);
	
	TCHAR buffer[1000];
	if (encrypt(g_options.proxyInfo.getPassword(), buffer, 1000))
	{
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYPASSWORD, buffer, iniFilePath);
	}

	::WritePrivateProfileString(SETTINGS_GROUP, KEY_PROXYUSERNAME, WcharMbcsConverter::char2tchar(g_options.proxyInfo.getUsername()).get(), iniFilePath);
	_itot_s(g_options.notifyUpdates, temp, 16, 10);
	::WritePrivateProfileString(SETTINGS_GROUP, KEY_NOTIFYUPDATES, temp, iniFilePath);

	_itot_s(g_options.showUnstable, temp, 16, 10);
	::WritePrivateProfileString(SETTINGS_GROUP, KEY_SHOWUNSTABLE, temp, iniFilePath);

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
				Utility::startGpup(nppData._nppHandle, tNppPath, configPath.c_str(), TRUE);
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


	time_t ltime;
	time(&ltime);

	if (g_options.notifyUpdates && ((ltime - g_options.lastCheck) > 259200))
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