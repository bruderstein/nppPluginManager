#include "stdafx.h"
#include "ProgressDialog.h"
#include "Resource.h"



ProgressDialog::ProgressDialog(HINSTANCE hInst)
{
	
	_completedSteps = 0;
	_hInst = hInst;
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_PROGRESS_CLASS || ICC_LINK_CLASS;

	InitCommonControlsEx(&icc);
	_hSelf = ::CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_PROGRESSDIALOG), NULL, (DLGPROC)dlgProc, reinterpret_cast<LPARAM>(this));
	if (_hSelf == NULL)
	{
		DWORD err = ::GetLastError();
		err++;
	}
}

void ProgressDialog::goToCenter()
{
    RECT rc;
	HWND hParent = ::GetDesktopWindow();
	::GetClientRect(hParent, &rc);
    POINT center;
    center.x = rc.left + (rc.right - rc.left)/2;
    center.y = rc.top + (rc.bottom - rc.top)/2;
    ::ClientToScreen(hParent, &center);
	
	RECT wrc;
	::GetWindowRect(_hSelf, &wrc);
	int x = center.x - (wrc.right - wrc.left)/2;
	int y = center.y - (wrc.bottom - wrc.top)/2;

	::SetWindowPos(_hSelf, HWND_TOP, x, y, wrc.right - wrc.left, wrc.bottom - wrc.top, SWP_SHOWWINDOW);
}


void ProgressDialog::doDialog()
{
	goToCenter();
}

void ProgressDialog::close()
{
    CloseWindow(_hSelf);
}

void ProgressDialog::setStepCount(int stepCount)
{
	_stepCount = stepCount;
	::SendMessage(_hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, _stepCount));	
}


BOOL CALLBACK ProgressDialog::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
		{
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
			ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(lParam);
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}
		default:
		{
			ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(static_cast<LONG_PTR>(::GetWindowLongPtr(hWnd, GWLP_USERDATA)));
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}
	}

}

BOOL CALLBACK ProgressDialog::runDlgProc(HWND hWnd, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(message)
	{
		case WM_INITDIALOG:
			_hSelf = hWnd;
			_hProgressBar = ::GetDlgItem(hWnd, IDC_PROGRESSBAR);
			_hStatus = ::GetDlgItem(hWnd, IDC_CURRENTSTATUS);
			return TRUE;
	}

	return FALSE;
}




void ProgressDialog::stepComplete()
{
	_completedSteps++;
	::SendMessage(_hProgressBar, PBM_SETPOS, _completedSteps, 0);
}


void ProgressDialog::setStatus(const TCHAR* status)
{
	::SetWindowText(_hStatus, status);
}
