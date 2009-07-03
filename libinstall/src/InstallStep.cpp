#include "InstallStep.h"
/*

void InstallStep::setTstring(const char *src, tstring &dest)
{
#ifdef _UNICODE
	TCHAR *tmpBuf = new TCHAR[strlen(src) + 1];
	int len = strlen(src); 
    size_t newSize = mbstowcs(tmpBuf, src, len); 
	tmpBuf[newSize] = '\0';
	dest = tmpBuf;
	delete[] tmpBuf;
#else
	dest = src;
#endif
}
*/