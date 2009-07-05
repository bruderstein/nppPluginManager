#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <boost/function.hpp>
#include "InstallStep.h"
#include "CopyStep.h"

#include "tstring.h"

using namespace std;


CopyStep::CopyStep(const TCHAR *from, const TCHAR *to, BOOL attemptReplace, BOOL validate)
{
	_from = from;
	_to = to;
	_failIfExists = !attemptReplace;
	_validate = validate;
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
	bool copy;

	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			dest = toPath;
			dest.append(foundData.cFileName);
			src = fromDir;
			src.append(foundData.cFileName);
			copy = false;
			if (_validate)
			{
				switch(ValidateFile(src))
				{
				
					case VALIDATE_OK:
						copy = true;
						break;

					case VALIDATE_UNKNOWN:
						tstring msg(_T("It has not been possible to validate the integrity of '"));
						msg.append(foundData.cFileName);
						msg.append(_T("' needed to install or update a plugin.  Do you want to copy this file anyway (not recommended)?"));
						
						int userChoice = ::MessageBox(NULL, msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);
						
						if (userChoice == IDYES)
						{
							copy = true;
						}
						else
						{
							status = STEPSTATUS_FAIL;
							copy = false;
						}



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
		} while(status != STEPSTATUS_FAIL && ::FindNextFile(hFindFile, &foundData));
	}

	::FindClose(hFindFile);

	return status;
}
