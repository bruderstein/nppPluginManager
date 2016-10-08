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
#include "libinstall/VariableHandler.h"

using namespace std;


VariableHandler::VariableHandler() 
{
	_variables = new map<tstring, tstring>();
}

void VariableHandler::setVariable(const TCHAR *variableName, const TCHAR *value)
{
	tstring tVariableName = variableName;
	(*_variables)[tVariableName] = value;
	
}

void VariableHandler::replaceVariables(tstring &source)
{
	tstring::size_type startPos, endPos;
	startPos = 0;
	endPos = tstring::npos;

	do 
	{
		startPos = source.find(_T('$'), startPos);
		if (startPos != tstring::npos)
		{
			endPos = source.find(_T('$'), startPos + 1);
			if (endPos == tstring::npos)
				break;
		}

		if (endPos != tstring::npos)
		{
			tstring varValue = (*_variables)[source.substr(startPos + 1, endPos - startPos - 1)];
			source.replace(startPos, endPos - startPos + 1, varValue);
			startPos = startPos + varValue.size();
			endPos = tstring::npos;
		}


	} while(startPos != tstring::npos);

	

}


const tstring& VariableHandler::getVariable(const TCHAR* variableName)
{
	return (*_variables)[tstring(variableName)];
}

const VariableHandler::iterator VariableHandler::begin() {
    return _variables->begin();
}

const VariableHandler::iterator VariableHandler::end() {
    return _variables->end();
}