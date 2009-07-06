#ifndef _PLUGINLISTVIEW_H
#define _PLUGINLISTVIEW_H

#include <windows.h>
#include <tchar.h>
#include <boost/shared_ptr.hpp>
#include <list>
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
	Plugin* getCurrentPlugin();
	boost::shared_ptr< std::list<Plugin*> > getSelectedPlugins();

	

private:
	/* Handles */
	HWND	_hListView;
	HWND	_hDescription;

	/* Actual list container */
	PluginListContainer _list;


	/* Private methods */
	void	initColumns(void);
	int		getCurrentSelectedIndex();


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