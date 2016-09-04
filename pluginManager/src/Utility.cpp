/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009-2010,2015 Dave Brotherstone <davegb@pobox.com>

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
#include "Utility.h"
#include "PluginManager.h"
#include "PluginVersion.h"
#include "PluginManagerVersion.h"

BOOL Utility::removeDirectory(const TCHAR* directory)
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


void Utility::startGpup(HWND errorParent, const TCHAR *nppDir, const TCHAR *arguments, bool /* needAdmin */)
{

	tstring gpupExe(nppDir);
	gpupExe.append(_T("\\updater\\gpup.exe"));

	if (!::PathFileExists(gpupExe.c_str()))
	{
		::MessageBox(errorParent, _T("A file needed by the plugin manager (gpup.exe) is not present under the updater directory.  You should update or reinstall the Plugin Manager plugin to fix this problem.  Notepad++ will not restart."),
			_T("Notepad++ Plugin Manager"), MB_ICONERROR);
		return;
	}

	PluginVersion *gpupVersion = getFileVersion(gpupExe.c_str());

	if ((*gpupVersion) < PluginVersion(PLUGINMANAGERVERSION_STRING))
	{
		::MessageBox(errorParent, _T("The external plugin updater (gpup.exe) under ")
									 _T("the updater directory does not appear to be the correct version.  ")
									 _T("It will most likely work, but you should reinstall the ")
									 _T("plugin manager at your next opportunity to ensure full ")
									 _T("functionality.  We'll attempt to use the current version ")
									 _T("for this operation."), 
									 _T("Plugin Manager GPUP.EXE Version"),
									 MB_OK | MB_ICONWARNING);
	}
	delete gpupVersion;
	//gpupExe.insert(0, _T("\""));
	//gpupExe.append(_T("\" "));

	tstring gpupArguments(arguments);

	if (!gpupArguments.empty())
		gpupArguments.append(_T(" "));

	tstring notepadExe;
	TCHAR* notepadCmdLine = ::GetCommandLine();

	TCHAR stopChar = _T(' ');

	if (*notepadCmdLine == _T('\"'))
	{
		++notepadCmdLine;
		stopChar = _T('\"');
	}

	while(*notepadCmdLine && *notepadCmdLine != stopChar)
	{
		notepadExe.push_back(*notepadCmdLine);
		++notepadCmdLine;
	}

	

	gpupArguments.append(_T("-w \"Notepad++\" -e \""));
	gpupArguments.append(notepadExe);
	gpupArguments.append(_T("\""));
	
	// gpupExe.append(gpupArguments);
	/* 
	STARTUPINFO startup;
	memset(&startup, 0, sizeof(STARTUPINFO));

	startup.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION procinfo;
*/
	::ShellExecute(NULL,
				   _T("open"),
				   gpupExe.c_str(),
				   gpupArguments.c_str(),
				   NULL,
				   SW_SHOW);

	/*
    ::CreateProcess(NULL,						 // No module - using command line
				    (TCHAR *)gpupExe.c_str(),    // command line & arguments
						NULL,        // process security
						NULL,        // thread security
						FALSE,        // inherit handles flag
						NULL,           // flags
						NULL,        // inherit environment
						NULL,        // inherit directory
						&startup,    // STARTUPINFO
						&procinfo);  // PROCESS_INFORMATION
*/
}


PluginVersion* Utility::getFileVersion(const TCHAR* path)
{
	DWORD handle;
	DWORD bufferSize = ::GetFileVersionInfoSize(path, &handle);
	PluginVersion *version;

	if (bufferSize <= 0) 
		return new PluginVersion();

	unsigned char* buffer = new unsigned char[bufferSize];
	::GetFileVersionInfo(path, handle, bufferSize, buffer);
	
	/*struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;*/

	VS_FIXEDFILEINFO* lpFileInfo;

	UINT cbFileInfo;

	VerQueryValue(buffer, 
              _T("\\"),
              (LPVOID*)&lpFileInfo,
              &cbFileInfo);

	if (cbFileInfo)
	{
		version = new PluginVersion((lpFileInfo->dwFileVersionMS & 0xFFFF0000) >> 16, 
												  lpFileInfo->dwFileVersionMS & 0x0000FFFF,
												  (lpFileInfo->dwFileVersionLS & 0xFFFF0000) >> 16,
												  lpFileInfo->dwFileVersionLS & 0x0000FFFF);
	} 
	else 
	{
		version = new PluginVersion();
	}

	delete [] buffer;

	return version;
}