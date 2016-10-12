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
#include "libinstall/RunStep.h"
#include "libinstall/VariableHandler.h"
#include "libinstall/tstring.h"
#include "libinstall/Validate.h"
#include "libinstall/ModuleInfo.h"

using namespace std;


RunStep::RunStep(const TCHAR *file, const TCHAR *arguments, BOOL outsideNpp, const tstring& validateBaseUrl)
	: _file(file), 
	  _arguments( arguments ? arguments : _T("")),
	  _outsideNpp(outsideNpp),
      _validateBaseUrl(validateBaseUrl)
{
	
}


StepStatus RunStep::perform(tstring& basePath, TiXmlElement*  forGpup, 
							 std::function<void(const TCHAR*)> setStatus,
							 std::function<void(const int)> stepProgress, 
							 const ModuleInfo* moduleInfo,
                             CancelToken& cancelToken)
{
	StepStatus status = STEPSTATUS_FAIL;

	if (_outsideNpp)
	{
		TiXmlElement* runElement = new TiXmlElement(_T("run"));
		
		tstring fullFilePath(basePath);
		fullFilePath.append(_file);

		runElement->SetAttribute(_T("file"), fullFilePath.c_str());
		
		runElement->SetAttribute(_T("arguments"), _arguments.c_str());
		
		forGpup->LinkEndChild(runElement);

		return STEPSTATUS_NEEDGPUP;
	}


	tstring statusString = _T("Running ");
	statusString.append(_file);
	setStatus(statusString.c_str());
	

	if(_file.find(_T("..")) != tstring::npos)
	{
		MessageBox(moduleInfo->getHParent(), _T("The executable path may not be within the sandbox area - this is dangerous and hence not permitted.  If you are seeing this message, please report it on the Notepad++ forums."), _T("Notepad++ Plugin Manager"), MB_ICONEXCLAMATION | MB_OK);
		return STEPSTATUS_FAIL;
	}

	
	tstring executable(basePath);
	executable.append(_file);
    BOOL executeFile = FALSE;
	switch(Validator::validate(_validateBaseUrl, executable, cancelToken, moduleInfo))
	{
	
		case VALIDATE_OK:
			executeFile = TRUE;
			break;

		case VALIDATE_UNKNOWN:
			{
				tstring msg(_T("It has not been possible to validate the integrity of '"));
				msg.append(_file);
				msg.append(_T("' needed to install or update a plugin.  Do you want to EXECUTE this file anyway (highly not recommended)?"));
				
				int userChoice = ::MessageBox(moduleInfo->getHParent(), msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);
				
				if (userChoice == IDYES)
				{
					executeFile = TRUE;
				}
				else
				{
					status = STEPSTATUS_FAIL;
					executeFile = FALSE;
				}
				break;
			}


		case VALIDATE_BANNED:
			{
				tstring msg(_T("'"));
				msg.append(_file);
				msg.append(_T("' has been identified as unstable, incorrect or dangerous.  It is NOT recommended you EXECUTE this file.  Do you want to EXECUTE this file anyway?"));
				
				int userChoice = ::MessageBox(moduleInfo->getHParent(), msg.c_str(), _T("Plugin Manager"), MB_ICONWARNING | MB_YESNO);
				
				if (userChoice == IDYES)
				{
					executeFile = TRUE;
				}
				else
				{
					status = STEPSTATUS_FAIL;
					executeFile = FALSE;
				}
				break;
			}
			


	}


	if (executeFile && execute(executable.c_str(), _arguments.c_str()))
	{
		status = STEPSTATUS_SUCCESS;
		MessageBox(moduleInfo->getHParent(), _T("Press OK when the installation program has completed."), _T("Notepad++ Plugin Manager"), MB_OK | MB_ICONQUESTION);
	}

	return status;
}



BOOL RunStep::execute(const TCHAR *executable, const TCHAR *arguments)
{
	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = executable;
	sei.lpParameters = arguments;
	sei.lpVerb = _T("open");
	sei.nShow = SW_SHOW;

	if (::ShellExecuteEx(&sei))
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
		return TRUE;
	}
	
	return FALSE;
}




void RunStep::replaceVariables(VariableHandler *variableHandler)
{
	if (variableHandler)
	{
		variableHandler->replaceVariables(_file);
		variableHandler->replaceVariables(_arguments);
	}

}

