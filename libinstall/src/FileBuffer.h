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
	int getCharAt(size_t position);
	


private:
	ifstream _file;
	boost::shared_ptr<char> _buffer;

	size_t _currentBufferStart;
	size_t _bufferLength;
	static const size_t BUFFER_SIZE      = 8192;
	static const size_t REVERSE_PREFETCH =  384;
};

#endif