#pragma once


#include "StaticDialog.h"
#include "resource.h"
#include "PluginManager.h"
#include "PluginList.h"
#include "PluginListView.h"
#include "ProgressDialog.h"

#define PATHENV_MAX_LENGTH  8191
#define TAB_PAGE_COUNT      3

struct POSITIONINFO {
    HWND handle;
    int bottomOffset;
    int leftOffset;
    int height;
    int width;
};


class PluginManagerDialog : StaticDialog
{
public:
	PluginManagerDialog();
	~PluginManagerDialog() {
	    _bottomComponents.clear();
	};

    void init(HINSTANCE hInst, NppData nppData);
	void setPluginList(PluginList* pluginList);

   	void doDialog();

    virtual void destroy() {
        
    };

	static INT_PTR CALLBACK tabWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK availableTabDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK updatesTabDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK installedTabDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void setStatus(const TCHAR* status);
	void setStepProgress(const int percentageComplete);
	void setStepComplete();

	void refreshLists();

protected :
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	/* Handles */
	NppData			_nppData;
    HWND			_HSource;
	HWND			_hCloseButton;
	HWND			_hSettingsButton;
    HWND            _hNbcLogo;

	/* List of plugins, both available and installed*/
	PluginList*		_pluginList;

	/* ListView controller objects */
	PluginListView  _installedListView;
	PluginListView  _availableListView;
	PluginListView  _updatesListView;


	/* Tab control data header */
	typedef struct tag_dlghdr {
		HWND hwndTab;
		HWND hwndDisplay;
		RECT rcDisplay;
		HWND hwndPage[TAB_PAGE_COUNT];
		WNDPROC defWndProc;

	} DLGHDR;
	DLGHDR			_tabHeader;


	static const int TAB_AVAILABLE = 0;
	static const int TAB_UPDATES   = 1;
    static const int TAB_INSTALLED = 2;

	typedef struct button {
		HWND hWnd;
		int  rightOffset;
		int width, height;
	} BUTTON;

	typedef struct tabPage {
		int topMargin, leftMargin, rightMargin;
		HWND hListView;
		HWND hDescription;
		int  descriptionBottomOffset;
		int  descriptionHeight;
		int  listViewBottomOffset;
		int  buttonBottomOffset;
		int  nButtons;
		BUTTON *pButtons;
	} TABPAGE;

	TABPAGE _tabs[3];

	/* Size and offset info */
	int _leftMargin, _rightMargin;
	int _topMargin, _bottomMargin;

	int _tabBottomOffset;
	int _closeButtonBottomOffset, _closeButtonRightOffset;
	int _closeButtonWidth, _closeButtonHeight;

    bool _isDownloading;
    uintptr_t _downloadThread;
    std::list<std::shared_ptr<POSITIONINFO>> _bottomComponents;


	/* Private methods */

	/* Initialisers */
	void initTabControl();

	/* GUI handlers */
	void OnSelChanged(HWND hwndDlg);
	void sizeTab(TABPAGE tab, int width, int height);
	void sizeWindow(int width, int height);
	DLGTEMPLATE* DoLockDlgRes(LPCTSTR lpszResName);

	/* Threaded proc to download the plugin list and populate the views */
	static void downloadAndPopulate(PVOID pvoid);
	static void refreshDownload(PVOID pvoid);
	static void populateLists(PluginManagerDialog* dlg);

	/* Threaded procs to install plugins from a given list */
	static UINT installThreadProc(LPVOID param);

	/* Installs plugins from given list */ 
	void startInstall(ProgressDialog* progressDialog, PluginListView *pluginListView, BOOL isUpdate);

    void addBottomComponent(HWND hWnd, WINDOWINFO& wiDlg, UINT id);

};
