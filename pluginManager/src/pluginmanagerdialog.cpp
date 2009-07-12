/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009 Dave Brotherstone <davegb@pobox.com>

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
#include <windows.h>
#include <commctrl.h>
#include <process.h>

#pragma warning (push)
#pragma warning (disable : 4512) // assignment operator could not be generated
#include <boost/bind.hpp>
#pragma warning (pop)

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <set>


#include "resource.h"

#include "tinyxml/tinyxml.h"
#include "libinstall/DownloadManager.h"

#include "PluginManagerDialog.h"
#include "Plugin.h"
#include "PluginManager.h"
#include "ProgressDialog.h"
#include "Utility.h"
#include "SettingsDialog.h"
#include "libinstall/md5.h"
#include "WcharMbcsConverter.h"

using namespace std;
using namespace boost;


PluginManagerDialog::PluginManagerDialog()
{
	_pluginList = NULL;
}

void PluginManagerDialog::doDialog()
{
	if (!isCreated())
	{
        create(IDD_PLUGINMANAGER_DLG);

	}

	goToCenter();
}

BOOL CALLBACK PluginManagerDialog::availableTabDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
			::SetWindowLong(hWnd, GWL_USERDATA, lParam);
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(lParam);

			dlg->_tabs[TAB_AVAILABLE].hListView = ::GetDlgItem(hWnd, IDC_LISTAVAILABLE);	
			dlg->_tabs[TAB_AVAILABLE].hDescription = ::GetDlgItem(hWnd, IDC_EDITAVAILABLE);
			dlg->_tabs[TAB_AVAILABLE].nButtons = 1;
			dlg->_tabs[TAB_AVAILABLE].pButtons = new BUTTON[1];
			dlg->_tabs[TAB_AVAILABLE].pButtons[0].hWnd = ::GetDlgItem(hWnd, IDC_BUTTONINSTALL);
			
			WINDOWINFO wiTab;
			::GetWindowInfo(hWnd, &wiTab);

			WINDOWINFO wiObject;
			::GetWindowInfo(dlg->_tabs[TAB_AVAILABLE].pButtons[0].hWnd, &wiObject);
			dlg->_tabs[TAB_AVAILABLE].buttonBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;

			dlg->_tabs[TAB_AVAILABLE].pButtons[0].rightOffset = wiTab.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_AVAILABLE].pButtons[0].width = wiObject.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_AVAILABLE].pButtons[0].height = wiObject.rcClient.bottom - wiObject.rcClient.top;
			
			::GetWindowInfo(dlg->_tabs[TAB_AVAILABLE].hListView, &wiObject);
			dlg->_tabs[TAB_AVAILABLE].listViewBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.bottom;

			dlg->_tabs[TAB_AVAILABLE].leftMargin = wiObject.rcClient.left - wiTab.rcClient.left;;
			dlg->_tabs[TAB_AVAILABLE].rightMargin = wiTab.rcClient.right - wiObject.rcClient.right;
			dlg->_tabs[TAB_AVAILABLE].topMargin = wiObject.rcClient.top - wiTab.rcClient.top;
			
			
			::GetWindowInfo(dlg->_tabs[TAB_AVAILABLE].hDescription, &wiObject);
			dlg->_tabs[TAB_AVAILABLE].descriptionBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;
			dlg->_tabs[TAB_AVAILABLE].descriptionHeight = wiObject.rcClient.bottom - wiObject.rcClient.top;
			
			PluginListView::VERSIONCOLUMN columns[1];
			columns[0] = PluginListView::VERSION_AVAILABLE;
			dlg->_availableListView.init(dlg->_tabs[TAB_AVAILABLE].hListView, dlg->_tabs[TAB_AVAILABLE].hDescription, 1, columns);
			dlg->_availableListView.setMessage(_T("Downloading plugin list..."));
			return TRUE;
		}

		case WM_SIZE:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
			dlg->sizeTab(dlg->_tabs[TAB_AVAILABLE], LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}

		case WM_NOTIFY:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));

			if (((LPNMHDR)lParam)->hwndFrom == dlg->_tabs[TAB_AVAILABLE].hListView)
				return dlg->_availableListView.notify(wParam, lParam);
			else
				return FALSE;
		}

		case WM_COMMAND:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));

			switch(LOWORD(wParam))
			{
				case IDC_BUTTONINSTALL:
				{
					//

					ProgressDialog progress(dlg->_hInst, 
						boost::bind(&PluginList::startInstall, dlg->_pluginList, dlg->_hSelf, _1, &dlg->_availableListView, FALSE));
					progress.doModal(dlg->_hSelf);
					
					//	dlg->installPlugins(dlg->_availableListView);
					break;
				}
			}
			break;
		}

	}
	return FALSE;
}










