#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#include <tchar.h>
#include "md5.h"




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
        return FALSE;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        return FALSE;
    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, 
		&cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            return FALSE;
        }
    }

    if (!bResult)
    {
        return FALSE;
    }

    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        TCHAR *currentHashBuffer = hashBuffer;
        for (DWORD i = 0; i < cbHash; i++)
        {
            _stprintf(currentHashBuffer, _T("%02x"), rgbHash[i]);
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