#ifndef _GPUP_PROGRESSDIALOG_H
#define _GPUP_PROGRESSDIALOG_H

class ProgressDialog
{
public:
	ProgressDialog(HINSTANCE hInst);

	void stepComplete();
	void setStatus(const TCHAR* status);

	BOOL CALLBACK runDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void doDialog(int stepCount);

	static BOOL CALLBACK dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


private:
	int _stepCount;
	int _completedSteps;

	HINSTANCE _hInst;
	HWND _hSelf;
	HWND _hProgressBar;
	HWND _hStatus;

	void goToCenter();
};

#endif
