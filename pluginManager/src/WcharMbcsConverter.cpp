#include <boost/shared_ptr.hpp>
#include <windows.h>
#include "WcharMbcsConverter.h"
using namespace boost;


shared_ptr<wchar_t> WcharMbcsConverter::char2wchar(const char* mbStr)
{
	
	shared_ptr<wchar_t> wideCharStr;

	int len = ::MultiByteToWideChar(CP_UTF8, 0, mbStr, -1, NULL, 0);
	
	
	if (len > 0)
	{
		wideCharStr.reset(new wchar_t[len]);
		MultiByteToWideChar(CP_UTF8, 0, mbStr, -1, wideCharStr.get(), len);
	}
	else
	{
		wideCharStr.reset(new wchar_t[1]);
		wideCharStr.get()[0] = 0;
	}

	return wideCharStr;
}


shared_ptr<char> WcharMbcsConverter::wchar2char(const wchar_t* wcStr)
{

	shared_ptr<char> multiByteStr;

	int len = WideCharToMultiByte(CP_UTF8, 0, wcStr, -1, NULL, 0, NULL, NULL);

	if (len > 0)
	{
		multiByteStr.reset(new char[len]);
		WideCharToMultiByte(CP_UTF8, 0, wcStr, -1, multiByteStr.get(), len, NULL, NULL);
	}
	else
	{
		multiByteStr.reset(new char[1]);
		multiByteStr.get()[0] = 0;
	}

	return multiByteStr;
}


shared_ptr<TCHAR> WcharMbcsConverter::char2tchar(const char* mbStr)
{
#ifdef _UNICODE
	return char2wchar(mbStr);
#else
	int len = strlen(mbStr) + 1;
	shared_ptr<TCHAR> result(new TCHAR[len]);
	strcpy_s(result.get(), len, mbStr);
	return result;
#endif
}


shared_ptr<char> WcharMbcsConverter::tchar2char(const TCHAR* tStr)
{
#ifdef _UNICODE
	return wchar2char(tStr);
#else
	int len = _tcslen(tStr) + 1;
	shared_ptr<TCHAR> result(new TCHAR[len]);
	strcpy_s(result.get(), len, tStr);
	return result;
#endif
}