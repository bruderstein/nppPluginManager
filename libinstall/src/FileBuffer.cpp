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
#include "libinstall/FileBuffer.h"

using namespace std;

FileBuffer::FileBuffer()
{
	_currentBufferStart = 0;
	_bufferLength = 0;
}

FileBuffer::FileBuffer(const TCHAR *filename)
{
	open(filename);	
}

FileBuffer::~FileBuffer()
{
	_file.close();
}


void FileBuffer::open(const TCHAR *filename)
{
	_file.open(filename);
	_buffer.reset(new char[BUFFER_SIZE]);
	_file.read(_buffer.get(), BUFFER_SIZE);
	_currentBufferStart = 0;
	_bufferLength = static_cast<size_t>(_file.gcount());
}

char FileBuffer::getCharAt(size_t position)
{
	size_t positionInBuffer = position - _currentBufferStart;
	if (positionInBuffer != 0 && positionInBuffer < _bufferLength)
		return _buffer.get()[positionInBuffer];
	else
	{
		_currentBufferStart = position - REVERSE_PREFETCH;
		if (_currentBufferStart < 0) 
			_currentBufferStart = 0;
		
		_file.seekg((std::streamoff)_currentBufferStart);
		_file.read(_buffer.get(), BUFFER_SIZE);
		_bufferLength = static_cast<size_t>(_file.gcount());
		positionInBuffer = position - _currentBufferStart;
		if (positionInBuffer < _bufferLength && positionInBuffer > 0)
			return _buffer.get()[positionInBuffer];
		else
			return FILEBUFFER_EOF;
	}
	
}
