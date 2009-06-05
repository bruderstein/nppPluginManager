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


#ifndef PLUGIN_TEMPLATE_DLG_H
#define PLUGIN_TEMPLATE_DLG_H

#include "PluginTemplate.h"
#include "DockingDlgInterface.h"
#include "Toolbar.h"


class TemplateDialog : public DockingDlgInterface
{
public:
	TemplateDialog(void);
	~TemplateDialog(void);

    void init(HINSTANCE hInst, NppData nppData, tPluginProp *pMgrProp);

	void destroy(void) {};

   	void doDialog(bool willBeShown = true);

protected:

	/* get toolbar tooltips */
	void GetNameStrFromCmd(UINT resID, LPTSTR tip);

	virtual BOOL CALLBACK run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/* initial dialog here */
	void InitialDialog(void);
	
	/* toolbar commands */
	void tb_cmd(UINT message);

private:
	/* Handles */
	NppData					_nppData;
	HWND					_hEdit;

	/* classes */
	ToolBar					_ToolBar;
	ReBar					_Rebar;

	/* settings */
	tTbData					_data;
	tPluginProp*			_pPluginProp;
};


#endif // PLUGIN_TEMPLATE_DLG_H

