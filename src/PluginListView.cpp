
#include "PluginListView.h"
#include "PluginList.h"
#include <commctrl.h>
#include <string.h>

PluginListView::PluginListView()
{
	_listMode = LISTMODE_LIST;
	_message = NULL;
}

void PluginListView::init(HWND hListView, HWND hDescription, int nVersionColumns, VERSIONCOLUMN columns[])
{
	_hListView = hListView;
	_hDescription = hDescription;
	_nVersionColumns = nVersionColumns;
	_columns = new VERSIONCOLUMN[nVersionColumns];
	
	for(int index = 0; index < nVersionColumns; index++)
		_columns[index] = columns[index];

	initColumns();
}

LRESULT PluginListView::notify(WPARAM wParam, LPARAM lParam)
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
						// need plugin object get tchars to be clever
						
						plvdi->item.pszText = const_cast<TCHAR*>(plugin->getName().c_str());
						break;
					}
					default:
						switch(_columns[plvdi->item.iSubItem - 1])
						{
							case VERSION_INSTALLED:
								plvdi->item.pszText = plugin->getInstalledVersion().getDisplayString();
								plvdi->item.cchTextMax = _tcslen(plvdi->item.pszText);
								break;

							case VERSION_AVAILABLE:
								plvdi->item.pszText = plugin->getVersion().getDisplayString();
								plvdi->item.cchTextMax = _tcslen(plvdi->item.pszText);
								break;
						}
						break;
				 }
				 return TRUE;
			}
			break;
		}

		case NM_CLICK:
		{
			Plugin* plugin = getCurrentPlugin();
			if (NULL != plugin)
			{
				TCHAR *description = (TCHAR*)plugin->getDescription().c_str();
				::SendMessage(_hDescription, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(description));
			} 
			else
				::SendMessage(_hDescription, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(_T("")));

		}
	}
}

void PluginListView::initColumns(void)
{

	
	LVCOLUMN col;
	col.mask		= LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	col.fmt			= LVCFMT_LEFT;
	col.iSubItem	= 0;
	col.cx			= 250;
	col.pszText = _T("Plugin");
	ListView_InsertColumn(_hListView, 0, &col);

	for(int index = 0; index < _nVersionColumns; index++)
	{
		col.iSubItem	= index + 1;
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

		ListView_InsertColumn(_hListView, index + 1, &col);
	}
	
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

	
}

int PluginListView::getCurrentSelectedIndex()
{
	return SendMessage(_hListView, LVM_GETNEXTITEM, -1, LVIS_SELECTED); 
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

	int msgLength = _tcslen(msg);
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