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
#ifndef _RUNSTEP_H
#define _RUNSTEP_H
#include <windows.h>
#include <string>
#include "InstallStep.h"
#include <tchar.h>
#include "tstring.h"
#include "validate.h"

class RunStep : public InstallStep
{
public:
	RunStep(const TCHAR* file, const TCHAR* arguments, BOOL outsideNpp, const CHAR* proxy, const long proxyPort);
	~RunStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		boost::function<void(const TCHAR*)> setStatus,
		boost::function<void(const int)> stepProgress, const HWND windowParent);

	void replaceVariables(VariableHandler *variableHandler);

private:
	BOOL execute(const TCHAR *executable, const TCHAR *arguments);


	std::string _proxy;
	long    _proxyPort;
	BOOL	_outsideNpp;
	tstring	_file;
	tstring _arguments;
};

#endif