BOOL CALLBACK PluginManagerDialog::updatesTabDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
			::SetWindowLong(hWnd, GWL_USERDATA, (LONG)lParam);
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(lParam);

			dlg->_tabs[TAB_UPDATES].hListView = ::GetDlgItem(hWnd, IDC_LISTUPDATES);	
			dlg->_tabs[TAB_UPDATES].hDescription = ::GetDlgItem(hWnd, IDC_EDITUPDATE);
			dlg->_tabs[TAB_UPDATES].nButtons = 1;
			dlg->_tabs[TAB_UPDATES].pButtons = new BUTTON[1];
			dlg->_tabs[TAB_UPDATES].pButtons[0].hWnd = ::GetDlgItem(hWnd, IDC_BUTTONUPDATE);
			
			WINDOWINFO wiTab;
			::GetWindowInfo(hWnd, &wiTab);

			WINDOWINFO wiObject;
			::GetWindowInfo(dlg->_tabs[TAB_UPDATES].pButtons[0].hWnd, &wiObject);
			dlg->_tabs[TAB_UPDATES].buttonBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;

			dlg->_tabs[TAB_UPDATES].pButtons[0].rightOffset = wiTab.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_UPDATES].pButtons[0].width = wiObject.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_UPDATES].pButtons[0].height = wiObject.rcClient.bottom - wiObject.rcClient.top;
			
			::GetWindowInfo(dlg->_tabs[TAB_UPDATES].hListView, &wiObject);
			dlg->_tabs[TAB_UPDATES].listViewBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.bottom;

			dlg->_tabs[TAB_UPDATES].leftMargin = wiObject.rcClient.left - wiTab.rcClient.left;;
			dlg->_tabs[TAB_UPDATES].rightMargin = wiTab.rcClient.right - wiObject.rcClient.right;
			dlg->_tabs[TAB_UPDATES].topMargin = wiObject.rcClient.top - wiTab.rcClient.top;
			
			
			::GetWindowInfo(dlg->_tabs[TAB_UPDATES].hDescription, &wiObject);
			dlg->_tabs[TAB_UPDATES].descriptionBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;
			dlg->_tabs[TAB_UPDATES].descriptionHeight = wiObject.rcClient.bottom - wiObject.rcClient.top;
			

			PluginListView::VERSIONCOLUMN columns[2];
			columns[0] = PluginListView::VERSION_INSTALLED;
			columns[1] = PluginListView::VERSION_AVAILABLE;
			dlg->_updatesListView.init(dlg->_tabs[TAB_UPDATES].hListView, dlg->_tabs[TAB_UPDATES].hDescription, 2, columns);
			dlg->_updatesListView.setMessage(_T("Downloading plugin list..."));
			return TRUE;
		}

		case WM_SIZE:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLong(hWnd, GWL_USERDATA));
			dlg->sizeTab(dlg->_tabs[TAB_UPDATES], LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}

		case WM_NOTIFY:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLong(hWnd, GWL_USERDATA));

			if (((LPNMHDR)lParam)->hwndFrom == dlg->_tabs[TAB_UPDATES].hListView)
			{
				dlg->_updatesListView.notify(wParam, lParam);
				return TRUE;
			}
			else
				return FALSE;
		}
		
		case WM_COMMAND:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLong(hWnd, GWL_USERDATA));

			switch(LOWORD(wParam))
			{
				case IDC_BUTTONUPDATE:
				{
					ProgressDialog progress(dlg->_hInst, 
						boost::bind(&PluginList::startInstall, dlg->_pluginList, dlg->_hSelf, _1, &dlg->_updatesListView, TRUE));
					progress.doModal(dlg->_hSelf);
					
					
					break;
				}
			}
			break;
		}
	}
	return FALSE;
}


