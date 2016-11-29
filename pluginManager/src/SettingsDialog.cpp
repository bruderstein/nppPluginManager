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
#include "SettingsDialog.h"
#include "resource.h"

void SettingsDialog::doModal(NppData *nppData, HWND parent)
{
	_nppData = nppData;
	::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_CONFIGDIALOG), parent, SettingsDialog::dlgProc, reinterpret_cast<LPARAM>(this));
}

INT_PTR SettingsDialog::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
			return reinterpret_cast<SettingsDialog*>(lParam)->run_dlgProc(hWnd, message, wParam, lParam);

		default:
		{
			SettingsDialog* dlg = reinterpret_cast<SettingsDialog*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (dlg)
				return dlg->run_dlgProc(hWnd, message, wParam, lParam);
			else
				return FALSE;
		}
	}
	
}




INT_PTR SettingsDialog::run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch(message)
	{
		case WM_INITDIALOG:
			_hSelf = hWnd;
			initialiseOptions();
			return TRUE;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
					setOptions();


				case IDCANCEL:
					::EndDialog(hWnd, 0);
			}
			return TRUE;
		}

		default:
			return FALSE;
	}



}


void SettingsDialog::initialiseOptions()
{
	char tmp[15];

	::SendMessage(GetDlgItem(_hSelf, IDC_NOTIFY), BM_SETCHECK, g_options.notifyUpdates ? BST_CHECKED : BST_UNCHECKED, 0);
	
	::SendMessage(GetDlgItem(_hSelf, IDC_SHOWUNSTABLE), BM_SETCHECK, g_options.showUnstable ? BST_CHECKED : BST_UNCHECKED, 0);

	
	_ltoa_s(g_options.daysToCheck, tmp, 15, 10);
	::SetWindowTextA(GetDlgItem(_hSelf, IDC_DAYSTOCHECK), tmp);

	::SendMessage(GetDlgItem(_hSelf, IDC_INSTALLALLUSERS), BM_SETCHECK, g_options.installLocation == INSTALLLOC_APPDATA ? BST_UNCHECKED : BST_CHECKED, 0);
	::SendMessage(GetDlgItem(_hSelf, IDC_FORCEHTTP), BM_SETCHECK, g_options.forceHttp != TRUE ? BST_UNCHECKED : BST_CHECKED, 0);
	::SendMessage(GetDlgItem(_hSelf, IDC_USEDEVPLUGINLIST), BM_SETCHECK, g_options.useDevPluginList != TRUE ? BST_UNCHECKED : BST_CHECKED, 0);

	::EnableWindow(GetDlgItem(_hSelf, IDC_INSTALLALLUSERS), g_options.appDataPluginsSupported);

	tstring info(_T("Plugin Config path is:\r\n"));
	TCHAR path[MAX_PATH];
	path[0] = _T('\0');
	::SendMessage(_nppData->_nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(path));
	info.append(path);
	info.append(_T("\r\n"));
	if (g_options.appDataPluginsSupported)
	{
		info.append(_T("Plugins in user's AppData directory are enabled. Remove the allowAppDataPlugins.xml file from the Notepad++ directory to disable."));
	}
	else
	{
		info.append(_T("Plugins in user's AppData directory are disabled - to enable in Notepad++ version 5.9.7 onwards place an empty file called allowAppDataPlugins.xml in the Notepad++ directory."));
	}
	
	SetWindowText(GetDlgItem(_hSelf, IDC_INFOTEXT), info.c_str());

}

void SettingsDialog::setOptions()
{
	char address[MAX_PATH];
	
	::GetWindowTextA(GetDlgItem(_hSelf, IDC_DAYSTOCHECK), address, MAX_PATH);
	g_options.daysToCheck = atol(address);

	LRESULT result = ::SendMessage(GetDlgItem(_hSelf, IDC_NOTIFY), BM_GETCHECK, 0, 0);
	if (BST_CHECKED == result)
		g_options.notifyUpdates = TRUE;
	else
		g_options.notifyUpdates = FALSE;

	result = ::SendMessage(GetDlgItem(_hSelf, IDC_SHOWUNSTABLE), BM_GETCHECK, 0, 0);
	if (BST_CHECKED == result)
		g_options.showUnstable = TRUE;
	else
		g_options.showUnstable = FALSE;

	result = ::SendMessage(GetDlgItem(_hSelf, IDC_FORCEHTTP), BM_GETCHECK, 0, 0);
	if (BST_CHECKED == result)
		g_options.forceHttp = TRUE;
	else
		g_options.forceHttp = FALSE;

	result = ::SendMessage(GetDlgItem(_hSelf, IDC_USEDEVPLUGINLIST), BM_GETCHECK, 0, 0);
	if (BST_CHECKED == result) {
		g_options.useDevPluginList = TRUE;
	} else {
		g_options.useDevPluginList = FALSE;
	}

	if (g_options.appDataPluginsSupported)
	{
		result = ::SendMessage(GetDlgItem(_hSelf, IDC_INSTALLALLUSERS), BM_GETCHECK, 0, 0);
		if (BST_CHECKED == result)
			g_options.installLocation = INSTALLLOC_ALLUSERS;
		else
			g_options.installLocation = INSTALLLOC_APPDATA;
	}
	else
	{
		g_options.installLocation = INSTALLLOC_ALLUSERSNOAPPDATA;
	}

}
		
