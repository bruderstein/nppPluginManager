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
#include "libinstall/InstallStepFactory.h"
#include "libinstall/DownloadStep.h"
#include "libinstall/CopyStep.h"
#include "libinstall/DeleteStep.h"
#include "libinstall/RunStep.h"
#include "libinstall/tstring.h"
#include "libinstall/VariableHandler.h"


using namespace std;

InstallStepFactory::InstallStepFactory(VariableHandler* variableHandler)
{
	_variableHandler = variableHandler;
}



std::shared_ptr<InstallStep> InstallStepFactory::create(TiXmlElement* element)
{
	std::shared_ptr<InstallStep> installStep;

	if (!_tcscmp(element->Value(), _T("download")) && element->FirstChild())
	{
		installStep.reset(new DownloadStep(element->FirstChild()->Value(), element->Attribute(_T("filename"))));
	}
	else if (!_tcscmp(element->Value(), _T("copy")))
	{
		const TCHAR *tFrom = element->Attribute(_T("from"));
		const TCHAR *tTo = element->Attribute(_T("to"));
		const TCHAR *tToFile = element->Attribute(_T("toFile"));

		const TCHAR *tReplace = element->Attribute(_T("replace"));
		const TCHAR *tValidate = element->Attribute(_T("validate"));
		const TCHAR *tBackup = element->Attribute(_T("backup"));
		const TCHAR *tIsGpup = element->Attribute(_T("isGpup"));
		const TCHAR *tRecursive = element->Attribute(_T("recursive"));

		BOOL attemptReplace = FALSE;
		BOOL validate		= FALSE;
		BOOL backup			= FALSE;
		BOOL isGpup         = FALSE;
		BOOL recursive	    = FALSE;

		if (tReplace && !_tcscmp(tReplace, _T("true")))
			attemptReplace = TRUE;

		if (tValidate && !_tcscmp(tValidate, _T("true")))
			validate = TRUE;

		if (tBackup && !_tcscmp(tBackup, _T("true")))
			backup = TRUE;

		if (tIsGpup && !_tcscmp(tIsGpup, _T("true")))
			isGpup = TRUE;

		if (tRecursive && !_tcscmp(tRecursive, _T("true")))
			recursive = TRUE;

        tstring validateUrl;
        if (_variableHandler) {
            validateUrl = _variableHandler->getVariable(VALIDATE_BASE_URL_VAR);
		}
		installStep.reset(new CopyStep(tFrom, tTo, tToFile, attemptReplace, validate, isGpup, backup, recursive, validateUrl));
	}
	else if (!_tcscmp(element->Value(), _T("delete")))
	{
		const TCHAR *tFile = element->Attribute(_T("file"));
		
		// If no file attribute specified, just return a null step
		if (!tFile)
			return installStep;

		BOOL isDirectory = FALSE;
		
		const TCHAR *tIsDir = element->Attribute(_T("isDirectory"));
		
		if (tIsDir && !_tcsicmp(tIsDir, _T("true")))
		{
			isDirectory = TRUE;
		}

		installStep.reset(new DeleteStep(tFile, isDirectory));
	}
	else if (!_tcscmp(element->Value(), _T("run")))
	{
		const TCHAR *tFile = element->Attribute(_T("file"));
		const TCHAR *tArgs = element->Attribute(_T("arguments"));
		const TCHAR *tOutsideNpp = element->Attribute(_T("outsideNpp"));

		BOOL outsideNpp = FALSE;
		if (tOutsideNpp && !_tcsicmp(tOutsideNpp, _T("true")))
		{
			outsideNpp = TRUE;
		}

        tstring validateUrl;
        if (_variableHandler) {
            validateUrl = _variableHandler->getVariable(VALIDATE_BASE_URL_VAR);
		}
		installStep.reset(new RunStep(tFile, tArgs, outsideNpp, validateUrl));
	}
	else if (!_tcscmp(element->Value(), _T("setVariable")))
	{
        // Maybe an option to allow this?
        _variableHandler->setVariable(element->Attribute(_T("name")), element->Attribute(_T("value")));
	}


	return installStep;
}



