
#include <memory>
#include <string>
#include <tchar.h>

#include <iostream>
#include <fstream>
#include <limits.h>
#include <map>
#include <set>
#include <list>

#include <shlwapi.h>
#include <commctrl.h>
#include <process.h>




#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WindowsX.h>

#include <strsafe.h>
typedef std::basic_string<TCHAR>			tstring;



#pragma warning (push)
#pragma warning (disable : 4512) // assignment operator could not be generated
#include <boost/bind.hpp>
#pragma warning (pop)

#include <boost/function.hpp>
