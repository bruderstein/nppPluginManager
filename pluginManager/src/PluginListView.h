/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009-2010 Dave Brotherstone <davegb@pobox.com>

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
#ifndef _PLUGINLISTVIEW_H
#define _PLUGINLISTVIEW_H

#include "PluginManager.h"

class PluginListView
{
public:

	PluginListView();

	enum VERSIONCOLUMN {
		VERSION_INSTALLED,
		VERSION_AVAILABLE
	};


	
	void	init(HWND hListView, HWND hDescription, int nVersionColumns, VERSIONCOLUMN columns[], bool displayUpdateDesc = false);
	void	setList(PluginListContainer &list);

	void    removeSelected();
	void    selectAll();
	void    selectNone();

	void	setMessage(TCHAR *msg);
	LRESULT notify(WPARAM wParam, LPARAM lParam);
	Plugin* getCurrentPlugin();
	std::shared_ptr< std::list<Plugin*> > getSelectedPlugins();
	BOOL    empty();
	

private:
	/* Handles */
	HWND	_hListView;
	HWND	_hDescription;

	/* Actual list container */
	PluginListContainer _list;


	int _nVersionColumns;
	VERSIONCOLUMN* _columns;
	bool _displayUpdateDesc;

	enum LISTMODE {
		LISTMODE_LIST,
		LISTMODE_MESSAGE
	};

	LISTMODE	_listMode;
	TCHAR*		_message;


	/* Private methods */
	void	initColumns(void);
	int		getCurrentSelectedIndex();
	void    setAllCheckState(BOOL checked);

	enum LVSORTCOLUMN
	{
		LVSORTCOLUMN_NAME,
		LVSORTCOLUMN_VERSIONAVAILABLE,
		LVSORTCOLUMN_VERSIONINSTALLED,
		LVSORTCOLUMN_CATEGORY,
		LVSORTCOLUMN_STABILITY
	};

	static int CALLBACK stringComparer(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK versionComparer(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};



#endif