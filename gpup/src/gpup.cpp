/*
This file is part of GPUP, which is part of Plugin Manager 
Plugin for Notepad++

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

#include "stdafx.h"
#include "gpup.h"






#include "Options.h"
#include "tinyxml/tinyxml.h"
#include "libinstall/InstallStep.h"
#include "libinstall/InstallStepFactory.h"
#include "libinstall/VariableHandler.h"
#include "libinstall/CancelToken.h"
#include "libinstall/ModuleInfo.h"
#include "ProgressDialog.h"

#define RETURN_SUCCESS				0
#define RETURN_INVALID_PARAMETERS   1
#define RETURN_CANCELLED			2
#define RETURN_ACTIONERROR			3


#define MAX_WRITE_ATTEMPTS			10

// Global Variables:
HINSTANCE	   hInst;								// current instance
ProgressDialog *g_progressDialog;


using namespace std;
using namespace std::placeholders;


BOOL parseCommandLine(const TCHAR* cmdLine, Options& options)
{
	/* Command line options are
	 * -a <actionsFile.xml>
	 * -w <windowName>
	 * -e <exeToRun>
	 *
     * w and e are mandatory 
	 */

	if (!cmdLine)
		return TRUE;

	
	
	
	tstring* arg;
	arg = new tstring;

	list<tstring*> argList;

	enum PARSEMODE
	{
		PARSEMODE_START,
		PARSEMODE_STANDARD,
		PARSEMODE_QUOTED
	} parseMode;

	parseMode = PARSEMODE_START;

	while(*cmdLine)
	{

		switch(parseMode)
		{
			case PARSEMODE_START:
				if (*cmdLine == _T('\"'))
				{
					parseMode = PARSEMODE_QUOTED;
				}
				else if (*cmdLine != _T(' '))
				{
					arg->push_back(*cmdLine);
					parseMode = PARSEMODE_STANDARD;
				}
				
				break;


			case PARSEMODE_STANDARD:			
				if (*cmdLine == _T(' '))
				{
					parseMode = PARSEMODE_START;
					argList.push_back(arg);
					arg = new tstring;
				}
				else
				{
					arg->push_back(*cmdLine);
				}
				break;

			case PARSEMODE_QUOTED:
				if (*cmdLine == _T('\"'))
				{
					parseMode = PARSEMODE_START;
					argList.push_back(arg);
					arg = new tstring;
				}
				else
				{
					arg->push_back(*cmdLine);
				}
				break;

		}

		++cmdLine;
	}

	if (!arg->empty())
		argList.push_back(arg);
	
    options.setArgList(argList);
	
	list<tstring*>::iterator iter = argList.begin();
	while(iter != argList.end())
	{
		if (*(*iter) == _T("-w"))
		{
			++iter;
			if (iter != argList.end())
				options.setWindowName((*iter)->c_str());
		}
		else if (*(*iter) == _T("-e"))
		{
			++iter;
			if (iter != argList.end())
				options.setExeName((*iter)->c_str());
		}
		else if (*(*iter) == _T("-a"))
		{
			++iter;
			if (iter != argList.end())
				options.setActionsFile((*iter)->c_str());
		}
		else if (*(*iter) == _T("-c"))
		{
			++iter;
			if (iter != argList.end())
				options.setCopyFrom((*iter)->c_str());
		}
		else if (*(*iter) == _T("-t"))
		{
			++iter;
			if (iter != argList.end())
				options.setCopyTo((*iter)->c_str());
		}
        else if (*(*iter) == _T("-M"))
        {
            options.setIsAdmin(TRUE);
        }

		++iter;
	}



	return TRUE;

}


BOOL closeMainProgram(Options &options)
{
	HWND h = ::FindWindowEx(NULL, NULL, options.getWindowName().c_str(), NULL);
	
	DWORD dwPid;
	HANDLE hProc = NULL;

	while (h)
	{
		
		
		GetWindowThreadProcessId(h, &dwPid);
		if (dwPid) // Wait for process to terminate
		{
			hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwPid);
		}
		else 
		{
			hProc = NULL;
		}
		
		if (!::SendMessage(h, WM_CLOSE, 0, 0))
			return FALSE;

		if (hProc)
		{
			WaitForSingleObject(hProc, INFINITE);
		}

		h = ::FindWindowEx(NULL, NULL, options.getWindowName().c_str(), NULL);
	}

	if (hProc)
	{
		CloseHandle(hProc);
		hProc = NULL;
	}

	return TRUE;
}


void startNewInstance(const tstring& exeName)
{

	STARTUPINFO startup;
	memset(&startup, 0, sizeof(STARTUPINFO));

	startup.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION procinfo;
	
    ::CreateProcess((TCHAR *)exeName.c_str(),        
						NULL,         // arguments
						NULL,        // process security
						NULL,        // thread security
						FALSE,        // inherit handles flag
						NULL,           // flags
						NULL,        // inherit environment
						NULL,        // inherit directory
						&startup,    // STARTUPINFO
						&procinfo);  // PROCESS_INFORMATION

}




void showProgressDialog()
{
	g_progressDialog = new ProgressDialog(hInst);
	g_progressDialog->doDialog();
}

void closeProgressDialog()
{
    g_progressDialog->close();
}

void setStatus(const TCHAR* status)
{
	g_progressDialog->setStatus(status);
}

void setStepCount(int stepCount)
{
	g_progressDialog->setStepCount(stepCount);
}


void stepProgress(int /*percentageComplete*/)
{
	
}


