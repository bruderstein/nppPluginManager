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
#include "libinstall/DirectoryUtil.h"

using namespace std;

BOOL DirectoryUtil::createDirectories(const TCHAR *dir)
{
	BOOL created = FALSE;

	if (!::CreateDirectory(dir, NULL))
	{
		tstring createPath = dir;
		list<tstring> pathsToCreate;

		// Add the deepest directory to the top of the list, so it will be created last
		pathsToCreate.push_back(createPath);

		do {

			createPath.erase(createPath.find_last_of(_T('\\')));
			if (!::CreateDirectory(createPath.c_str(), NULL))
			{
				pathsToCreate.push_back(createPath);
			}
			else
			{
				created = TRUE;
			}

		} while (createPath.find(_T('\\')) != tstring::npos 
			&& !created);
		
		if (created)
		{
			for(list<tstring>::reverse_iterator iter = pathsToCreate.rbegin(); iter != pathsToCreate.rend(); ++iter)
			{
				::CreateDirectory(iter->c_str(), NULL);
			}
		}
	}
	else
	{
		created = TRUE;
	}

	return created;

}

