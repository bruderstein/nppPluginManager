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
#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <boost/function.hpp>
#include "libinstall/InstallStep.h"
#include "libinstall/DeleteStep.h"

#include "libinstall/tstring.h"

using namespace std;


DeleteStep::DeleteStep(const TCHAR *file)
{
	_file = file;
}

StepStatus DeleteStep::perform(tstring& /*basePath*/, TiXmlElement* forGpup, 
							 boost::function<void(const TCHAR*)> setStatus,
							 boost::function<void(const int)> stepProgress, 
							 const HWND /*windowParent*/)
{
	StepStatus status = STEPSTATUS_FAIL;

	tstring statusString = _T("Deleting ");
	statusString.append(_file);
	setStatus(statusString.c_str());


	BOOL deleteSuccess = ::DeleteFile(_file.c_str());

	if (!deleteSuccess)
	{
				status = STEPSTATUS_NEEDGPUP;
				// Add delete file to forGpup doc
				
				TiXmlElement* deleteElement = new TiXmlElement(_T("delete"));
				
				deleteElement->SetAttribute(_T("file"), _file.c_str());
				
				forGpup->LinkEndChild(deleteElement);

	}
	else
		status = STEPSTATUS_SUCCESS;

	return status;
}
