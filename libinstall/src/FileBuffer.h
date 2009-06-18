#ifndef _FILEBUFFER_H
#define _FILEBUFFER_H

#include <string>
#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>


#define FILEBUFFER_EOF	  -1
using namespace std;

class FileBuffer
{
public:
	FileBuffer();
	FileBuffer(const char *filename);
	~FileBuffer();

	void open(const char *filename);
	int getCharAt(long position);
	


private:
	ifstream _file;
	boost::shared_ptr<char> _buffer;

	long _currentBufferStart;
	long _bufferLength;
	static const long BUFFER_SIZE      = 8192;
	static const long REVERSE_PREFETCH =  384;
};

#endif