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
#include <shlwapi.h>
#include <list>
#include <boost/function.hpp>
#include "libinstall/InstallStep.h"
#include "libinstall/CopyStep.h"
#include "libinstall/DownloadManager.h"
#include "libinstall/md5.h"
#include "libinstall/tstring.h"
#include "libinstall/DirectoryUtil.h"
#include "libinstall/VariableHandler.h"

using namespace std;


CopyStep::CopyStep(const TCHAR *from, const TCHAR *to, const TCHAR *toFile, BOOL attemptReplace, BOOL validate, BOOL backup, const char* proxy, const long proxyPort)
{
	_from = from;
	if (to)
	{
		_toDestination = TO_DIRECTORY;
		_to = to;
	}

	if (toFile)
	{
		_toFile = toFile;
		_toDestination = TO_FILE;
	}

	_failIfExists = !attemptReplace;
	_validate	= validate;
	_backup		= backup;
	_proxy		= proxy;
	_proxyPort	= proxyPort;
}

void CopyStep::replaceVariables(VariableHandler *variableHandler)
{
	if (variableHandler)
	{
		variableHandler->replaceVariables(_from);
		
		if (_toDestination == TO_DIRECTORY)
			variableHandler->replaceVariables(_to);
		else if (_toDestination == TO_FILE)
			variableHandler->replaceVariables(_toFile);
	}

}

ValidateStatus CopyStep::Validate(tstring& file)
{
	DownloadManager download;
	
	TCHAR localMD5[(MD5::HASH_LENGTH * 2) + 1];
	MD5::hash(file.c_str(), localMD5, (MD5::HASH_LENGTH * 2) + 1);
	tstring validateUrl = VALIDATE_BASEURL;
	validateUrl.append(localMD5);
	string validateResult;
	if (download.getUrl(validateUrl.c_str(), validateResult, _proxy.c_str(), _proxyPort))
	{
		if (validateResult == VALIDATE_RESULT_OK)
			return VALIDATE_OK;

		else if (validateResult == VALIDATE_RESULT_UNKNOWN)
			return VALIDATE_UNKNOWN;

		else if (validateResult == VALIDATE_RESULT_BANNED)
			return VALIDATE_BANNED;
		else 
			return VALIDATE_UNKNOWN;
	}
	else
		return VALIDATE_UNKNOWN;
}

StepStatus CopyStep::perform(tstring &basePath, TiXmlElement* forGpup, 
							 boost::function<void(const TCHAR*)> setStatus,
							 boost::function<void(const int)> stepProgress,
							 const HWND windowParent)
{
	StepStatus status = STEPSTATUS_SUCCESS;

	tstring fromPath = basePath;
	
	fromPath.append(_from);
	
	tstring statusString = _T("Copying files...");
	setStatus(statusString.c_str());


	tstring fromDir;


	tstring toPath;
	
	
	if (_toDestination == TO_DIRECTORY)
	{
		toPath = _to;
		
		if (toPath == _T(""))
			return STEPSTATUS_FAIL;

		// Check destination directory exists
		if (!::PathFileExists(_to.c_str()))
		{
			DirectoryUtil::createDirectories(_to.c_str());
		}
	

		toPath.append(_T("\\"));
	}
	else
	{
		toPath = _toFile;
		if (toPath == _T(""))
			return STEPSTATUS_FAIL;
	}
	
	tstring::size_type backSlash = fromPath.find_last_of(_T("\\"));
	if (backSlash != tstring::npos)
	{
		fromDir = fromPath.substr(0, backSlash + 1);
	
	} else
	{
		fromDir = basePath;
	}

	// For each file in fromPath, fromFileSpec, copy to [_to]\[found file]
	WIN32_FIND_DATA foundData;
	HANDLE hFindFile = ::FindFirstFile(fromPath.c_str(), &foundData);

	tstring src;
	tstring dest;
	bool copy;

	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			dest = toPath;
			if (_toDestination == TO_DIRECTORY)
				dest.append(foundData.cFileName);
			else if (_toDestination == TO_FILE && dest[dest.size() - 1] == _T('\\'))	
				dest.append(foundData.cFileName);
			else if (_toDestination == TO_FILE && ::PathIsDirectory(dest.c_str()))
			{
				dest.append(_T("\\"));
				dest.append(foundData.cFileName);
			}
			
			// Exclude the . and .. directories
			if (_tcscmp(foundData.cFileName, _T(".")) 
				&& _tcscmp(foundData.cFileName, _T("..")))
			{
				statusString = _T("Copying ");
				statusString.append(foundData.cFileName);
				setStatus(statusString.c_str());


				src = fromDir;
				src.append(foundData.cFileName);
				copy = false;
				if (_validate)
				{
					switch(Validate(src))
					{
					
						case VALIDATE_OK:
							copy = true;
							break;

						case VALIDATE_UNKNOWN:
							{
								tstring msg(_T("It has not been possible to validate the integrity of '"));
								msg.append(foundData.cFileName);
								msg.append(_T("' needed to install or update a plugin.  Do you want to copy this file anyway (not recommended)?"));
								
								int userChoice = ::MessageBox(windowParent, msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);
								
								if (userChoice == IDYES)
								{
									copy = true;
								}
								else
								{
									status = STEPSTATUS_FAIL;
									copy = false;
								}
								break;
							}


						case VALIDATE_BANNED:
							{
								tstring msg(_T("'"));
								msg.append(foundData.cFileName);
								msg.append(_T("' has been identified as unstable, incorrect or dangerous.  It is NOT recommended you install this file.  Do you want to install this file anyway?"));
								
								int userChoice = ::MessageBox(windowParent, msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);
								
								if (userChoice == IDYES)
								{
									copy = true;
								}
								else
								{
									status = STEPSTATUS_FAIL;
									copy = false;
								}
								break;
							}
							


					}
				}
				else 
					copy = true;

				if (copy)
				{
					if (_backup && ::PathFileExists(dest.c_str()))
					{
						tstring baseBackupPath(dest);
						baseBackupPath.append(_T(".backup"));
						tstring backupPath(baseBackupPath);
						int counter = 1;
						TCHAR buf[10];

						// Keep checking the paths - if there's more than 500, tough.
						while(::PathFileExists(backupPath.c_str()) && counter < 500)
						{
							++counter;
							_itot_s(counter, buf, 10, 10);
							backupPath = baseBackupPath;
							backupPath.append(buf);
						}

						// If there's 500 backups, give it a silly name.
						if (counter >= 500)
						{
							backupPath = baseBackupPath;
							backupPath.append(_T("_too_many_backups"));
						}

						::CopyFile(dest.c_str(), backupPath.c_str(), FALSE);

					}

					if (!::CopyFile(src.c_str(), dest.c_str(), _failIfExists))
					{
						status = STEPSTATUS_NEEDGPUP;
						// Add file to forGpup doc
						
						TiXmlElement* copy = new TiXmlElement(_T("copy"));
						
						copy->SetAttribute(_T("from"), src.c_str());
						
						if (_toDestination == TO_DIRECTORY)
							copy->SetAttribute(_T("to"), _to.c_str());
						else if (_toDestination == TO_FILE)
							copy->SetAttribute(_T("toFile"), _toFile.c_str());

						copy->SetAttribute(_T("replace"), _T("true"));
						if (_backup)
							copy->SetAttribute(_T("backup"), _T("true"));
						
						forGpup->LinkEndChild(copy);

					}
				}
			}
		} while(status != STEPSTATUS_FAIL && ::FindNextFile(hFindFile, &foundData));
	}

	::FindClose(hFindFile);

	return status;
}
