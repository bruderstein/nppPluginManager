#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <boost/function.hpp>
#include "libinstall/InstallStep.h"
#include "libinstall/CopyStep.h"
#include "libinstall/DownloadManager.h"
#include "libinstall/md5.h"
#include "libinstall/tstring.h"

using namespace std;


CopyStep::CopyStep(const TCHAR *from, const TCHAR *to, BOOL attemptReplace, BOOL validate, const char* proxy, long proxyPort)
{
	_from = from;
	_to = to;
	_failIfExists = !attemptReplace;
	_validate = validate;
	_proxy = proxy;
	_proxyPort = proxyPort;
}

ValidateStatus CopyStep::Validate(tstring& file)
{
	DownloadManager download;
	TCHAR localMD5[MD5::HASH_LENGTH + 1];
	MD5::hash(file.c_str(), localMD5, MD5::HASH_LENGTH + 1);
	tstring validateUrl = VALIDATE_BASEURL;
	validateUrl.append(localMD5);
	string validateResult;
	if (download.getUrl(validateUrl.c_str(), validateResult, "", 0))
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
							break;
						}


					case VALIDATE_BANNED:
						{
							tstring msg(_T("'"));
							msg.append(foundData.cFileName);
							msg.append(_T("' has been identified as unstable, incorrect or dangerous.  It is NOT recommended you install this file.  Do you want to install this file anyway?"));
							
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
							break;
						}
						


				}
			}
			else 
				copy = true;

			if (copy && !::CopyFile(src.c_str(), dest.c_str(), _failIfExists))
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
