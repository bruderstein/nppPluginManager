#include <boost/shared_ptr.hpp>
#include <windows.h>

using namespace boost;


shared_ptr<wchar_t> char2wchar(const char* mbStr)
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


shared_ptr<char> wchar2char(const wchar_t* wcStr)
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

	//static boost::shared_ptr<const TCHAR>   char2tchar(const char* mbStr);
	//static boost::shared_ptr<const char>    tchar2char(const TCHAR* tStr);
	