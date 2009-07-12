/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009 Dave Brotherstone <davegb@pobox.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <string>
#include <shlwapi.h>
#include <boost/shared_ptr.hpp>
#include "libinstall/Decompress.h"
#include "libinstall/WcharMbcsConverter.h"
#include "libinstall/tstring.h"
#include "libinstall/DirectoryUtil.h"

#include "unzip.h"


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
			unzCloseCurrentFile(hZip);
		}
		else
		{

			char buffer[BUFFER_SIZE];
			int bytesRead;
			
			FILE *fp = NULL;
			string outputFilename;

			setString(destDir, outputFilename);
			outputFilename.append(filename);
			
			string::size_type pos = outputFilename.find_first_of(_T('/'));
			// Replace all the forward slashes with backward ones 
			while (pos != string::npos)
			{

				outputFilename.replace(pos, 1, 1, _T('\\'));
				pos = outputFilename.find_first_of(_T('/'), pos);
			}
			
			// Now grab the directory name of the output
			pos = outputFilename.find_last_of(_T('\\'));

			if (pos != string::npos)
			{
				// If it doesn't exist, create it (and its parents)
				string outputDir = string(outputFilename, 0, pos);
				if (!::PathFileExistsA(outputDir.c_str()))
				{
					shared_ptr<TCHAR> outputDirToCreate = WcharMbcsConverter::char2tchar(outputDir.c_str());
					DirectoryUtil::createDirectories(outputDirToCreate.get());
				}
			}

			if (fopen_s(&fp, outputFilename.c_str(), "wb") == 0)
			{
				do
				{
					bytesRead = unzReadCurrentFile(hZip, buffer, BUFFER_SIZE);
					
					if (bytesRead > 0)
						fwrite(buffer, bytesRead, 1, fp);
				
				} while(bytesRead > 0);
			}
			else
			{
				// Opening output file failed, so close the zip and fail the step
				unzClose(hZip);
				return FALSE;
			}

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