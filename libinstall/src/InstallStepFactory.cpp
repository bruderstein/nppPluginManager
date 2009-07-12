/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009 Dave Brotherstone <davegb@pobox.com>

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
#include "libinstall/InstallStepFactory.h"
#include "libinstall/DownloadStep.h"
#include "libinstall/CopyStep.h"
#include "libinstall/DeleteStep.h"

#include "libinstall/tstring.h"
#include "libinstall/VariableHandler.h"

using namespace boost;
using namespace std;

InstallStepFactory::InstallStepFactory(VariableHandler* variableHandler)
{
	_variableHandler = variableHandler;
}



shared_ptr<InstallStep> InstallStepFactory::create(TiXmlElement* element, const char* proxy, long proxyPort)
{
	shared_ptr<InstallStep> installStep;

	if (!_tcscmp(element->Value(), _T("download")) && element->FirstChild())
	{
		installStep.reset(new DownloadStep(element->FirstChild()->Value(), element->Attribute(_T("filename")), proxy, proxyPort));
	}
	else if (!_tcscmp(element->Value(), _T("copy")))
	{
		const TCHAR *tFrom = element->Attribute(_T("from"));
		const TCHAR *tTo = element->Attribute(_T("to"));
		const TCHAR *tReplace = element->Attribute(_T("replace"));
		const TCHAR *tValidate = element->Attribute(_T("validate"));
		const TCHAR *tBackup = element->Attribute(_T("backup"));
		
		BOOL attemptReplace = FALSE;
		BOOL validate		= FALSE;
		BOOL backup			= FALSE;

		if (tReplace && !_tcscmp(tReplace, _T("true")))
			attemptReplace = TRUE;

		if (tValidate && !_tcscmp(tValidate, _T("true")))
			validate = TRUE;

		if (tBackup && !_tcscmp(tBackup, _T("true")))
			backup = TRUE;


		tstring from;
		tstring to;
		if (tFrom)
			from = tFrom;
		if (tTo)
			to = tTo;


		if (_variableHandler)
		{
			_variableHandler->replaceVariables(from);
			_variableHandler->replaceVariables(to);
		}

		installStep.reset(new CopyStep(from.c_str(), to.c_str(), attemptReplace, validate, backup, proxy, proxyPort));
	}
	else if (!_tcscmp(element->Value(), _T("delete")))
	{
		const TCHAR *tFile = element->Attribute(_T("file"));
		
		// If no file attribute specified, just return a null step
		if (!tFile)
			return installStep;

		tstring file;

		file = tFile;
		if (_variableHandler)
		{
			_variableHandler->replaceVariables(file);
		}

		installStep.reset(new DeleteStep(file.c_str()));
	}

	return installStep;
}



