#include <string>
#define ZLIB_WINAPI
#include "Decompress.h"

#include "unzip.h"
#include "tstring.h"

using namespace std;

BOOL Decompress::unzip(const tstring &zipFile, const tstring &destDir)
{
	string zip;
	setString(zipFile, zip);
	
	unzFile hZip = unzOpen(zip.c_str());
	
	if (unzGoToFirstFile(hZip) != UNZ_OK)
	{
		unzClose(hZip);
		return FALSE;
	}

	int nextFileResult;

	do {
		if (unzOpenCurrentFile(hZip) != UNZ_OK)
		{
			unzClose(hZip);
			return FALSE;
		}

		char filename[MAX_PATH];
		if (unzGetCurrentFileInfo(hZip, NULL, filename, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK)
		{
			unzClose(hZip);
			return FALSE;
		}


		char buffer[BUFFER_SIZE];
		int bytesRead;
		
		FILE *fp;
		string outputFilename;
		setString(destDir, outputFilename);
		outputFilename.append(filename);
		fp = fopen(outputFilename.c_str(), "wb");

		do
		{
			bytesRead = unzReadCurrentFile(hZip, buffer, BUFFER_SIZE);
			
			if (bytesRead > 0)
				fwrite(buffer, bytesRead, 1, fp);
		
		} while(bytesRead > 0);
		
		unzCloseCurrentFile(hZip);

		nextFileResult = unzGoToNextFile(hZip);
		
	} while (nextFileResult == UNZ_OK);

	unzClose(hZip);

	return TRUE;
}


void Decompress::setString(const tstring& src, string &dest)
{
#ifdef _UNICODE
	int len = src.size() * 2;
	char *tmpBuf = new char[len];
	
    size_t newSize = wcstombs(tmpBuf, src.c_str(), len); 
	tmpBuf[newSize] = '\0';
	dest = tmpBuf;
	delete[] tmpBuf;
#else
	dest = src;
#endif
}