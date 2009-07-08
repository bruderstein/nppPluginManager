
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


void PluginManagerDialog::doDialog()
{
	if (!isCreated())
	{
        create(IDD_PLUGINMANAGER_DLG);
		//_pluginListView.init(GetDlgItem(_hSelf, IDC_PLUGINLIST));
//		setupTabControl();
		
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
						boost::bind(&PluginManagerDialog::startInstall, dlg, _1, &dlg->_availableListView, FALSE));
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

struct InstallParam
{
	PluginManagerDialog* pluginManagerDialog;
	PluginListView*      pluginListView;
	ProgressDialog*		 progressDialog;
	BOOL                 isUpdate;
};

void PluginManagerDialog::startInstall(ProgressDialog* progressDialog, PluginListView *pluginListView, BOOL isUpdate)
{
	InstallParam *ip = new InstallParam;
	ip->pluginListView = pluginListView;
	ip->progressDialog = progressDialog;
	ip->pluginManagerDialog = this;
	ip->isUpdate = isUpdate;

	::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PluginManagerDialog::installThreadProc, (LPVOID)ip, 0, 0);
}


UINT PluginManagerDialog::installThreadProc(LPVOID param)
{
	InstallParam *ip = reinterpret_cast<InstallParam*>(param);
	
	ip->pluginManagerDialog->installPlugins(ip->progressDialog, ip->pluginListView, ip->isUpdate);

	// clean up the parameter
	delete ip;

	return 0;
}





