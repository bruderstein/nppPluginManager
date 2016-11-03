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
#include "libinstall/InstallStep.h"
#include "libinstall/DeleteStep.h"
#include "libinstall/VariableHandler.h"
#include "libinstall/CancelToken.h"



using namespace std;


DeleteStep::DeleteStep(const TCHAR *file, BOOL isDirectory)
{
	_file = file;
	_isDirectory = isDirectory;
}


StepStatus DeleteStep::perform(tstring& /*basePath*/, TiXmlElement* forGpup, 
							 std::function<void(const TCHAR*)> setStatus,
							 std::function<void(const int)> stepProgress, 
							 const ModuleInfo* /*moduleInfo*/,
                             CancelToken& /*cancelToken*/)
{
	StepStatus status = STEPSTATUS_FAIL;

	tstring statusString = _T("Deleting ");
	statusString.append(_file);
	setStatus(statusString.c_str());


	BOOL deleteSuccess = FALSE;
	
	if (_isDirectory)
	{
		deleteSuccess = removeDirectory(_file.c_str());
	}
	else
	{
		deleteSuccess = ::DeleteFile(_file.c_str());
	}

	if (!deleteSuccess)
	{
				status = STEPSTATUS_NEEDGPUP;
				// Add delete file to forGpup doc
				
				TiXmlElement* deleteElement = new TiXmlElement(_T("delete"));
				
				deleteElement->SetAttribute(_T("file"), _file.c_str());
				deleteElement->SetAttribute(_T("isDirectory"), _isDirectory ? _T("true") : _T("false"));
				forGpup->LinkEndChild(deleteElement);

	}
	else
		status = STEPSTATUS_SUCCESS;

	return status;
}



void DeleteStep::replaceVariables(VariableHandler *variableHandler)
{
	if (variableHandler)
	{
		variableHandler->replaceVariables(_file);
	}

}


BOOL DeleteStep::removeDirectory(const TCHAR* directory)
{
	tstring dir = directory;
	tstring baseDir = directory;
	baseDir.append(_T("\\"));

	dir.append(_T("\\*"));
	WIN32_FIND_DATA foundData;
	
	HANDLE hFind = ::FindFirstFile(dir.c_str(), &foundData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(foundData.cFileName, _T(".")) && _tcscmp(foundData.cFileName, _T("..")))
				{
					tstring thisDir(baseDir);
					thisDir.append(foundData.cFileName);
					removeDirectory(thisDir.c_str());
					
				}
			}
			else
			{
				tstring thisFile(baseDir);
				thisFile.append(foundData.cFileName);

				::DeleteFile(thisFile.c_str());
			}
		} while(::FindNextFile(hFind, &foundData));
		
		::FindClose(hFind);
	}

	::RemoveDirectory(directory);

	return TRUE;

}