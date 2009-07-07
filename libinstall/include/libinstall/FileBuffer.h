#ifndef _FILEBUFFER_H
#define _FILEBUFFER_H

#include <string>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <boost/shared_ptr.hpp>
#include <tchar.h>

#define FILEBUFFER_EOF	  CHAR_MAX
using namespace std;

class FileBuffer
{
public:
	FileBuffer();
	FileBuffer(const TCHAR *filename);
	~FileBuffer();

	void open(const TCHAR *filename);
	char getCharAt(size_t position);

private:
	ifstream _file;
	boost::shared_ptr<char> _buffer;

	size_t _currentBufferStart;
	size_t _bufferLength;
	static const size_t BUFFER_SIZE      = 8192;
	static const size_t REVERSE_PREFETCH =  384;
};

#endif