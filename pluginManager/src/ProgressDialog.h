#ifndef _PROGRESSDIALOG_H
#define _PROGRESSDIALOG_H

#include "libinstall/CancelToken.h"

class ProgressDialog
{
public:
    ProgressDialog(HINSTANCE hInst, CancelToken cancelToken, std::function<void(ProgressDialog*)> startFunction);
    ~ProgressDialog() {};

    void doModal(HWND parent);

    void stepComplete();
    void setStepProgress(int percentageComplete);
    void setCurrentStatus(const TCHAR* status);
    void setStepCount(int stepCount);

    void close();

    /* Dialog procedures */
    static INT_PTR CALLBACK dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR CALLBACK runDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    int			_numberOfSteps;
    int			_completedSteps;

    /* Handles */
    HWND		_hProgressOverall;
    HWND		_hProgressCurrent;
    HWND		_hStatus;
    HWND		_hSelf;
    HINSTANCE	_hInst;
    CancelToken _cancelToken;

    std::function<void(ProgressDialog*)> _startFunction;


    void goToCenter();
};

#endif