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

Modified for inclusion in VS2010 project "Python Script"
*/
#ifndef _WCHARMBCSCONVERTER_H
#define _WCHARMBCSCONVERTER_H



class WcharMbcsConverter {
public:
	
	static std::shared_ptr<wchar_t> char2wchar(const char* mbStr);
	static std::shared_ptr<char>    wchar2char(const wchar_t* wcStr);

	static std::shared_ptr<TCHAR>   char2tchar(const char* mbStr);
	static std::shared_ptr<char>    tchar2char(const TCHAR* tStr);


};

#endif
