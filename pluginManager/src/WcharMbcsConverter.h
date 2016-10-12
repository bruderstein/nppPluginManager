

class WcharMbcsConverter {
public:
	
	static std::shared_ptr<wchar_t> char2wchar(const char* mbStr);
	static std::shared_ptr<char>    wchar2char(const wchar_t* wcStr);
	
	static std::shared_ptr<TCHAR>   char2tchar(const char* mbStr);
	static std::shared_ptr<char>    tchar2char(const TCHAR* tStr);


};