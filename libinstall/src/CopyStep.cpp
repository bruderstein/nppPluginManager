#include <tchar.h>
#include <string.h>
#include <windows.h>
#include "CopyStep.h"

#include "tstring.h"

using namespace std;


CopyStep::CopyStep(const TCHAR *from, const TCHAR *to)
{
	_from = from;
	_to = to;
}

BOOL CopyStep::Perform(tstring &basePath)
{
	tstring fromPath = basePath;

	fromPath.append(_from);
	
	tstring fromDir;

	tstring toPath = _to;
	toPath.append(_T("\\"));

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

	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			dest = toPath;
			dest.append(foundData.cFileName);
			src = fromDir;
			src.append(foundData.cFileName);
			::CopyFile(src.c_str(), dest.c_str(), false);
		} while(::FindNextFile(hFindFile, &foundData));
	}

	::FindClose(hFindFile);

	return TRUE;
}
