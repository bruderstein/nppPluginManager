#include <string>
#include <boost/shared_ptr.hpp>
#define ZLIB_WINAPI
#include "Decompress.h"
#include "WcharMbcsConverter.h"
#include "unzip.h"
#include "tstring.h"

using namespace std;
using namespace boost;

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


		if (filename[strlen(filename) - 1] == '/')
		{
			string outputDir;
			setString(destDir, outputDir);
			outputDir.append(filename);
			outputDir.erase(outputDir.size() - 1);
			::CreateDirectoryA(outputDir.c_str(), NULL);
		}
		else
		{

			char buffer[BUFFER_SIZE];
			int bytesRead;
			
			FILE *fp;
			string outputFilename;

			setString(destDir, outputFilename);
			outputFilename.append(filename);
			fopen_s(&fp, outputFilename.c_str(), "wb");

			do
			{
				bytesRead = unzReadCurrentFile(hZip, buffer, BUFFER_SIZE);
				
				if (bytesRead > 0)
					fwrite(buffer, bytesRead, 1, fp);
			
			} while(bytesRead > 0);
			
			unzCloseCurrentFile(hZip);
			fclose(fp);
		}
		nextFileResult = unzGoToNextFile(hZip);
		
	} while (nextFileResult == UNZ_OK);

	unzClose(hZip);

	return TRUE;
}


void Decompress::setString(const tstring &src, std::string &dest)
{
	shared_ptr<char> cDest = WcharMbcsConverter::tchar2char(src.c_str());
	dest = cDest.get();
}