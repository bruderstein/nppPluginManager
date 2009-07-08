#include <tchar.h>

#define BUFSIZE 4096
#define MD5LEN    16


class MD5 
{
public:
	static BOOL hash(const TCHAR *filename, TCHAR *hashBuffer, int hashBufferLength);

	static const int HASH_LENGTH = 16;
};