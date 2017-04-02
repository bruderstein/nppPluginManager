// EncryptionTest.cpp : Defines the entry point for the console application.
//

#include "precompiled_headers.h"
#include "../PluginManager/src/Encrypter.h"

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	Encrypter encrypter;
	unsigned char keyBuffer[20];
	TCHAR hexKeyBuffer[500];

	strcpy((char *)keyBuffer, "testPassword");

	encrypter.encryptToHex(keyBuffer, strlen((char *)keyBuffer), hexKeyBuffer, 500);
	_tprintf(_T("Generated hex encryption: %s\r\n"), hexKeyBuffer);
	
	unsigned char decBuffer[50];
	encrypter.decryptHex(hexKeyBuffer, decBuffer, 50);
    printf("Decrypted: %s\r\n", decBuffer);
	
	getchar();
	return 0;
}

