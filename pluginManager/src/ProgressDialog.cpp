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
#include "resource.h"
#include "ProgressDialog.h"
#include "libinstall/CancelToken.h"



ProgressDialog::ProgressDialog(HINSTANCE hInst, CancelToken cancelToken, std::function<void(ProgressDialog*)> startFunction)
    : _hInst(hInst),
      _startFunction(startFunction),
      _hSelf(0),
      _numberOfSteps(0),
      _completedSteps(0),
      _hProgressOverall(0),
      _hProgressCurrent(0),
      _hStatus(0),
      _cancelToken(cancelToken)
{
}



INT_PTR CALLBACK ProgressDialog::runDlgProc(HWND hWnd, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(message)
	{
		case WM_INITDIALOG:
		{
			_hProgressOverall = ::GetDlgItem(hWnd, IDC_PROGRESSOVERALL);
			_hProgressCurrent = ::GetDlgItem(hWnd, IDC_PROGRESSCURRENT);
			_hStatus		  = ::GetDlgItem(hWnd, IDC_LABELCURRENT);
			_hSelf			  = hWnd;
			::SendMessage(_hProgressCurrent, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
			goToCenter();
			_startFunction(this);

			return TRUE;
		}

	}

	return FALSE;
}

INT_PTR CALLBACK ProgressDialog::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
		{
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
			ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(lParam);
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}

        case WM_COMMAND:
            {
                if (IDCANCEL == wParam) {
                    int mbResult = MessageBox(hWnd, _T("Are you sure you wish to abort the current installation/removal?"), _T("Cancel installation / removal?"), MB_YESNO | MB_ICONQUESTION);
                    if (IDYES == mbResult) {
			            ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
                        dlg->_cancelToken.triggerCancel();
                    }
                }
                return FALSE;
            }
		default:
		{
			ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}
	}

}

void ProgressDialog::goToCenter()
{
    RECT rc;
	HWND hParent = ::GetParent(_hSelf);
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


void ProgressDialog::setStepCount(int stepCount)
{
	_numberOfSteps = stepCount;
	if (_hSelf != NULL)
		::SendMessage(_hProgressOverall, PBM_SETRANGE, 0, MAKELPARAM(0, stepCount));
}


void ProgressDialog::stepComplete()
{
	_completedSteps++;
	::SendMessage(_hProgressOverall, PBM_SETPOS, _completedSteps, 0);
}

void ProgressDialog::setStepProgress(int percentageComplete)
{
	::SendMessage(_hProgressCurrent, PBM_SETPOS, percentageComplete, 0);
}

void ProgressDialog::setCurrentStatus(const TCHAR* status)
{
	::SetWindowText(_hStatus, status);
}



void ProgressDialog::doModal(HWND parent)
{
	DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_PROGRESSDIALOG), parent, ProgressDialog::dlgProc, reinterpret_cast<LPARAM>(this));
}

			   
void ProgressDialog::close()
{
	::EndDialog(_hSelf, 0);
}