#include <windows.h>
#include <commctrl.h>
#include <boost/function.hpp>
#include "ProgressDialog.h"
#include "resource.h"

using namespace boost;

ProgressDialog::ProgressDialog(HINSTANCE hInst, function<void(ProgressDialog*)> startFunction)
{
	_hInst = hInst;
	_startFunction = startFunction;
	_hSelf = 0;
	_completedSteps = 0;
}



BOOL CALLBACK ProgressDialog::runDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			
			_startFunction(this);

			return TRUE;
		}

	}

	return FALSE;
}

BOOL CALLBACK ProgressDialog::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
		{
			::SetWindowLongPtr(hWnd, GWL_USERDATA, lParam);
			ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(lParam);
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}
		default:
		{
			ProgressDialog* dlg = reinterpret_cast<ProgressDialog*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}
	}

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