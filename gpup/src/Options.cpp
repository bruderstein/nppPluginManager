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
#include "Options.h"

void Options::setActionsFile(const TCHAR* actionsFile)
{
	_actionsFile = actionsFile;
}

void Options::setExeName(const TCHAR* exeName)
{
	_exeName = exeName;
}

void Options::setWindowName(const TCHAR* windowName)
{
	_windowName = windowName;
}

void Options::setCopyFrom(const TCHAR* copyFrom)
{
	_copyFrom = copyFrom;
}

void Options::setCopyTo(const TCHAR* copyTo)
{
	_copyTo = copyTo;
}


void Options::setArgList(const std::list<tstring*>& argList) 
{
    _argList = argList;
}

void Options::setIsAdmin(const BOOL isAdmin) 
{
    _isAdmin = isAdmin;
}


const tstring& Options::getActionsFile() const
{
	return _actionsFile;
}

const tstring& Options::getExeName() const
{
	return _exeName;
}

const tstring& Options::getWindowName() const 
{
	return _windowName;
}

const tstring& Options::getCopyFrom() const 
{
	return _copyFrom;
}

const tstring& Options::getCopyTo() const 
{
	return _copyTo;
}

const std::list<tstring*>& Options::getArgList() const
{
    return _argList;
}

const BOOL Options::isAdmin() const 
{
    return _isAdmin;
}