BOOL CALLBACK PluginManagerDialog::installedTabDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
			::SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)lParam);
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(lParam);

			dlg->_tabs[TAB_INSTALLED].hListView = ::GetDlgItem(hWnd, IDC_LISTINSTALLED);	
			dlg->_tabs[TAB_INSTALLED].hDescription = ::GetDlgItem(hWnd, IDC_EDITINSTALLED);
			dlg->_tabs[TAB_INSTALLED].nButtons = 2;
			dlg->_tabs[TAB_INSTALLED].pButtons = new BUTTON[2];
			dlg->_tabs[TAB_INSTALLED].pButtons[0].hWnd = ::GetDlgItem(hWnd, IDC_BUTTONREMOVE);
			dlg->_tabs[TAB_INSTALLED].pButtons[1].hWnd = ::GetDlgItem(hWnd, IDC_REINSTALL);
			WINDOWINFO wiTab;
			::GetWindowInfo(hWnd, &wiTab);

			WINDOWINFO wiObject;
			::GetWindowInfo(dlg->_tabs[TAB_INSTALLED].pButtons[0].hWnd, &wiObject);
			dlg->_tabs[TAB_INSTALLED].buttonBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;

			dlg->_tabs[TAB_INSTALLED].pButtons[0].rightOffset = wiTab.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_INSTALLED].pButtons[0].width = wiObject.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_INSTALLED].pButtons[0].height = wiObject.rcClient.bottom - wiObject.rcClient.top;


			::GetWindowInfo(dlg->_tabs[TAB_INSTALLED].pButtons[1].hWnd, &wiObject);
			
			dlg->_tabs[TAB_INSTALLED].pButtons[1].rightOffset = wiTab.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_INSTALLED].pButtons[1].width = wiObject.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_INSTALLED].pButtons[1].height = wiObject.rcClient.bottom - wiObject.rcClient.top;
			

			::GetWindowInfo(dlg->_tabs[TAB_INSTALLED].hListView, &wiObject);
			dlg->_tabs[TAB_INSTALLED].listViewBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.bottom;

			dlg->_tabs[TAB_INSTALLED].leftMargin = wiObject.rcClient.left - wiTab.rcClient.left;;
			dlg->_tabs[TAB_INSTALLED].rightMargin = wiTab.rcClient.right - wiObject.rcClient.right;
			dlg->_tabs[TAB_INSTALLED].topMargin = wiObject.rcClient.top - wiTab.rcClient.top;
			
			
			::GetWindowInfo(dlg->_tabs[TAB_INSTALLED].hDescription, &wiObject);
			dlg->_tabs[TAB_INSTALLED].descriptionBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;
			dlg->_tabs[TAB_INSTALLED].descriptionHeight = wiObject.rcClient.bottom - wiObject.rcClient.top;

			// Initialise the list view
			PluginListView::VERSIONCOLUMN columns[1];
			columns[0] = PluginListView::VERSION_INSTALLED;
			dlg->_installedListView.init(dlg->_tabs[TAB_INSTALLED].hListView, dlg->_tabs[TAB_INSTALLED].hDescription, 1, columns);
			dlg->_installedListView.setMessage(_T("Downloading plugin list..."));
			return TRUE;
		}

		case WM_SIZE:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLong(hWnd, GWL_USERDATA));
			dlg->sizeTab(dlg->_tabs[TAB_INSTALLED], LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}

		case WM_NOTIFY:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLong(hWnd, GWL_USERDATA));

			if (((LPNMHDR)lParam)->hwndFrom == dlg->_tabs[TAB_INSTALLED].hListView)
			{
				dlg->_installedListView.notify(wParam, lParam);
				return TRUE;
			}

			break;
		}


		case WM_COMMAND:
		{
			PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog*>(::GetWindowLong(hWnd, GWL_USERDATA));

			switch(LOWORD(wParam))
			{
				case IDC_BUTTONREMOVE:
				{
					ProgressDialog progress(dlg->_hInst, 
						boost::bind(&PluginList::startRemove, dlg->_pluginList, dlg->_hSelf, _1, &dlg->_installedListView));
					progress.doModal(dlg->_hSelf);
					break;
				}

				case IDC_REINSTALL:
				{
					ProgressDialog progress(dlg->_hInst, 
						boost::bind(&PluginList::startInstall, dlg->_pluginList, dlg->_hSelf, _1, &dlg->_installedListView, TRUE));
					progress.doModal(dlg->_hSelf);
					break;
				}
			}
			break;
		}


		


	}
	return FALSE;
}

 
BOOL CALLBACK PluginManagerDialog::tabWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
			
			return TRUE;
		}

		case WM_SIZE:
		{
			
			DLGHDR *dlgHdr = reinterpret_cast<DLGHDR*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));


			dlgHdr->rcDisplay.left = 0;
			dlgHdr->rcDisplay.right = LOWORD(lParam);
			dlgHdr->rcDisplay.top = 0;
			dlgHdr->rcDisplay.bottom = HIWORD(lParam);

			TabCtrl_AdjustRect(hWnd, FALSE, &dlgHdr->rcDisplay);
		
			::SetWindowPos(dlgHdr->hwndDisplay, HWND_TOP, dlgHdr->rcDisplay.left, dlgHdr->rcDisplay.top,
						dlgHdr->rcDisplay.right - dlgHdr->rcDisplay.left,
						dlgHdr->rcDisplay.bottom - dlgHdr->rcDisplay.top, SWP_NOZORDER);
			
			//::SetWindowPos(hWnd, HWND_TOP, 0, 0, HIWORD(lParam), LOWORD(lParam), SWP_NOMOVE | SWP_NOZORDER);
			return TRUE;
			
		}

		default:
			DLGHDR *dlgHdr = reinterpret_cast<DLGHDR*>(::GetWindowLong(hWnd, GWL_USERDATA));
			if (dlgHdr->defWndProc)
				return ::CallWindowProc(dlgHdr->defWndProc, hWnd, Message, wParam, lParam);
			else
				return ::DefWindowProc(hWnd, Message, wParam, lParam);
			break;
	}

}

