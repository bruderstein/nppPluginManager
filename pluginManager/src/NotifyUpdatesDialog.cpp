/*
This file is part of Plugin Manager for Notepad++
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
#include "NotifyUpdatesDialog.h"
#include "PluginInterface.h"
#include "resource.h"

using namespace std;
using namespace std::placeholders;

void NotifyUpdatesDialog::doDialog()
{
    if (!isCreated())
        create(IDD_UPDATESNOTIFY);

	goToCenter();
}

void NotifyUpdatesDialog::doModal()
{
    ::DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_UPDATESNOTIFY), _nppData._nppHandle, NotifyUpdatesDialog::dlgProc, reinterpret_cast<LPARAM>(this));
}

void NotifyUpdatesDialog::init(HINSTANCE hInst, NppData nppData, PluginList* pluginList)
{
	_nppData = nppData;
	_pluginList = pluginList;

	_updateList = _pluginList->getUpdateablePlugins();
			
	tstring pluginConfigFilename(_pluginList->getVariableHandler()->getVariable(_T("CONFIGDIR")));
	pluginConfigFilename.append(_T("\\PluginManager.ini"));
				
	PluginListContainer::iterator iter = _updateList.begin();
	while (iter != _updateList.end())
	{
		TCHAR ignoreVersion[20];
		::GetPrivateProfileString(_T("IgnoreUpdates"), (*iter)->getName().c_str(), _T(""), ignoreVersion, 20, pluginConfigFilename.c_str());
		if (PluginVersion(ignoreVersion) == (*iter)->getVersion())
			iter = _updateList.erase(iter);
		else
			++iter;
	}

	
	Window::init(hInst, nppData._nppHandle);
}


BOOL NotifyUpdatesDialog::updatesAvailable()
{
	return !_updateList.empty();
}


INT_PTR CALLBACK NotifyUpdatesDialog::run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
			PluginListView::VERSIONCOLUMN columns[2];
			columns[0] = PluginListView::VERSION_INSTALLED;
			columns[1] = PluginListView::VERSION_AVAILABLE;
			_hListView = ::GetDlgItem(_hSelf, IDC_LISTUPDATES);
			_hUpdateDescription = ::GetDlgItem(_hSelf, IDC_UPDATEDESC);
			_pluginListView.init(_hListView, _hUpdateDescription, 2, columns, true);
			
			_pluginListView.setList(_updateList);
			_pluginListView.selectAll();
			

			goToCenter();
			return TRUE;
		}
		case WM_COMMAND : 
		{
			switch (wParam)
			{
				case IDC_IGNORE:
					{
						std::shared_ptr<list<Plugin*> > selectedPlugins = _pluginListView.getSelectedPlugins();
						tstring pluginConfigFilename(_pluginList->getVariableHandler()->getVariable(_T("CONFIGDIR")));
						pluginConfigFilename.append(_T("\\PluginManager.ini"));
						
						list<Plugin*>::iterator iter = selectedPlugins->begin();
						while (iter != selectedPlugins->end())
						{
							::WritePrivateProfileString(_T("IgnoreUpdates"), 
								(*iter)->getName().c_str(), 
								(*iter)->getVersion().getDisplayString(),
								pluginConfigFilename.c_str());
							++iter;
						}

						_pluginListView.removeSelected();

						if (_pluginListView.empty())
							::EndDialog(_hSelf, 0);

						return TRUE;
					}

				case IDC_UPDATE:
					{
                        CancelToken cancelToken;
						ProgressDialog progress(_hInst, 
                            cancelToken,
							std::bind(&PluginList::startInstall, _pluginList, _nppData._nppHandle, _1, &_pluginListView, TRUE, cancelToken));
						progress.doModal(_hSelf);
						
						_pluginListView.removeSelected();

						if (_pluginListView.empty())
							::EndDialog(_hSelf, 0);

						_pluginListView.selectAll();

						return TRUE;
					}
				case IDCANCEL :
					::EndDialog(_hSelf, 0);
					return TRUE;

				default :
					break;
			}
			break;
		}

		case WM_NOTIFY:
		{
			
			if (((LPNMHDR)lParam)->hwndFrom == _hListView)
			{
				_pluginListView.notify(wParam, lParam);
				return TRUE;
			}

			break;
		}

	}
	return FALSE;
}

