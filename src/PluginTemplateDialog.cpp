/*
This file is part of Plugin Template Plugin for Notepad++
Copyright (C)2007 Jens Lorenz <jens.plugin.npp@gmx.de>

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

#include "PluginTemplateDialog.h"
#include "resource.h"


/************************* define here your toolbar layout *********************/

/* messages needs to be added in resource.h */

static ToolBarButtonUnit toolBarIcons[] = {
	{IDM_EX_UNDO, -1, -1, -1, -1, 0/* TBSTYLE_DROPDOWN */},
	{IDM_EX_REDO, -1, -1, -1, -1, 0/* TBSTYLE_DROPDOWN */}
};
					
static int stdIcons[] = {IDB_EX_UNDO, IDB_EX_REDO};

/** 
 *	Note: On change, keep sure to change order of IDM_EX_... also in function GetNameStrFromCmd
 */
static LPTSTR szToolTip[5] = {
	"Undo",
	"Redo"
};

void TemplateDialog::GetNameStrFromCmd(UINT resID, LPTSTR tip)
{
	strcpy(tip, szToolTip[resID - IDM_EX_UNDO]);
}
/**************************** end of toolbar layout ****************************/




/* Note: Within constructor send dialog IDD to DockingDlgInterface class */
TemplateDialog::TemplateDialog(void) : DockingDlgInterface(IDD_DOCK_DLG), _hEdit(NULL)
{
}

TemplateDialog::~TemplateDialog(void)
{
}


void TemplateDialog::init(HINSTANCE hInst, NppData nppData, tPluginProp *pPluginProp)
{
	_nppData = nppData;
	_pPluginProp = pPluginProp;
	DockingDlgInterface::init(hInst, nppData._nppHandle);
}


void TemplateDialog::doDialog(bool willBeShown)
{
    if (!isCreated())
	{
		create(&_data);

		// define the default docking behaviour
		_data.uMask			= DWS_DF_CONT_RIGHT | DWS_ICONTAB;
		_data.hIconTab		= (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_TABBAR), IMAGE_ICON, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
		_data.pszModuleName	= getPluginFileName();
		_data.dlgID			= TOGGLE_DOCKABLE_WINDOW_INDEX;
		::SendMessage(_hParent, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&_data);
	}
	display(willBeShown);
}


BOOL CALLBACK TemplateDialog::run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
		case WM_INITDIALOG:
		{
			InitialDialog();
			break;
		}
		case WM_SIZE:
		case WM_MOVE:
		{
			RECT	rc			= {0};

			/* test if in side or if docked on top or bottom */
			if ((_iDockedPos == CONT_LEFT) || (_iDockedPos == CONT_RIGHT))
			{
				getClientRect(rc);

				/* set position of toolbar */
				_ToolBar.reSizeTo(rc);
				_Rebar.reSizeTo(rc);

				/* set position of edit control */
				rc.top	   += 26;
				rc.bottom  -= 26;
				::SetWindowPos(_hEdit, NULL, rc.left, rc.top, rc.right, rc.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
			}
			else
			{
				getClientRect(rc);
				rc.right = rc.left + 23;
                
				/* set position of toolbar */
				_ToolBar.reSizeTo(rc);
				_Rebar.reSizeTo(rc);

				/* set position of edit control */
				getClientRect(rc);
				rc.left		+= 23;
				rc.right	-= 23;
				::SetWindowPos(_hEdit, NULL, rc.left, rc.top, rc.right, rc.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
			}
			break;
		}
		case WM_COMMAND:
		{
			if ((HWND)lParam == _ToolBar.getHSelf())
			{
				tb_cmd(LOWORD(wParam));
				return TRUE;
			}
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR		nmhdr = (LPNMHDR)lParam;

			if (nmhdr->hwndFrom == _hEdit) {

				/* do what ever you need */

			} else if (nmhdr->code == TTN_GETDISPINFO) {

				/* tooltip request of toolbar */

				LPTOOLTIPTEXT lpttt; 

				lpttt = (LPTOOLTIPTEXT)nmhdr; 
				lpttt->hinst = _hInst; 

				// Specify the resource identifier of the descriptive 
				// text for the given button.
				int resId = int(lpttt->hdr.idFrom);

				TCHAR	tip[16];
				GetNameStrFromCmd(resId, tip);
				lpttt->lpszText = tip;
				return TRUE;
			} else {

				/* parse all other notifications to docking dialog interface */

				return DockingDlgInterface::run_dlgProc(hWnd, Message, wParam, lParam);
			}
			break;
		}
		case WM_DESTROY:
		{
			/* destroy icon of tab */
            ::DestroyIcon(_data.hIconTab);
			break;
		}
		default:
			return DockingDlgInterface::run_dlgProc(hWnd, Message, wParam, lParam);
	}

	return FALSE;
}

void TemplateDialog::InitialDialog(void)
{
	/* get handle of dialogs */
	_hEdit			= ::GetDlgItem(_hSelf, IDC_EDIT_TEMP);

	/* create toolbar */
	_ToolBar.init(_hInst, _hSelf, 16, toolBarIcons, sizeof(toolBarIcons)/sizeof(ToolBarButtonUnit), true, stdIcons, sizeof(stdIcons)/sizeof(int));
	_ToolBar.display();
	_Rebar.init(_hInst, _hSelf, &_ToolBar);
	_Rebar.display();
}

void TemplateDialog::tb_cmd(UINT message)
{
	switch (message)
	{
		case IDM_EX_UNDO:
		{
			::SendMessage(_hEdit, WM_UNDO, 0, 0);
			break;
		}
		case IDM_EX_REDO:
		{
			TCHAR	pszText[256];
			_stprintf(pszText, "Value1 = %d\r\nValue2 = %d\r\n\r\n", _pPluginProp->iValue1, _pPluginProp->iValue2);
			::SendMessage(_hEdit, WM_SETTEXT, 0, (LPARAM)pszText);
			break;
		}
		default:
			break;
	}
}
