
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
			return TRUE;

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
	::SetWindowTextA(GetDlgItem(_hSelf, IDC_USERNAME), _proxyInfo->getUsername());
	::SetWindowTextA(GetDlgItem(_hSelf, IDC_PASSWORD), _proxyInfo->getPassword());

}

void ProxyCredentialsDlg::setCredentials()
{
	char tmp[MAX_PATH];
	::GetWindowTextA(GetDlgItem(_hSelf, IDC_USERNAME), tmp, MAX_PATH);
	_proxyInfo->setUsername(tmp);
	
	::GetWindowTextA(GetDlgItem(_hSelf, IDC_PASSWORD), tmp, MAX_PATH);
	_proxyInfo->setPassword(tmp);


}
		
