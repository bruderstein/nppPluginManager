#include <boost/shared_ptr.hpp>
#include <tchar.h>


class WcharMbcsConverter {
public:
	
	static boost::shared_ptr<wchar_t> char2wchar(const char* mbStr);
	static boost::shared_ptr<char>    wchar2char(const wchar_t* wcStr);

//	static boost::shared_ptr<const TCHAR>   char2tchar(const char* mbStr);
//	static boost::shared_ptr<const char>    tchar2char(const TCHAR* tStr);


};