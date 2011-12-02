
#include "precompiled_headers.h"
#include "libinstall/ProxyInfo.h"
#include "libinstall/ModuleInfo.h"
#include "libinstall/ProxyCredentialsDlg.h"

#include "resource.h"

BOOL ProxyCredentialsDlg::getCredentials(const ModuleInfo* moduleInfo, ProxyInfo* proxyInfo)
{
	_proxyInfo = proxyInfo;
	return static_cast<BOOL>(::DialogBoxParam((HINSTANCE)moduleInfo->getHModule(), MAKEINTRESOURCE(IDD_CREDENTIALS), moduleInfo->getHParent(), ProxyCredentialsDlg::dlgProc, reinterpret_cast<LPARAM>(this)));
}

BOOL ProxyCredentialsDlg::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
			return reinterpret_cast<ProxyCredentialsDlg*>(lParam)->run_dlgProc(hWnd, message, wParam, lParam);

		default:
		{
			ProxyCredentialsDlg* dlg = reinterpret_cast<ProxyCredentialsDlg*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
			if (dlg)
				return dlg->run_dlgProc(hWnd, message, wParam, lParam);
			else
				return FALSE;
		}
	}
	
}




BOOL ProxyCredentialsDlg::run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch(message)
	{
		case WM_INITDIALOG:
			_hSelf = hWnd;
			initialiseCredentials();
			return FALSE;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
					setCredentials();
					::EndDialog(hWnd, TRUE);
					break;

				case IDCANCEL:
					::EndDialog(hWnd, FALSE);
					break;
			}
			return TRUE;
		}

		default:
			return FALSE;
	}



}


void ProxyCredentialsDlg::initialiseCredentials()
{
	const char *username = _proxyInfo->getUsername();
	const char *password = _proxyInfo->getPassword();

	
	::SetWindowTextA(GetDlgItem(_hSelf, IDC_USERNAME), username);

	::SetWindowTextA(GetDlgItem(_hSelf, IDC_PASSWORD), password);
	if (username && *username)
	{
		Edit_SetSel(GetDlgItem(_hSelf, IDC_PASSWORD), 0, strlen((const char *)password));
		::SetFocus(GetDlgItem(_hSelf, IDC_PASSWORD));
	}
	else
	{
		::SetFocus(GetDlgItem(_hSelf, IDC_USERNAME));
	}

	if (_proxyInfo->getSaveCredentials() == SAVECRED_UNKNOWN
		|| _proxyInfo->getSaveCredentials() == SAVECRED_YES)
	{
		::SendMessage(GetDlgItem(_hSelf, IDC_SAVECREDS), BM_SETCHECK, BST_CHECKED, 0);
	}
	else
	{
		::SendMessage(GetDlgItem(_hSelf, IDC_SAVECREDS), BM_SETCHECK, BST_UNCHECKED, 0);
	}
}

void ProxyCredentialsDlg::setCredentials()
{
	char tmp[MAX_PATH];
	::GetWindowTextA(GetDlgItem(_hSelf, IDC_USERNAME), tmp, MAX_PATH);
	_proxyInfo->setUsername(tmp);
	
	::GetWindowTextA(GetDlgItem(_hSelf, IDC_PASSWORD), tmp, MAX_PATH);
	_proxyInfo->setPassword(tmp);

	LRESULT result = ::SendMessage(GetDlgItem(_hSelf, IDC_SAVECREDS), BM_GETCHECK, 0, 0);
	if (result == BST_CHECKED)
	{
		_proxyInfo->setSaveCredentials(SAVECRED_YES);
	}
	else
	{
		_proxyInfo->setSaveCredentials(SAVECRED_NO);
	}

}
		
