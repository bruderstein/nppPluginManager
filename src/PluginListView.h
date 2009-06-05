#ifndef _PLUGINLISTVIEW_H
#define _PLUGINLISTVIEW_H

#include <windows.h>
#include <tchar.h>
#include "PluginManager.h"

class PluginListView
{
public:

	PluginListView();

	enum VERSIONCOLUMN {
		VERSION_INSTALLED,
		VERSION_AVAILABLE
	};

	
	void	init(HWND hListView, HWND hDescription, int nVersionColumns, VERSIONCOLUMN columns[]);
	void	setList(PluginListContainer &list);
	void	setMessage(TCHAR *msg);
	LRESULT notify(WPARAM wParam, LPARAM lParam);



	

private:
	/* Handles */
	HWND	_hListView;
	HWND	_hDescription;

	/* Actual list container */
	PluginListContainer _list;


	/* Private methods */
	void	initColumns(void);
	int		getCurrentSelectedIndex();
	Plugin* getCurrentPlugin();

	int _nVersionColumns;
	VERSIONCOLUMN* _columns;

	enum LISTMODE {
		LISTMODE_LIST,
		LISTMODE_MESSAGE
	};

	LISTMODE	_listMode;
	TCHAR*		_message;
};



#endif