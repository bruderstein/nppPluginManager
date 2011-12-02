#ifndef _ENCRYPTER_H
#define _ENCRYPTER_H

class Encrypter 
{
public:
	
	/** Returns the length of the key encrypted data */
	int encrypt(unsigned const char *inBuffer, const int inLength, unsigned char *outBuffer, const int outBufferLength);

	int decrypt(const unsigned char *encBuffer, const int encLength, unsigned char *decBuffer, const int decBufferLength);
	
	bool convertToHex(const unsigned char *source, int sourceLength, TCHAR *hex, int bufferLength);
	bool convertFromHex(const TCHAR *hex, int hexLength, unsigned char *result, int resultLength);

	int decryptHex(const TCHAR *encrypted, unsigned char *decryptBuffer, const int bufferLength);
	int encryptToHex(unsigned char *inBuffer, const int inLength, TCHAR *encHex, const int encHexBufferLength);
private:


};

#endif