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
#ifndef _FILEBUFFER_H
#define _FILEBUFFER_H

#define FILEBUFFER_EOF	  CHAR_MAX

class FileBuffer
{
public:
	FileBuffer();
	FileBuffer(const TCHAR *filename);
	~FileBuffer();

	void open(const TCHAR *filename);
	char getCharAt(size_t position);

private:
	std::ifstream _file;
	std::shared_ptr<char> _buffer;

	size_t _currentBufferStart;
	size_t _bufferLength;
	static const size_t BUFFER_SIZE      = 8192;
	static const size_t REVERSE_PREFETCH =  384;
};

#endif