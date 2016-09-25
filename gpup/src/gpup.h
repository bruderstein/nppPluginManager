#pragma once
/*
This file is part of GPUP, which is part of Plugin Manager 
Plugin for Notepad++

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

#include "resource.h"


#ifdef ALLOW_OVERRIDE_XML_URL
#define VALIDATE_BASE_URL          _T("http://www.brotherstone.co.uk/npp/pm/admin/validate.php?md5=")
#define VALIDATE_BASE_HTTP_URL          _T("http://www.brotherstone.co.uk/npp/pm/admin/validate.php?md5=")
#else
#define VALIDATE_BASE_URL          _T("https://nppxml.bruderste.in/pm/validate?md5=")
#define VALIDATE_BASE_HTTP_URL     _T("http://nppxml.bruderste.in/pm/validate?md5=")
#endif

#define DEV_VALIDATE_BASE_URL          _T("https://nppxmldev.bruderste.in/pm/validate?md5=")