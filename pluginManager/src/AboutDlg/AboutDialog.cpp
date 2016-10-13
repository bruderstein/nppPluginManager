/*
This file is part of Plugin Template for Notepad++
Copyright (C)2006 Jens Lorenz <jens.plugin.npp@gmx.de>

Modifications for Plugin Manager Plugin for Notepad++
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
#include "AboutDialog.h"
#include "PluginInterface.h"
#include "resource1.h"

void AboutDialog::doDialog()
{
    if (!isCreated())
        create(IDD_ABOUT_DLG);

	goToCenter();
}


INT_PTR CALLBACK AboutDialog::run_dlgProc(UINT Message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
           
			return TRUE;
		}
		case WM_COMMAND : 
		{
			switch (wParam)
			{
				case IDOK :
				case IDCANCEL :
					display(FALSE);
					return TRUE;

				default :
					break;
			}
		}
	}
	return FALSE;
}

