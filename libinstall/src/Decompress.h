#ifndef _DECOMPRESS_H
#define _DECOMPRESS_H


#include "unzip.h"
#include <windows.h>

class Decompress
{
	Decompress();
	~Decompress();

	static BOOL unzip(const char* zipFile, const char* destDir);
};


#endif