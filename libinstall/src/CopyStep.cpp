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
#include "libinstall/CopyStep.h"
#include "libinstall/DownloadManager.h"
#include "libinstall/md5.h"
#include "libinstall/tstring.h"
#include "libinstall/DirectoryUtil.h"
#include "libinstall/VariableHandler.h"
#include "libinstall/Validate.h"
#include "libinstall/ModuleInfo.h"
#include "libinstall/CancelToken.h"

using namespace std;


CopyStep::CopyStep(const TCHAR *from, const TCHAR *to, const TCHAR *toFile, BOOL attemptReplace,
				   BOOL validate, BOOL isGpup, BOOL backup, BOOL recursive,
				   const tstring& validateBaseUrl)
				   : _from(from), _validate(validate), _failIfExists(!attemptReplace),
				     _isGpup(isGpup), _backup(backup), _recursive(recursive),
                     _validateBaseUrl(validateBaseUrl)
{

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




StepStatus CopyStep::perform(tstring &basePath, TiXmlElement* forGpup,
							 std::function<void(const TCHAR*)> setStatus,
							 std::function<void(const int)> stepProgress,
							 const ModuleInfo* moduleInfo,
                             CancelToken& cancelToken)
{

	tstring fromPath = basePath;

	fromPath.append(_from);

	tstring statusString = _T("Copying files...");
	setStatus(statusString.c_str());





	tstring toPath;

	// Need to split this out into separate function, then we can call it recursively (if _recursive).

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


	//////////// Special GPUP.EXE handling (run the new version to copy itself over the old one)
	if (_isGpup)
	{
		setStatus(_T("Copying GPUP.EXE"));
		if (_toDestination == TO_DIRECTORY)
			toPath.append(_T("gpup.exe"));
		copyGpup(basePath, toPath);
		return STEPSTATUS_SUCCESS;
	}

	return copyDirectory(fromPath, toPath, forGpup, setStatus, stepProgress, moduleInfo, cancelToken);
}


StepStatus CopyStep::copyDirectory(tstring& fromPath, tstring& toPath,
					 TiXmlElement* forGpup,
					 std::function<void(const TCHAR*)> setStatus,
					 std::function<void(const int)> stepProgress,
                     const ModuleInfo* moduleInfo,
                     CancelToken& cancelToken)
{
	StepStatus status = STEPSTATUS_SUCCESS;

	tstring fromDir;

	tstring::size_type backSlash = fromPath.find_last_of(_T("\\"));
	if (backSlash != tstring::npos)
	{
		fromDir = fromPath.substr(0, backSlash + 1);

	} else
	{
		// Was basePath
		fromDir = fromPath;
	}


	// For each file in fromPath, fromFileSpec, copy to [_to]\[found file]
	WIN32_FIND_DATA foundData;
	HANDLE hFindFile = ::FindFirstFile(fromPath.c_str(), &foundData);

	tstring src;
	tstring dest;
	tstring statusString;
	tstring fullFoundPath;



	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
            if (cancelToken.isSignalled()) {
                return STEPSTATUS_FAIL;
            }

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

			fullFoundPath = fromDir;
			fullFoundPath.append(foundData.cFileName);



			// Exclude the . and .. directories
			if (_tcscmp(foundData.cFileName, _T("."))
				&& _tcscmp(foundData.cFileName, _T("..")))
			{

				// Check if we've found a directory, if so, then
				if (::PathIsDirectory(fullFoundPath.c_str()))
				{
					if (_recursive)
					{
						// If recursive, then copy everything in the child directories
						fullFoundPath.append(_T("\\*.*"));

						// Check destination directory exists
						if (!::PathFileExists(dest.c_str()))
						{
							DirectoryUtil::createDirectories(dest.c_str());
						}

						// Destination must end in a backslash for directories
						dest.append(_T("\\"));
						// Recursively call ourselves to copy this directory
						status = copyDirectory(fullFoundPath, dest, forGpup, setStatus, stepProgress, moduleInfo, cancelToken);

					}

					// Skip to next file
					continue;
				}

				statusString = _T("Copying ");
				statusString.append(foundData.cFileName);
				setStatus(statusString.c_str());

				src = fromDir;
				src.append(foundData.cFileName);
				bool copy = false;
				if (_validate)
				{
					switch(Validator::validate(_validateBaseUrl.c_str(), src, cancelToken, moduleInfo))
					{

						case VALIDATE_OK:
							copy = true;
							break;

						case VALIDATE_UNKNOWN:
							{
								tstring msg(_T("It has not been possible to validate the integrity of '"));
								msg.append(foundData.cFileName);
								msg.append(_T("' needed to install or update a plugin.  Do you want to copy this file anyway (not recommended)?"));

								int userChoice = ::MessageBox(moduleInfo->getHParent(), msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);

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

								int userChoice = ::MessageBox(moduleInfo->getHParent(), msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);

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

					// Mainly for gpup, but also if copying to a filename, the path must exist
					tstring destPath = dest.substr(0, dest.find_last_of(_T("\\")));
					if (!::PathIsDirectory(destPath.c_str())) {
						DirectoryUtil::createDirectories(destPath.c_str());
					}

					if (!::CopyFile(src.c_str(), dest.c_str(), _failIfExists))
					{
						status = STEPSTATUS_NEEDGPUP;
						// Add file to forGpup doc

						TiXmlElement* copyElement = new TiXmlElement(_T("copy"));

						copyElement->SetAttribute(_T("from"), src.c_str());

						copyElement->SetAttribute(_T("toFile"), dest.c_str());

						copyElement->SetAttribute(_T("replace"), _T("true"));
						if (_backup)
							copyElement->SetAttribute(_T("backup"), _T("true"));

						if (_validate) {
							copyElement->SetAttribute(_T("validate"), _T("true"));
						}
						forGpup->LinkEndChild(copyElement);

					}
				}
			}
		} while(status != STEPSTATUS_FAIL && ::FindNextFile(hFindFile, &foundData));
	}

	::FindClose(hFindFile);
	return status;
}



void CopyStep::copyGpup(const tstring& basePath, const tstring& toPath)
{
	if (!_tcsicmp(_T("gpup.exe"), PathFindFileName(_from.c_str())))
	{
		tstring arguments(_T("-c \""));
		arguments.append(basePath.c_str());
		arguments.append(_from.c_str());
		arguments.append(_T("\" -t \""));
		arguments.append(toPath.c_str());
		arguments.append(_T("\""));
		tstring gpupExe(basePath);
		gpupExe.append(_from);
		callGpup(gpupExe.c_str(), arguments.c_str());
	}
}

void CopyStep::callGpup(const TCHAR *gpupPath, const TCHAR *arguments)
{
	OSVERSIONINFO osvi;
    BOOL bIsWindowsVistaOrLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsVistaOrLater = (osvi.dwMajorVersion >= 6);

	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = gpupPath;
	sei.lpParameters = arguments;
	sei.lpVerb =  bIsWindowsVistaOrLater ? _T("runas") : _T("open");
	sei.nShow = SW_SHOW;

	if (::ShellExecuteEx(&sei))
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}

}