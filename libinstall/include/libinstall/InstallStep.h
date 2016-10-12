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
#ifndef _INSTALLSTEP_H
#define _INSTALLSTEP_H

#include "tstring.h"
#include "tinyxml/tinyxml.h"

class VariableHandler;
class ModuleInfo;
class CancelToken;

enum StepStatus 
{
	STEPSTATUS_SUCCESS,
	STEPSTATUS_NEEDGPUP,
	STEPSTATUS_FAIL
};



class InstallStep
{
public:
	InstallStep() {};
	~InstallStep() {};

	virtual StepStatus perform(tstring &/*basePath*/, TiXmlElement* /*forGpup*/, 
		std::function<void(const TCHAR*)> /* setStatus */,
		std::function<void(const int)> /* stepProgress */,
		const ModuleInfo* /*windowParent */,
        CancelToken& /* cancelToken */) { return STEPSTATUS_SUCCESS; };

	virtual StepStatus remove(tstring &/*basePath*/, TiXmlElement* /*forGpup*/, 
		std::function<void(const TCHAR*)> /* setStatus */,
		std::function<void(const int)> /* stepProgress */,
		const ModuleInfo* /*windowParent */) { return STEPSTATUS_SUCCESS; };

	virtual void replaceVariables(VariableHandler* /*variableHandler*/) { };

protected:
//	void setTstring(const char *src, tstring &dest);

};

#endif