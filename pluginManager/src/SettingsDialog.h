#ifndef _SETTINGSDIALOG_H
#define _SETTINGSDIALOG_H

struct NppData;



class SettingsDialog
{

public:
	SettingsDialog(): _nppData(nullptr), _hSelf(nullptr) {};
	~SettingsDialog() {};    
    
	void doModal(NppData *nppData, HWND parent);
   	
    

protected :
	

private:
	
	static INT_PTR CALLBACK dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void initialiseOptions();
	void setOptions();

	NppData *_nppData;
	HWND   _hSelf;
};



#endif // _SETTINGSDIALOG_H