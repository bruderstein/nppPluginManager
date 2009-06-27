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

const tstring& Options::getActionsFile()
{
	return _actionsFile;
}

const tstring& Options::getExeName()
{
	return _exeName;
}

const tstring& Options::getWindowName()
{
	return _windowName;
}