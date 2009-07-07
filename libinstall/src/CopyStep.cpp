#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <boost/function.hpp>
#include "libinstall/InstallStep.h"
#include "libinstall/CopyStep.h"

#include "libinstall/tstring.h"

using namespace std;


CopyStep::CopyStep(const TCHAR *from, const TCHAR *to, BOOL attemptReplace)
{
	_from = from;
	_to = to;
	_failIfExists = !attemptReplace;
}

StepStatus CopyStep::perform(tstring &basePath, TiXmlElement* forGpup, 
							 boost::function<void(const TCHAR*)> setStatus,
							 boost::function<void(const int)> stepProgress)
{
	StepStatus status = STEPSTATUS_SUCCESS;

	tstring fromPath = basePath;
	
	fromPath.append(_from);
	
	tstring statusString = _T("Copying ");
	statusString.append(_from);
	setStatus(statusString.c_str());


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
			if (!::CopyFile(src.c_str(), dest.c_str(), _failIfExists))
			{
				status = STEPSTATUS_NEEDGPUP;
				// Add file to forGpup doc
				
				TiXmlElement* copy = new TiXmlElement(_T("copy"));
				
				copy->SetAttribute(_T("from"), src.c_str());
				copy->SetAttribute(_T("to"), _to.c_str());
				copy->SetAttribute(_T("replace"), _T("true"));
				forGpup->LinkEndChild(copy);

			}
		} while(::FindNextFile(hFindFile, &foundData));
	}

	::FindClose(hFindFile);

	return status;
}
