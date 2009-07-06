#include <tchar.h>
#include <windows.h>
#include "tstring.h"


class Utility
{
public:
	static BOOL removeDirectory(const TCHAR* directory);
	static void startGpup(const TCHAR* nppDir, const TCHAR* arguments);
	
};

