/*
This file is part of Plugin Manager Plugin for Notepad++

Copyright (C)2009-2010 Dave Brotherstone <davegb@pobox.com>

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
#include "precompiled_headers.h"
#include "libinstall/Decompress.h"
#include "libinstall/WcharMbcsConverter.h"
#include "libinstall/tstring.h"
#include "libinstall/DirectoryUtil.h"

#include "unzip.h"
#include "iowin32.h"

using namespace std;

BOOL Decompress::unzip(const tstring &zipFile, const tstring &destDir)
{

	zlib_filefunc_def filefunc;
	fill_win32_filefunc(&filefunc);
	unzFile hZip = unzOpen2(zipFile.c_str(), &filefunc);

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

		std::shared_ptr<TCHAR> tFilename = WcharMbcsConverter::char2tchar(filename);


		if ((tFilename.get())[_tcslen(tFilename.get()) - 1] == _T('/'))
		{
			tstring outputDir(destDir);

			outputDir.append(tFilename.get());
			outputDir.erase(outputDir.size() - 1);
			::CreateDirectory(outputDir.c_str(), NULL);
			unzCloseCurrentFile(hZip);
		}
		else
		{

			FILE *fp = NULL;
			tstring outputFilename (destDir);

			outputFilename.append(tFilename.get());

			tstring::size_type pos = outputFilename.find_first_of(_T('/'));
			// Replace all the forward slashes with backward ones
			while (pos != string::npos)
			{

				outputFilename.replace(pos, 1, 1, _T('\\'));
				pos = outputFilename.find_first_of(_T('/'), pos);
			}

			// Now grab the directory name of the output
			pos = outputFilename.find_last_of(_T('\\'));

			if (pos != tstring::npos)
			{
				// If it doesn't exist, create it (and its parents)
				tstring outputDir = tstring(outputFilename, 0, pos);
				if (!::PathFileExists(outputDir.c_str()))
				{
					DirectoryUtil::createDirectories(outputDir.c_str());
				}
			}

			if (_tfopen_s(&fp, outputFilename.c_str(), _T("wb")) == 0)
			{
				char buffer[BUFFER_SIZE];
				int bytesRead;

				do
				{
					bytesRead = unzReadCurrentFile(hZip, buffer, BUFFER_SIZE);

					if (bytesRead > 0 && fp)
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
			if (fp)
			{
				fclose(fp);
			}
		}
		nextFileResult = unzGoToNextFile(hZip);

	} while (nextFileResult == UNZ_OK);

	unzClose(hZip);

	return TRUE;
}


void Decompress::setString(const tstring &src, std::string &dest)
{
	std::shared_ptr<char> cDest = WcharMbcsConverter::tchar2char(src.c_str());
	dest = cDest.get();
}