BOOL CALLBACK PluginManagerDialog::run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	DownloadManager downloadManager;

	switch (Message) 
	{
        case WM_INITDIALOG :
		{
			initTabControl();
			WINDOWINFO wiCtl;
			wiCtl.cbSize = sizeof(WINDOWINFO);
			::GetWindowInfo(_tabHeader.hwndTab, &wiCtl);
			
			WINDOWINFO wiDlg;
			wiDlg.cbSize = sizeof(WINDOWINFO);
			::GetWindowInfo(hWnd, &wiDlg);
			_leftMargin = wiCtl.rcClient.left - wiDlg.rcClient.left;
			_rightMargin = wiDlg.rcClient.right - wiCtl.rcClient.right;
			_topMargin = wiCtl.rcClient.top - wiDlg.rcClient.top;
			_tabBottomOffset = wiDlg.rcClient.bottom - wiCtl.rcClient.bottom;
			_hCloseButton = GetDlgItem(hWnd, IDOK);
			_hSettingsButton = GetDlgItem(hWnd, IDC_SETTINGS);
			::GetWindowInfo(_hCloseButton, &wiCtl);
			_closeButtonRightOffset = wiDlg.rcClient.right - wiCtl.rcClient.left;
			_closeButtonBottomOffset = wiDlg.rcClient.bottom - wiCtl.rcClient.top;
			_closeButtonWidth = wiCtl.rcClient.right - wiCtl.rcClient.left;	
			_closeButtonHeight = wiCtl.rcClient.bottom - wiCtl.rcClient.top;

			_beginthread(downloadAndPopulate, 0, this);
			

			return TRUE;
		}
		case WM_COMMAND : 
		{
			switch (wParam)
			{
				case IDC_SETTINGS:
				{
					SettingsDialog settingsDlg;

					settingsDlg.doModal(_hSelf);
					return TRUE;
				}
				case IDOK :
				case IDCANCEL :
					display(FALSE);
					return TRUE;

				default :
					break;
			}
		}

		case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR) lParam;
			if (lpnmhdr->hwndFrom == GetDlgItem(_hSelf, IDC_PLUGINTABCTRL))
			{
				switch(lpnmhdr->code)
				{
					case TCN_SELCHANGE:
						OnSelChanged(lpnmhdr->hwndFrom);
						break;
				}
			}
			break;
		}

		case WM_SIZE:
			sizeWindow(LOWORD(lParam), HIWORD(lParam));
			break;


	}
	return FALSE;
}

