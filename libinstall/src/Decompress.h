#ifndef _DECOMPRESS_H
#define _DECOMPRESS_H


#include "unzip.h"
#include "tstring.h"
#include <windows.h>

class Decompress
{
public:
	Decompress();
	~Decompress();

	static BOOL unzip(const tstring& zipFile, const tstring& destDir);

private:
	static const int BUFFER_SIZE = 4096;

	static void setString(const tstring &src, std::string &dest);
};


#endif