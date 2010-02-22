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

#include <windows.h>

#include "PluginManager.h"
#include "SettingsDialog.h"
#include "resource.h"

void SettingsDialog::doModal(HWND parent)
{
	::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_CONFIGDIALOG), parent, SettingsDialog::dlgProc, reinterpret_cast<LPARAM>(this));
}

BOOL SettingsDialog::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			::SetWindowLongPtr(hWnd, GWL_USERDATA, lParam);
			return reinterpret_cast<SettingsDialog*>(lParam)->run_dlgProc(hWnd, message, wParam, lParam);

		default:
		{
			SettingsDialog* dlg = reinterpret_cast<SettingsDialog*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
			if (dlg)
				return dlg->run_dlgProc(hWnd, message, wParam, lParam);
			else
				return FALSE;
		}
	}
	
}




BOOL SettingsDialog::run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM /*lParam*/)
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
	::SetWindowTextA(GetDlgItem(_hSelf, IDC_PROXYADDRESS), g_options.proxy.c_str());
	char tmp[15];

	if (g_options.proxyPort != 0) 
	{
		_ltoa_s(g_options.proxyPort, tmp, 15, 10);
		::SetWindowTextA(GetDlgItem(_hSelf, IDC_PROXYPORT), tmp);
	}
	else
		::SetWindowTextA(GetDlgItem(_hSelf, IDC_PROXYPORT), "");	

	::SendMessage(GetDlgItem(_hSelf, IDC_NOTIFY), BM_SETCHECK, g_options.notifyUpdates ? BST_CHECKED : BST_UNCHECKED, 0);

	::SendMessage(GetDlgItem(_hSelf, IDC_SHOWUNSTABLE), BM_SETCHECK, g_options.showUnstable ? BST_CHECKED : BST_UNCHECKED, 0);

}

void SettingsDialog::setOptions()
{
	char address[MAX_PATH];
	::GetWindowTextA(GetDlgItem(_hSelf, IDC_PROXYADDRESS), address, MAX_PATH);
	g_options.proxy = address;

	::GetWindowTextA(GetDlgItem(_hSelf, IDC_PROXYPORT), address, MAX_PATH);
	g_options.proxyPort = atol(address);
	
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

}
		
