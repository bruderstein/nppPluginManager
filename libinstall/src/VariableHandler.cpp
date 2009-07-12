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
#include "libinstall/VariableHandler.h"
#include "libinstall/tstring.h"

using namespace std;


VariableHandler::VariableHandler(const TCHAR *nppDir, const TCHAR *pluginDir, const TCHAR *configDir)
	 : _nppDir(nppDir), 
	   _pluginDir(pluginDir),
	   _configDir(configDir),
	   VAR_NPPDIR(_T("$NPPDIR$")),
	   VAR_PLUGINDIR(_T("$PLUGINDIR$")),
	   VAR_CONFIGDIR(_T("$CONFIGDIR$"))
{
}


void VariableHandler::replaceVariables(tstring &source)
{
	tstring::size_type p = source.find(VAR_PLUGINDIR);
	if (p != tstring::npos)
		source.replace(p, VAR_PLUGINDIR.size(), _pluginDir);

	p = source.find(VAR_NPPDIR);
	if (p != string::npos)
		source.replace(p, VAR_NPPDIR.size(), _nppDir);

	p = source.find(VAR_CONFIGDIR);
	if (p != string::npos)
		source.replace(p, VAR_CONFIGDIR.size(), _configDir);
}

const tstring& VariableHandler::getConfigDir()
{
	return _configDir;
}

const tstring& VariableHandler::getNppDir()
{
	return _nppDir;
}
	
