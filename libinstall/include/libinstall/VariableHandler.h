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
#ifndef _VARIABLEHANDLER_H
#define _VARIABLEHANDLER_H


class VariableHandler
{
public:
	VariableHandler();
		
	void setVariable(const TCHAR* variableName, const TCHAR* value);
	void replaceVariables(tstring &source);

	const tstring& getVariable(const TCHAR* variableName);
    typedef std::map<tstring, tstring>::iterator iterator;

    const iterator begin();
    const iterator end();

private:
	std::map<tstring, tstring> *_variables;

};



#endif