void PluginManagerDialog::OnSelChanged(HWND hwndDlg) 
{ 
    DLGHDR *pHdr = (DLGHDR *) GetWindowLongPtr(hwndDlg, GWL_USERDATA); 
    int iSel = TabCtrl_GetCurSel(pHdr->hwndTab); 
 
    // Destroy the current child dialog box, if any. 
    if (pHdr->hwndDisplay != NULL) 
        SetWindowPos(pHdr->hwndDisplay, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW);
 
	pHdr->hwndDisplay = pHdr->hwndPage[iSel];

	SetWindowPos(pHdr->hwndDisplay, HWND_TOP, 
		    pHdr->rcDisplay.left, 
		    pHdr->rcDisplay.top, 
			pHdr->rcDisplay.right - pHdr->rcDisplay.left, 
			pHdr->rcDisplay.bottom - pHdr->rcDisplay.top, 
			SWP_SHOWWINDOW);
} 


void PluginManagerDialog::initTabControl()
{
	HWND hTabCtrl = GetDlgItem(_hSelf, IDC_PLUGINTABCTRL);
	_tabHeader.hwndTab = hTabCtrl;

	// Create the tab pages
	TCITEM tab;
	tab.pszText = _T("Available");
	tab.mask = TCIF_TEXT;

	TabCtrl_InsertItem(hTabCtrl, 0, &tab);

	tab.pszText = _T("Updates");
	TabCtrl_InsertItem(hTabCtrl, 1, &tab);

	tab.pszText = _T("Installed");
	TabCtrl_InsertItem(hTabCtrl, 2, &tab);

	// Create the child dialogs 
	DLGTEMPLATE *dlg;
	dlg = DoLockDlgRes(MAKEINTRESOURCE(IDD_TABAVAILABLE));
	_tabHeader.hwndDisplay = NULL;
	_tabHeader.hwndPage[0] = CreateDialogIndirectParam(_hInst, 
								dlg, hTabCtrl, PluginManagerDialog::availableTabDlgProc, 
								reinterpret_cast<LPARAM>(this)); 

	dlg = DoLockDlgRes(MAKEINTRESOURCE(IDD_TABUPDATES));
	_tabHeader.hwndPage[1] = CreateDialogIndirectParam(_hInst, 
								dlg, hTabCtrl, PluginManagerDialog::updatesTabDlgProc,
								reinterpret_cast<LPARAM>(this)); 


	dlg = DoLockDlgRes(MAKEINTRESOURCE(IDD_TABINSTALLED));
	_tabHeader.hwndPage[2] = CreateDialogIndirectParam(_hInst, 
								dlg, hTabCtrl, PluginManagerDialog::installedTabDlgProc,
								reinterpret_cast<LPARAM>(this)); 


	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	::GetWindowInfo(hTabCtrl, &wi);
	
	
	
	_tabHeader.rcDisplay.left = 0;
	_tabHeader.rcDisplay.right = wi.rcClient.right - wi.rcClient.left;
	_tabHeader.rcDisplay.top = 0;
	_tabHeader.rcDisplay.bottom = wi.rcClient.bottom - wi.rcClient.top;
	TabCtrl_AdjustRect(hTabCtrl, FALSE, &_tabHeader.rcDisplay);
	// CopyRect(&_tabHeader.rcDisplay, &wi.rcClient);

	// Set the userdata
	::SetWindowLong(hTabCtrl, GWL_USERDATA, reinterpret_cast<LONG>(&_tabHeader));
	
	_tabHeader.defWndProc = reinterpret_cast<WNDPROC>(::GetWindowLong(hTabCtrl, GWL_WNDPROC));
	::SetWindowLong(hTabCtrl, GWL_WNDPROC, reinterpret_cast<LONG>(tabWndProc));
	// Fake a tab change, to show the first tab
	OnSelChanged(hTabCtrl);
}


