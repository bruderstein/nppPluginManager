/*
This file is part of Plugin Manager for Notepad++
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


#ifndef _NOTIFYUPDATESDIALOG_H
#define _NOTIFYUPDATESDIALOG_H

#include "StaticDialog.h"
#include "PluginManager.h"
#include "PluginList.h"
#include "PluginListView.h"


class NotifyUpdatesDialog : public StaticDialog
{

public:
	NotifyUpdatesDialog() : StaticDialog() {};
    
    void init(HINSTANCE hInst, NppData nppData, PluginList* pluginList);
	
   	void doDialog();
	void doModal();

    virtual void destroy() {
        
    };

	BOOL updatesAvailable();


protected :
	virtual BOOL CALLBACK run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	/* Handles */
	NppData			_nppData;
    HWND			_HSource;
	HWND            _hListView;

	PluginList      *_pluginList;
	PluginListView  _pluginListView;
	PluginListContainer _updateList;

};



#endif // ABOUT_DIALOG_H
