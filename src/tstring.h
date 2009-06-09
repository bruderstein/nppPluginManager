
#ifndef _TSTRING_H
#define _TSTRING_H

typedef std::basic_string<TCHAR>			tstring;

#ifdef _UNICODE
#define generic_string wstring
#else
#define generic_string string
#endif




#endif