DLGTEMPLATE* PluginManagerDialog::DoLockDlgRes(LPCTSTR lpszResName) 
{ 
    HRSRC hrsrc = FindResource((HMODULE)g_hModule, lpszResName, RT_DIALOG); 
    HGLOBAL hglb = LoadResource(_hInst, hrsrc); 
    return (DLGTEMPLATE *) LockResource(hglb); 
} 
	
void PluginManagerDialog::sizeWindow(int width, int height)
{
	// Size the tab control
	::MoveWindow(_tabHeader.hwndTab, _leftMargin, _topMargin, width - _leftMargin - _rightMargin, height - _tabBottomOffset, TRUE);
	
	// Move the close button
	::MoveWindow(_hCloseButton, width - _closeButtonRightOffset, height - _closeButtonBottomOffset, _closeButtonWidth, _closeButtonHeight, TRUE);

	// Move the settings button
	::MoveWindow(_hSettingsButton, _leftMargin, height - _closeButtonBottomOffset, _closeButtonWidth, _closeButtonHeight, TRUE);

}


void PluginManagerDialog::sizeTab(TABPAGE tab, int width, int height)
{
	// Size the ListView
	::MoveWindow(tab.hListView, tab.leftMargin, tab.topMargin, 
				width - tab.leftMargin - tab.rightMargin,
				height - tab.topMargin - tab.listViewBottomOffset, TRUE);


	// Size / move the edit box
	::MoveWindow(tab.hDescription, tab.leftMargin, height - tab.descriptionBottomOffset, 
				width - tab.leftMargin - tab.rightMargin,
				tab.descriptionHeight, TRUE);

	// Move the buttons
	for(int bCount = 0; bCount < tab.nButtons; bCount++)
	{
		BUTTON button = tab.pButtons[bCount];
		::MoveWindow(button.hWnd, width - button.rightOffset, height - tab.buttonBottomOffset, 
					button.width, button.height, TRUE);
	}


}





void PluginManagerDialog::downloadAndPopulate(PVOID pvoid)
{
	PluginManagerDialog *dlg = reinterpret_cast<PluginManagerDialog *>(pvoid);

	if (!dlg->_pluginList)
	{
		dlg->_pluginList = new PluginList();
		dlg->_pluginList->init(&dlg->_nppData);
		dlg->_pluginList->downloadList();
	}
	else
	{
		if (!dlg->_pluginList->listsAvailable())
		{
			dlg->_availableListView.setMessage(_T("Still downloading plugin list..."));
			dlg->_updatesListView.setMessage(_T("Still downloading plugin list..."));
			dlg->_installedListView.setMessage(_T("Still downloading plugin list..."));
			dlg->_pluginList->waitForListsAvailable();
		}
	}


	// Show the lists
	PluginListContainer availablePlugins = dlg->_pluginList->getAvailablePlugins();
	if (availablePlugins.empty())
		dlg->_availableListView.setMessage(_T("No new plugins available"));
	else
		dlg->_availableListView.setList(availablePlugins);


	PluginListContainer installedPlugins = dlg->_pluginList->getInstalledPlugins();

	if (installedPlugins.empty())
		dlg->_installedListView.setMessage(_T("There are no known installed plugins"));
	else
		dlg->_installedListView.setList(installedPlugins);
	

	PluginListContainer updatesPlugins = dlg->_pluginList->getUpdateablePlugins();
	
	if (updatesPlugins.empty())
		dlg->_updatesListView.setMessage(_T("There are no plugins with updates available"));
	else
	{
		dlg->_updatesListView.setList(updatesPlugins);
		dlg->_updatesListView.selectAll();
	}

	_endthread();
}


void PluginManagerDialog::init(HINSTANCE hInst, NppData nppData)
{
	_nppData = nppData;
	Window::init(hInst, nppData._nppHandle);	
}

void PluginManagerDialog::setPluginList(PluginList* pluginList)
{
	_pluginList = pluginList;
}