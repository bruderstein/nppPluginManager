#include <windows.h>
#include <tchar.h>
#include <string>
#include "Utility.h"

#include "tstring.h"

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


void Utility::startGpup(const TCHAR *nppDir, const TCHAR *arguments)
{
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
	
	tstring gpupExe(_T("\""));
	gpupExe.append(nppDir);
	gpupExe.append(_T("\\updater\\gpup.exe\" "));

	gpupExe.append(gpupArguments);
	STARTUPINFO startup;
	memset(&startup, 0, sizeof(STARTUPINFO));

	startup.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION procinfo;
	
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

}