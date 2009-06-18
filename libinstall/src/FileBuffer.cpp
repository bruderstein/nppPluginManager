#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "FileBuffer.h"

using namespace std;
using namespace boost;

FileBuffer::FileBuffer()
{
	_currentBufferStart = 0;
	_bufferLength = 0;
}

FileBuffer::FileBuffer(const char *filename)
{
	open(filename);	
}

FileBuffer::~FileBuffer()
{
	_file.close();
}


void FileBuffer::open(const char *filename)
{
	_file.open(filename);
	_buffer.reset(new char[BUFFER_SIZE]);
	_file.read(_buffer.get(), BUFFER_SIZE);
	_currentBufferStart = 0;
	_bufferLength = _file.gcount();
}

int FileBuffer::getCharAt(long position)
{
	long positionInBuffer = position - _currentBufferStart;
	if (positionInBuffer > 0 && positionInBuffer < _bufferLength)
		return _buffer.get()[positionInBuffer];
	else
	{
		_currentBufferStart = position - REVERSE_PREFETCH;
		if (_currentBufferStart < 0) 
			_currentBufferStart = 0;
		
		_file.seekg(_currentBufferStart);
		_file.read(_buffer.get(), BUFFER_SIZE);
		_bufferLength = _file.gcount();
		positionInBuffer = position - _currentBufferStart;
		if (positionInBuffer < _bufferLength && positionInBuffer > 0)
			return _buffer.get()[positionInBuffer];
		else
			return FILEBUFFER_EOF;
	}
	
}
