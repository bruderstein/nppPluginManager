
#include "precompiled_headers.h"

#include "Encrypter.h"



int Encrypter::encrypt(unsigned const char *keyBuffer, const int keyLength, unsigned char *encKeyBuffer, const int encKeyBufferLength)
{
	int retVal = 0;

	DATA_BLOB pDataOut;
	DATA_BLOB pDataIn;
	pDataIn.cbData = keyLength;
	pDataIn.pbData = static_cast<BYTE *>(const_cast<unsigned char *>(keyBuffer));
	HMODULE crypt32 = LoadLibrary(_T("crypt32.dll"));
	
	if (crypt32 != NULL)
	{
		typedef BOOL (__stdcall * CryptProtectDataFunc)(DATA_BLOB*, LPCTSTR, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);
		CryptProtectDataFunc cryptProtectData;
		cryptProtectData = (CryptProtectDataFunc)GetProcAddress(crypt32, "CryptProtectData");
		if (NULL != cryptProtectData)
		{
			BOOL success = cryptProtectData(&pDataIn, NULL, NULL, NULL, NULL, NULL, &pDataOut);

			if (success && (encKeyBufferLength >= static_cast<int>(pDataOut.cbData)))
			{
				memcpy(encKeyBuffer, pDataOut.pbData, pDataOut.cbData);
				retVal = pDataOut.cbData;
			}
	
			LocalFree(pDataOut.pbData);
		}
	}
	return retVal;
}

bool Encrypter::convertToHex(const unsigned char *source, int sourceLength, TCHAR *hex, int bufferLength)
{
	if (bufferLength < ((sourceLength * 2) + 1))
	{
		return false;
	}

	for (int pos = 0; pos < sourceLength; pos++)
	{
		_stprintf_s((hex + (pos*2)), bufferLength - (pos * 2), _T("%02x"), source[pos]);
	}

	hex[sourceLength * 2] = '\0';
	return true;
}

bool Encrypter::convertFromHex(const TCHAR *hex, int hexLength, unsigned char *result, int resultLength)
{
	bool retVal = true;

	for(int pos = 0, resultPos = 0; pos < hexLength && resultPos < resultLength; pos += 2)
	{
		result[resultPos] = ((hex[pos] & 0x0F) + ((hex[pos] & 0x40) ? 9 : 0)) << 4;
		result[resultPos++] |= (hex[pos+1] & 0x0F) + ((hex[pos+1] & 0x40) ? 9 : 0);
	}
	return retVal;
}



int Encrypter::decrypt(const unsigned char *encKeyBuffer, const int encKeyLength, unsigned char *keyBuffer, const int keyLength)
{
	int retVal = 0;

	DATA_BLOB pDataOut;
	DATA_BLOB pDataIn;
	pDataIn.cbData = encKeyLength;
	pDataIn.pbData = static_cast<BYTE *>(const_cast<unsigned char *>(encKeyBuffer));

	HMODULE crypt32 = LoadLibrary(_T("crypt32.dll"));
	
	if (crypt32 != NULL)
	{
		typedef BOOL (__stdcall * CryptUnprotectDataFunc)(DATA_BLOB*, LPCTSTR, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);
		CryptUnprotectDataFunc cryptUnprotectData;
		cryptUnprotectData = (CryptUnprotectDataFunc)GetProcAddress(crypt32, "CryptUnprotectData");
		if (NULL != cryptUnprotectData)
		{	
			BOOL success = cryptUnprotectData(&pDataIn, NULL, NULL, NULL, NULL, NULL, &pDataOut);

			if (success && (keyLength >= static_cast<int>(pDataOut.cbData)))
			{
				memcpy(keyBuffer, pDataOut.pbData, pDataOut.cbData);
				retVal = pDataOut.cbData;
			}
			LocalFree(pDataOut.pbData);
		}

	}
	
	return retVal;
}


int Encrypter::decryptHex(const TCHAR *encrypted, unsigned char *decryptBuffer, const int bufferLength)
{
	int encryptedLength = _tcslen(encrypted);
	int encBufferSize = (encryptedLength / 2) + 3;
	unsigned char *encryptedBuffer = new unsigned char[encBufferSize];
	convertFromHex(encrypted, encryptedLength, encryptedBuffer, encBufferSize);
	int result = decrypt(encryptedBuffer, encryptedLength / 2, decryptBuffer, bufferLength);
	if (bufferLength > result)
	{
		decryptBuffer[result] = '\0';
	}
	else
	{
		decryptBuffer[bufferLength] = '\0';
	}

	delete[] encryptedBuffer;
	return result;
}

int Encrypter::encryptToHex(unsigned char *inBuffer, const int inLength, TCHAR *encHex, const int encHexBufferLength)
{
	int encBufferLength = (inLength * 2) + 255;
	unsigned char *encBuffer = new unsigned char[encBufferLength];
	int encLength = encrypt(inBuffer, inLength, encBuffer, encBufferLength);
	convertToHex(encBuffer, encLength, encHex, encHexBufferLength);
	delete[] encBuffer;
	return encLength;
}

/*
bool getKey(unsigned char *buffer, int bufferSize)
{
	TCHAR encKeyBufferHex[1000];
	unsigned char encKeyBuffer[500];
	unsigned char keyBuffer[16];

	int len = ::GetPrivateProfileString(SETTINGS_GROUP, KEY_KEY, _T(""), encKeyBufferHex, 1000, iniFilePath);
	if (len == 0)
	{
		generateKey(keyBuffer, 16);
		int encryptedSize = encryptKey(keyBuffer, 16, encKeyBuffer, 500);
		convertToHex(encKeyBuffer, encryptedSize, encKeyBufferHex, 1000);
		::WritePrivateProfileString(SETTINGS_GROUP, KEY_KEY, encKeyBufferHex, iniFilePath);
	}
	else
	{
		convertFromHex(encKeyBufferHex, _tcslen(encKeyBufferHex), encKeyBuffer, 500);
		decryptKey(encKeyBuffer, keyBuffer, 16);	
	}

	if (bufferSize >= 16)
	{
		memcpy(buffer, keyBuffer, 16);
		return true;
	}
	else
	{
		return false;
	}
}
*/
