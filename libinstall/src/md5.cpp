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
#include <Wincrypt.h>
#include "libinstall/md5.h"




BOOL MD5::hash(const TCHAR *filename, TCHAR *hashBuffer, int hashBufferLength)
{
    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    
	if (hashBufferLength < ((MD5LEN * 2) + 1))
		return FALSE;
    
    hFile = CreateFile(filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        return FALSE;
    }

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        CloseHandle(hFile);
        return FALSE;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
		CryptReleaseContext(hProv, 0);
        CloseHandle(hFile);
        return FALSE;
    }

	
    do
    {
		bResult =  ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL);
		if (!bResult || 0 == cbRead)
			break;
        

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            return FALSE;
        }
	} while (bResult);
	
	if (bResult == FALSE)
	{
		CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
		return FALSE;
	}

    
    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        TCHAR *currentHashBuffer = hashBuffer;
        for (DWORD i = 0; i < cbHash; i++)
        {
            _stprintf_s(currentHashBuffer, hashBufferLength - (i * 2), _T("%02x"), rgbHash[i]);
			currentHashBuffer += 2;
        }
        dwStatus = TRUE;
    }
    else
    {
        dwStatus = FALSE;
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return dwStatus; 
}