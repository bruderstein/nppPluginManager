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
				tstring thisDir(baseDir);
				thisDir.append(foundData.cFileName);
				removeDirectory(thisDir.c_str());
				::RemoveDirectory(thisDir.c_str());
			}
			else
			{
				tstring thisFile(baseDir);
				thisFile.append(foundData.cFileName);

				::DeleteFile(thisFile.c_str());
			}
		} while(::FindNextFile(hFind, &foundData));
	}

	return TRUE;

}