void PluginManagerDialog::installPlugins(ProgressDialog* progressDialog, PluginListView* pluginListView, BOOL isUpgrade)
{
	
	tstring configDir = _pluginList.getVariableHandler()->getConfigDir();
	
	tstring basePath(configDir);

	basePath.append(_T("\\plugin_install_temp"));

	// Create the temp directory if it doesn't exist already
	::CreateDirectory(basePath.c_str(), NULL);
	basePath.append(_T("\\plugin"));
	


	TiXmlDocument* forGpupDoc = new TiXmlDocument();
	TiXmlElement* installElement = new TiXmlElement(_T("install"));
	forGpupDoc->LinkEndChild(installElement);
	
	shared_ptr< list<Plugin*> > selectedPlugins = pluginListView->getSelectedPlugins();
	
	shared_ptr< list<tstring> > installDueToDepends = _pluginList.calculateDependencies(selectedPlugins);
		
	if (!installDueToDepends->empty())
	{
		tstring dependsMessage = _T("The following plugin");
		if (installDueToDepends->size() > 1)
			dependsMessage.append(_T("s"));

		dependsMessage.append(_T(" need to be installed to support your selection.\r\n\r\n"));
		for(list<tstring>::iterator msgIter = installDueToDepends->begin(); msgIter != installDueToDepends->end(); msgIter++)
		{
			dependsMessage.append(*msgIter);
			dependsMessage.append(_T("\r\n"));
		}

		dependsMessage.append(_T("\r\nThey will be installed automatically."));


		::MessageBox(_hSelf, dependsMessage.c_str(), _T("Plugin Manager"), MB_OK | MB_ICONINFORMATION);

	}

	

	size_t installSteps = 0;
	list<Plugin*>::iterator pluginIter = selectedPlugins->begin();
	while(pluginIter != selectedPlugins->end())
	{
		installSteps += (*pluginIter)->getInstallStepCount();
		++pluginIter;
	}

	progressDialog->setStepCount(installSteps);

	pluginIter = selectedPlugins->begin();


	tstring pluginTemp;
	int pluginCount = 1;
	
	BOOL needRestart = FALSE;
	BOOL somethingInstalled = FALSE;

	TCHAR pluginCountChar[10];

	while(pluginIter != selectedPlugins->end())
	{
		BOOL directoryCreated = FALSE;
		do 
		{
			pluginTemp = basePath;
			_itot_s(pluginCount, pluginCountChar, 10, 10);
			pluginTemp.append(pluginCountChar);
			directoryCreated = ::CreateDirectory(pluginTemp.c_str(), NULL);
			++pluginCount;
		} while(!directoryCreated);

		pluginTemp.append(_T("\\"));
		


		if (isUpgrade)
		{
			/* Remove the existing file if is an upgrade
			 * This will be done in gpup, but the copy will come afterwards, also in gpup
			 * So, if the filename is the same as the existing plugin, gpup will delete the old
			 * file, then copy in the new.
			 * If the filename is different, the new one will be copied in now, then
			 * the old file will be deleted in gpup.  This is why it is important that
			 * replace="false" (default) on the actual plugin file copy step
			 */

			TiXmlElement* removeElement = new TiXmlElement(_T("delete"));
			removeElement->SetAttribute(_T("file"), (*pluginIter)->getFilename().c_str());
			installElement->LinkEndChild(removeElement);
		}

		InstallStatus status = (*pluginIter)->install(pluginTemp, installElement, 
			boost::bind(&ProgressDialog::setCurrentStatus, progressDialog, _1),
			boost::bind(&ProgressDialog::setStepProgress, progressDialog, _1),
			boost::bind(&ProgressDialog::stepComplete, progressDialog));

		switch(status)
		{
			case INSTALL_SUCCESS:
				Utility::removeDirectory(pluginTemp.c_str());
				somethingInstalled = TRUE;
				break;

			case INSTALL_NEEDRESTART:
				needRestart = TRUE;
				somethingInstalled = TRUE;
				break;

			case INSTALL_FAIL:
			{
				tstring message (_T("Installation of "));
				message.append((*pluginIter)->getName());
				message.append(_T(" failed."));

				::MessageBox(_hSelf, message.c_str(), _T("Installation Error"), MB_OK | MB_ICONERROR);
				Utility::removeDirectory(pluginTemp.c_str());
				break;
			}

		}

		++pluginIter;
	}
	
	
	progressDialog->close(); 


	if (needRestart)
	{
		tstring gpupFile(configDir);
		gpupFile.append(_T("\\PluginManagerGpup.xml"));

		forGpupDoc->SaveFile(gpupFile.c_str());
		delete forGpupDoc;

		int restartNow = ::MessageBox(_hSelf, _T("Some installation steps still need to be completed.  Notepad++ needs to be restarted in order to complete these steps.  If you restart later, the steps will not be completed.  Would you like to restart now?"), _T("Plugin Manager"), MB_YESNO | MB_ICONINFORMATION);
		if (restartNow == IDYES)
		{
			
			tstring gpupArguments(_T("-a \""));
			gpupArguments.append(gpupFile);
			gpupArguments.append(_T("\""));

			Utility::startGpup(_pluginList.getVariableHandler()->getNppDir().c_str(), gpupArguments.c_str());
		}
	}
	else if (somethingInstalled)
	{
		delete forGpupDoc;

		int restartNow = ::MessageBox(_hSelf, _T("Notepad++ needs to be restarted for changes to take effect.  Would you like to do this now?"), _T("Plugin Manager"), MB_YESNO | MB_ICONINFORMATION);
		if (restartNow == IDYES)
		{
			Utility::startGpup(_pluginList.getVariableHandler()->getNppDir().c_str(), _T(""));
		}
	}
	else
	{
		delete forGpupDoc;
	}
	
	
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
						boost::bind(&PluginManagerDialog::startInstall, dlg, _1, &dlg->_updatesListView, TRUE));
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
			dlg->_tabs[TAB_INSTALLED].nButtons = 1;
			dlg->_tabs[TAB_INSTALLED].pButtons = new BUTTON[1];
			dlg->_tabs[TAB_INSTALLED].pButtons[0].hWnd = ::GetDlgItem(hWnd, IDC_BUTTONREMOVE);
			
			WINDOWINFO wiTab;
			::GetWindowInfo(hWnd, &wiTab);

			WINDOWINFO wiObject;
			::GetWindowInfo(dlg->_tabs[TAB_INSTALLED].pButtons[0].hWnd, &wiObject);
			dlg->_tabs[TAB_INSTALLED].buttonBottomOffset = wiTab.rcClient.bottom - wiObject.rcClient.top;

			dlg->_tabs[TAB_INSTALLED].pButtons[0].rightOffset = wiTab.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_INSTALLED].pButtons[0].width = wiObject.rcClient.right - wiObject.rcClient.left;
			dlg->_tabs[TAB_INSTALLED].pButtons[0].height = wiObject.rcClient.bottom - wiObject.rcClient.top;
			
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
					break;
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

	/*LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 30;
	lvi.pszText = _T("Please wait while plugin list is downloaded...");
	lvi.iItem = 0;
	lvi.iSubItem = 0;
    ListView_InsertItem(dlg->_tabs[TAB_AVAILABLE].hListView, &lvi);
	ListView_InsertItem(dlg->_tabs[TAB_UPDATES].hListView, &lvi);
	ListView_InsertItem(dlg->_tabs[TAB_INSTALLED].hListView, &lvi);
*/
	// Work out the path of the Plugins.xml destination (in config dir)
	TCHAR pluginConfig[MAX_PATH];
	::SendMessage(dlg->_nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH - 26, reinterpret_cast<LPARAM>(pluginConfig));
	
	tstring pluginsListFilename(pluginConfig);
	pluginsListFilename.append(_T("\\PluginManagerPlugins.xml"));
		
	

	// Download the plugins.xml from the repository
	DownloadManager downloadManager;
	tstring contentType;
	TCHAR hashBuffer[(MD5LEN * 2) + 1];
	MD5::hash(pluginsListFilename.c_str(), hashBuffer, (MD5LEN * 2) + 1);
	string serverMD5;
	BOOL downloadResult = downloadManager.getUrl(_T("http://localhost:100/plugins.md5.txt"), serverMD5, g_options.proxy.c_str(), g_options.proxyPort);
	shared_ptr<char> cHashBuffer = WcharMbcsConverter::tchar2char(hashBuffer);
	if (downloadResult && serverMD5 != cHashBuffer.get())
		downloadManager.getUrl(_T("http://localhost:100/plugins.xml"), pluginsListFilename, contentType, g_options.proxy.c_str(), g_options.proxyPort);
	

	dlg->_pluginList.init(&dlg->_nppData);	
	// Parse it
	dlg->_pluginList.parsePluginFile(pluginsListFilename.c_str());
	
	// Check for what is installed
	TCHAR nppDirectory[MAX_PATH];
	::SendMessage(dlg->_nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(nppDirectory));
	dlg->_pluginList.checkInstalledPlugins(nppDirectory);

	// Show the lists
	PluginListContainer availablePlugins = dlg->_pluginList.getAvailablePlugins();
	if (availablePlugins.empty())
		dlg->_availableListView.setMessage(_T("No new plugins available"));
	else
		dlg->_availableListView.setList(availablePlugins);

	PluginListContainer installedPlugins = dlg->_pluginList.getInstalledPlugins();

	if (installedPlugins.empty())
		dlg->_installedListView.setMessage(_T("There are no known installed plugins"));
	else
		dlg->_installedListView.setList(installedPlugins);
	
	PluginListContainer updatesPlugins = dlg->_pluginList.getUpdateablePlugins();
	if (updatesPlugins.empty())
		dlg->_updatesListView.setMessage(_T("There are no plugins with updates available"));
	else
		dlg->_updatesListView.setList(updatesPlugins);

	_endthread();
}