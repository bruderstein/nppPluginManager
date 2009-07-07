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

}
		
