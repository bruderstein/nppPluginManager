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

#ifndef _COPYSTEP_H
#define _COPYSTEP_H
#include <windows.h>
#include <string>
#include "InstallStep.h"
#include <tchar.h>
#include "tstring.h"

#define VALIDATE_BASEURL          _T("http://npppm.brotherstone.co.uk/validate.php?md5=")
#define VALIDATE_RESULT_OK        "ok"
#define VALIDATE_RESULT_UNKNOWN   "unknown"
#define VALIDATE_RESULT_BANNED    "banned"

enum ValidateStatus
{
	VALIDATE_OK,
	VALIDATE_UNKNOWN,
	VALIDATE_BANNED
};



class CopyStep : public InstallStep
{
public:
	CopyStep(const TCHAR* from, const TCHAR* to, BOOL attemptReplace, BOOL validate,
		BOOL backup, const char* proxy, const long proxyPort);

	~CopyStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		 boost::function<void(const TCHAR*)> setStatus,
	     boost::function<void(const int)> stepProgress, const HWND windowParent);

private:
	
	ValidateStatus Validate(tstring& file);


	tstring	_from;
	tstring _to;
	
	std::string _proxy;
	long    _proxyPort;
	
	BOOL    _failIfExists;
	BOOL    _validate;
	BOOL	_backup;
};

#endif