BOOL processActionsFile(const tstring& actionsFile)
{
    ModuleInfo moduleInfo(::GetModuleHandle(NULL), NULL);

    CancelToken cancelToken;

	TiXmlDocument xmlDocument(actionsFile.c_str());
	if (xmlDocument.LoadFile())
	{
		TiXmlElement *install = xmlDocument.FirstChildElement(_T("install"));
		TiXmlElement stillToComplete(_T("install"));
		tstring basePath;

		if (install && !install->NoChildren())
		{
			VariableHandler variableHandler;
            
			InstallStepFactory installStepFactory(&variableHandler);
			TiXmlElement *step = install->FirstChildElement();
			int stepCount = 0;
			while (step)
			{
				stepCount++;
				step = static_cast<TiXmlElement*>(install->IterateChildren(step));
			} 

			setStepCount(stepCount);


			step = install->FirstChildElement();
			while (step)
			{
				std::shared_ptr<InstallStep> installStep = installStepFactory.create(step);
				// Progress to next step
				step = (TiXmlElement*) install->IterateChildren(step);

				// Not all steps are actually an install step, some are just setting variables
				if (installStep == NULL) {
					g_progressDialog->stepComplete();
					continue;
				}

				StepStatus stepStatus;
				stepStatus = installStep->perform(basePath,           // basePath
												&stillToComplete,   // forGpup (still can't achieve, so basically a fail)
												std::bind(&setStatus, _1),     // status update function
												std::bind(&stepProgress, _1),
												&moduleInfo,
												cancelToken); // step progress function

				// If it said it needed to do it in GPUP, then maybe N++ hasn't quite
				// finished quitting yet.
				int retryCount = 0;
				while (stepStatus == STEPSTATUS_NEEDGPUP && retryCount < 20)
				{
					::Sleep(500);
					++retryCount;
					stepStatus = installStep->perform(basePath,           // basePath
												&stillToComplete,   // forGpup (still can't achieve, so basically a fail)
												std::bind(&setStatus, _1),     // status update function
												std::bind(&stepProgress, _1), // step progress function
												&moduleInfo,
												cancelToken); 



				}

				g_progressDialog->stepComplete();				

			}

		}	

		::DeleteFile(actionsFile.c_str());

		return TRUE;
	}
	else
		return FALSE;

	

}

BOOL actionsFileHasActions(const tstring& actionsFile) 
{
    TiXmlDocument xmlDocument(actionsFile.c_str());
	if (xmlDocument.LoadFile())
	{
		TiXmlElement *install = xmlDocument.FirstChildElement(_T("install"));
		TiXmlElement stillToComplete(_T("install"));

		if (install && !install->NoChildren())
		{
            return TRUE;
        }
    }
    return FALSE;
}


DWORD runGpupAsAdmin(const Options& options) {
    const std::list<tstring*> args = options.getArgList();
    tstring parameters;
    
    
    for(std::list<tstring*>::const_iterator it = args.begin(); it != args.end(); ++it) {
        parameters.append(_T("\""));
        parameters.append(*(*it));
        parameters.append(_T("\" "));
    }

    parameters.append(_T("-M"));

    TCHAR gpupExeName[MAX_PATH];
    GetModuleFileName(0, gpupExeName, MAX_PATH);

    DWORD version = ::GetVersion();
    DWORD majorVersion = LOBYTE(version);

    BOOL isVistaOrGreater = (majorVersion >= 6);

    SHELLEXECUTEINFO shExecInfo = {0};
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = isVistaOrGreater ? _T("runas") : _T("open");
    shExecInfo.lpFile = gpupExeName;
    shExecInfo.lpParameters = parameters.c_str();   
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_SHOW;
    shExecInfo.hInstApp = NULL; 
    ShellExecuteEx(&shExecInfo);
    WaitForSingleObject(shExecInfo.hProcess,INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(shExecInfo.hProcess, &exitCode);
    return exitCode;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(nCmdShow);

	hInst = hInstance;

	Options options;
	parseCommandLine(lpCmdLine, options);

	// If this is just a copy request
	if (!options.getCopyFrom().empty() && !options.getCopyTo().empty())
	{
		BOOL copyStatus = CopyFile(options.getCopyFrom().c_str(), options.getCopyTo().c_str(), false);
		// We don't delete the old file, leave that to the caller
		// Return 0 for success, 1 for failure
		return copyStatus ? 0 : 1;
	}

	if (options.getWindowName() == _T("")
		|| options.getExeName() == _T(""))
	{
		::MessageBox(NULL, _T("Invalid parameters passed to the plugin updater (GPUP).\r\n")
						   _T("Usage:")
						   _T(" gpup.exe -w <window name> -e <exe name> [-a <actions file>]"), _T("Plugin Update"), MB_OK | MB_ICONERROR);
		return RETURN_INVALID_PARAMETERS;
	}

    showProgressDialog();
	
	setStatus(_T("Waiting for Notepad++ to close"));

	BOOL allClosed = closeMainProgram(options);

	if (!allClosed)
	{
		return RETURN_CANCELLED;
	}

	
    BOOL shouldRestartProcess = TRUE;
    DWORD returnValue = RETURN_SUCCESS;
	if (options.getActionsFile() != _T(""))
	{
        if (!options.isAdmin() && actionsFileHasActions(options.getActionsFile())) 
        {
            closeProgressDialog();
             returnValue = runGpupAsAdmin(options);
        }
        else 
        {
            shouldRestartProcess = FALSE;
            if (!processActionsFile(options.getActionsFile())) 
            {
			    MessageBox(NULL, _T("Error finishing installation steps.  Plugin installation has not completed successfully."), _T("Plugin Manager"), MB_OK | MB_ICONERROR);
            }
		}
	}

    if (shouldRestartProcess) 
    {
	    startNewInstance(options.getExeName());
    }
	

	return returnValue;

}
