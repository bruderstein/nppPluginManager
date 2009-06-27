#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "tstring.h"

class Options
{
public:
	Options()  {};
	~Options() {};

	void setActionsFile(const TCHAR* actionsFile);
	void setWindowName(const TCHAR* windowName);
	void setExeName(const TCHAR* exeName);

	const tstring& getActionsFile();
	const tstring& getExeName();
	const tstring& getWindowName();

private: 
	tstring _actionsFile;
	tstring _windowName;
	tstring _exeName;

	
};

#endif