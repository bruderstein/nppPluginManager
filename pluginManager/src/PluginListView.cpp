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

#include "precompiled_headers.h"
#include "PluginListView.h"
#include "PluginList.h"
#include "PluginVersion.h"
using namespace std;


PluginListView::PluginListView():
	_hListView(NULL),
	_hDescription(NULL),
	_nVersionColumns(0),
	_columns(NULL),
	_displayUpdateDesc(false),
	_listMode(LISTMODE_LIST),
	_message(NULL)
{
}

void PluginListView::init(HWND hListView, HWND hDescription, int nVersionColumns, VERSIONCOLUMN columns[], bool displayUpdateDesc)
{
	_hListView = hListView;
	_hDescription = hDescription;
	_nVersionColumns = nVersionColumns;
	_displayUpdateDesc = displayUpdateDesc;

	_columns = new VERSIONCOLUMN[nVersionColumns];
	
	for(int index = 0; index < nVersionColumns; index++)
		_columns[index] = columns[index];

	initColumns();
}

LRESULT PluginListView::notify(WPARAM /*wParam*/, LPARAM lParam)
{
	switch (((LPNMHDR)lParam)->code)
    {

		case LVN_GETDISPINFO:
		{
			if (_listMode == LISTMODE_LIST)
			{
				NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;    

				Plugin* plugin = reinterpret_cast<Plugin*>(plvdi->item.lParam);

				switch (plvdi->item.iSubItem)
				{
					case 0:
					{
						
						
						plvdi->item.pszText = const_cast<TCHAR*>(plugin->getName().c_str());
						break;
					}
					case 1:
					{
						plvdi->item.pszText = const_cast<TCHAR*>(plugin->getCategory().c_str());
						break;
					}

					default:
						if (plvdi->item.iSubItem == _nVersionColumns + 2)
						{
							plvdi->item.pszText = const_cast<TCHAR*>(plugin->getStability().c_str());
						} 
						else
						{
							switch(_columns[plvdi->item.iSubItem - 2])
							{
								case VERSION_INSTALLED:
									plvdi->item.pszText = plugin->getInstalledVersion().getDisplayString();
									plvdi->item.cchTextMax = (int)_tcslen(plvdi->item.pszText);
									break;

								case VERSION_AVAILABLE:
									plvdi->item.pszText = plugin->getVersion().getDisplayString();
									plvdi->item.cchTextMax = (int)_tcslen(plvdi->item.pszText);
									break;
							}
						}
						break;
				 }
				 return TRUE;
			}
			break;
		}

		case LVN_COLUMNCLICK:
			{
				NMLISTVIEW *colInfo = reinterpret_cast<NMLISTVIEW*>(lParam);
				int (CALLBACK *compareFunc)(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) = NULL;
				LVSORTCOLUMN sortColumn = LVSORTCOLUMN_NAME;

				if (colInfo->iSubItem < 2 || colInfo->iSubItem == _nVersionColumns + 2)
				{
					compareFunc = PluginListView::stringComparer;
					
					if (colInfo->iSubItem == 1)
					{
						sortColumn = LVSORTCOLUMN_CATEGORY;
					}
					else if (colInfo->iSubItem == _nVersionColumns + 2)
					{
						sortColumn = LVSORTCOLUMN_STABILITY;
					}
				}
				else
				{
					compareFunc = PluginListView::versionComparer;
					
					switch(_columns[colInfo->iSubItem - 2])
					{
						case VERSION_AVAILABLE:
							sortColumn = LVSORTCOLUMN_VERSIONAVAILABLE;
							break;

						case VERSION_INSTALLED:
							sortColumn = LVSORTCOLUMN_VERSIONINSTALLED;
							break;
							
						// Just in case there's another one introduced at some point (beta version or something)
						default:
							sortColumn = LVSORTCOLUMN_VERSIONAVAILABLE;
							break;
					}
				}

				ListView_SortItems(_hListView, compareFunc, static_cast<LPARAM>(sortColumn));
			}
			break;

		case NM_CLICK:
			if (_hDescription && lParam)
			{

				LPNMITEMACTIVATE itemActivate = reinterpret_cast<LPNMITEMACTIVATE>(lParam);
				if (itemActivate->iItem >= 0)
				{
					ListView_SetItemState(_hListView, getCurrentSelectedIndex(), 0, LVIS_SELECTED);
					ListView_SetItemState(_hListView, itemActivate->iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
			
			}
			break;
		
		case LVN_ITEMCHANGED:
		{
			LPNMLISTVIEW pnmv = reinterpret_cast<LPNMLISTVIEW>(lParam);
			if (pnmv->uNewState & LVIS_SELECTED)
			{
				Plugin* plugin = reinterpret_cast<Plugin*>(pnmv->lParam);
				if (plugin && _hDescription)
				{
					if (_displayUpdateDesc)
						::SendMessage(_hDescription, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(plugin->getUpdateDescription().c_str()));
					else
						::SendMessage(_hDescription, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(plugin->getDescription().c_str()));
				}
			}
		}
	}

	return FALSE;
}

void PluginListView::initColumns(void)
{

	
	LVCOLUMN col;
	col.mask		= LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	col.fmt			= LVCFMT_LEFT;
	col.iSubItem	= 0;
	col.cx			= 250;
	col.pszText		= _T("Plugin");
	ListView_InsertColumn(_hListView, 0, &col);
	
	col.iSubItem	= 1;
	col.pszText	    = _T("Category");
	col.cx			= 60;
	ListView_InsertColumn(_hListView, 1, &col);

	col.fmt			= LVCFMT_RIGHT;
	int columnOffset = 2;

	for(int index = 0; index < _nVersionColumns; index++)
	{
		col.iSubItem	= index + columnOffset;
		col.cx			= 110;
		switch(_columns[index])
		{
			case VERSION_INSTALLED:
				col.pszText	= _T("Installed Version");
				break;

			case VERSION_AVAILABLE:
				col.pszText = _T("Available Version");
				break;
		}

		ListView_InsertColumn(_hListView, index + columnOffset, &col);
	}
	
	col.iSubItem = _nVersionColumns + columnOffset;
	col.cx = 70;
	col.fmt = 0;
	col.pszText = _T("Stability");
	ListView_InsertColumn(_hListView, col.iSubItem, &col);

	ListView_SetExtendedListViewStyle(_hListView, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	
}

void PluginListView::setList(PluginListContainer &list)
{
	// Copy the list
	ListView_DeleteAllItems(_hListView);

	_list.clear();
	_list.insert(_list.end(), list.begin(), list.end());
	if (_listMode != LISTMODE_LIST)
	{
		ListView_SetExtendedListViewStyle(_hListView, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}
	_listMode = LISTMODE_LIST;
	
	ListView_SetItemCount(_hListView, _list.size());
	PluginListContainer::iterator iter = _list.begin();
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
	lvi.cchTextMax = MAX_PATH;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.state = 0;
	lvi.stateMask = LVIS_SELECTED;

	while (iter != _list.end())
	{
		lvi.lParam = reinterpret_cast<LPARAM>(*iter);
		ListView_InsertItem(_hListView, &lvi);
		++iter;
	}

	ListView_SortItems(_hListView, PluginListView::stringComparer, LVSORTCOLUMN_NAME);
	
}

int PluginListView::getCurrentSelectedIndex()
{
	return (int)SendMessage(_hListView, LVM_GETNEXTITEM, WPARAM(-1), LVIS_SELECTED); 
}

void PluginListView::setMessage(TCHAR *msg)
{
	if (_listMode != LISTMODE_MESSAGE)
	{
		ListView_SetExtendedListViewStyle(_hListView, LVS_EX_FULLROWSELECT);
	}

	_listMode = LISTMODE_MESSAGE;
	
	ListView_DeleteAllItems(_hListView);

	if (_message != NULL)
		delete[] _message;

	size_t msgLength = _tcslen(msg);
	_message = new TCHAR[msgLength + 1];
	_tcscpy_s(_message, msgLength + 1, msg);
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = _message;
	ListView_InsertItem(_hListView, &lvi);
	
}



Plugin* PluginListView::getCurrentPlugin()
{
	LVITEM item;
	item.mask = LVIF_PARAM;
	item.iItem = getCurrentSelectedIndex();
	
	if (item.iItem == -1) 
		return NULL;

	::SendMessage(_hListView, LVM_GETITEM, 0, reinterpret_cast<LPARAM>(&item));
	return reinterpret_cast<Plugin*>(item.lParam);
}


std::shared_ptr< list<Plugin*> > PluginListView::getSelectedPlugins()
{
	
	std::shared_ptr< list<Plugin*> > selectedList;
	
	if (_listMode == LISTMODE_MESSAGE)
		return selectedList;

	LVITEM item;
	item.mask = LVIF_PARAM;
	
	selectedList.reset(new list<Plugin*>());


	UINT size = ListView_GetItemCount(_hListView);

	for (UINT position = 0; position < size; position++)
	{
		item.iItem = position;
		ListView_GetItem(_hListView, &item);
		if (ListView_GetCheckState(_hListView, position))
			selectedList->push_back(reinterpret_cast<Plugin*>(item.lParam));

	}

	return selectedList;

}


void PluginListView::removeSelected()
{
	int size = ListView_GetItemCount(_hListView);
	
	for (int position = size - 1; position >= 0; position--)
	{
		if (ListView_GetCheckState(_hListView, position))
			ListView_DeleteItem(_hListView, position);
	}

}


void PluginListView::setAllCheckState(BOOL checked)
{
	int size = ListView_GetItemCount(_hListView);
	
	for (int position = 0; position < size; position++)
	{
		ListView_SetCheckState(_hListView, position, checked);
	}

}

void PluginListView::selectAll()
{
	setAllCheckState(TRUE);
}

void PluginListView::selectNone()
{
	setAllCheckState(FALSE);
}

BOOL PluginListView::empty()
{
	return (ListView_GetItemCount(_hListView) == 0);
}

int CALLBACK PluginListView::stringComparer(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const TCHAR *text1;
	const TCHAR *text2;

	switch(static_cast<LVSORTCOLUMN>(lParamSort))
	{
		case LVSORTCOLUMN_NAME:
			text1 = reinterpret_cast<Plugin*>(lParam1)->getName().c_str();
			text2 = reinterpret_cast<Plugin*>(lParam2)->getName().c_str();
			break;

		case LVSORTCOLUMN_STABILITY:
			text1 = reinterpret_cast<Plugin*>(lParam1)->getStability().c_str();
			text2 = reinterpret_cast<Plugin*>(lParam2)->getStability().c_str();
			break;

		case LVSORTCOLUMN_CATEGORY:
			text1 = reinterpret_cast<Plugin*>(lParam1)->getCategory().c_str();
			text2 = reinterpret_cast<Plugin*>(lParam2)->getCategory().c_str();
			break;

		default:
			text1 = _T("");
			text2 = _T("");
			break;
	}

	
	return _tcsicmp(text1, text2);
}


int CALLBACK PluginListView::versionComparer(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	PluginVersion version1, version2;

	switch(static_cast<LVSORTCOLUMN>(lParamSort))
	{
		case LVSORTCOLUMN_VERSIONAVAILABLE:
			version1 = reinterpret_cast<Plugin*>(lParam1)->getVersion();
			version2 = reinterpret_cast<Plugin*>(lParam2)->getVersion();
			break;

		case LVSORTCOLUMN_VERSIONINSTALLED:
			version1 = reinterpret_cast<Plugin*>(lParam1)->getInstalledVersion();
			version2 = reinterpret_cast<Plugin*>(lParam2)->getInstalledVersion();
			break;

	}

	int retVal = 0;

	if (version1 < version2)
		retVal = -1;
	else if (version1 > version2)
		retVal = 1;

	return retVal;
}