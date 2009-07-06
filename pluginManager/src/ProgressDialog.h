#ifndef _PROGRESSDIALOG_H
#define _PROGRESSDIALOG_H

#include <windows.h>
#include <boost/function.hpp>

class ProgressDialog
{
public:
	ProgressDialog(HINSTANCE hInst, boost::function<void(ProgressDialog*)> startFunction);
	~ProgressDialog() {};

	void doModal(HWND parent);

	void stepComplete();
	void setStepProgress(int percentageComplete);
	void setCurrentStatus(const TCHAR* status);
	void setStepCount(int stepCount);

	void close();

	/* Dialog procedures */
	static BOOL CALLBACK dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK runDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	int			_numberOfSteps;
	int			_completedSteps;

	/* Handles */
	HWND		_hProgressOverall;
	HWND		_hProgressCurrent;
	HWND		_hStatus;
	HWND		_hSelf;
	HINSTANCE	_hInst;

	boost::function<void(ProgressDialog*)> _startFunction;

};

#endif