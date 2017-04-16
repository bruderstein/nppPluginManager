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
#include "PluginVersion.h"

using namespace std;

PluginVersion::PluginVersion(void)
: _major(0), _minor(0), _revision(0), _build(0), _displayString(NULL), _isBad(false)
{
}

PluginVersion::PluginVersion(const char *version)
: _major(0), _minor(0), _revision(0), _build(0), _displayString(NULL), _isBad(false)
{
	
	size_t versionSize = strlen(version) + 1;
	char *str = new char[versionSize];
	strcpy_s(str, versionSize, version);
	parseString(str);
	delete[] str;
}


PluginVersion::PluginVersion(string version)
: _major(0), _minor(0), _revision(0), _build(0), _displayString(NULL), _isBad(false)
{
	char *str = new char[version.size() + 1];
	strcpy_s(str, version.size() + 1, version.c_str());
	parseString(str);
	delete[] str;
	
}

#ifdef _UNICODE

PluginVersion::PluginVersion(const TCHAR *version)
: _major(0), _minor(0), _revision(0), _build(0), _displayString(NULL), _isBad(false)
{
	size_t versionSize = _tcslen(version) + 1;
	TCHAR *str = new TCHAR[versionSize];
	_tcscpy_s(str, versionSize, version);
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(tstring version)
: _major(0), _minor(0), _revision(0), _build(0), _displayString(NULL), _isBad(false)
{
	TCHAR *str = new TCHAR[version.size() + 1];
	_tcscpy_s(str, version.size() + 1, version.c_str());
	parseString(str);
	delete[] str;
}

#endif


PluginVersion::PluginVersion(int major, int minor, int revision, int build)
: _major(major), _minor(minor), _revision(revision), _build(build), _displayString(NULL), _isBad(false)
{
}




bool PluginVersion::operator< (const PluginVersion &rhs)
{
	return (compare(*this, rhs) < 0);
}

bool PluginVersion::operator<= (const PluginVersion &rhs)
{
	return (compare(*this, rhs) <= 0);
}

bool PluginVersion::operator> (const PluginVersion &rhs)
{
	return (compare(*this, rhs) > 0);
}

bool PluginVersion::operator>= (const PluginVersion &rhs)
{
	return (compare(*this, rhs) >= 0);
}

bool PluginVersion::operator== (const PluginVersion &rhs)
{
	return (compare(*this, rhs) == 0);
}

bool PluginVersion::operator!= (const PluginVersion &rhs)
{
	return (compare(*this, rhs) != 0);
}


PluginVersion &PluginVersion::operator=(const char *rhs)
{
	_displayString = NULL;
	size_t versionSize = strlen(rhs) + 1;
	char *str = new char[versionSize];
	strcpy_s(str, versionSize, rhs);
	parseString(str);
	delete[] str;
	return *this;
}

#ifdef _UNICODE
PluginVersion &PluginVersion::operator=(const TCHAR *rhs)
{
	_displayString = NULL;
	size_t versionSize = _tcslen(rhs) + 1;
	TCHAR *str = new TCHAR[versionSize];
	_tcscpy_s(str, versionSize, rhs);
	parseString(str);
	delete[] str;
	return *this;
}



PluginVersion &PluginVersion::operator=(tstring &rhs)
{
	_displayString = NULL;
	TCHAR *str = new TCHAR[rhs.size() + 1];
	_tcscpy_s(str, rhs.size() + 1, rhs.c_str());
	parseString(str);
	delete[] str;
	return *this;
}
#endif

PluginVersion &PluginVersion::operator=(string &version)
{
	_displayString = NULL;
	char *str = new char[version.size() + 1];
	strcpy_s(str, version.size() + 1, version.c_str());
	parseString(str);
	delete[] str;
	return *this;
}



void PluginVersion::parseString(const char *version)
{

	if (NULL != _displayString)
	{
		delete[] _displayString;
		_displayString = NULL;
	}

	size_t versionSize = strlen(version) + 1;
	char* versionCopy = new char[versionSize];
	strcpy_s(versionCopy, versionSize, version);
	
	char* context;
	char* versionPart = strtok_s(versionCopy, ",.", &context);
	
	_major = _minor = _revision = _build = 0;

	if (versionPart)
	{
		StrTrimA(versionPart, " ");
		_major = atoi(versionPart);

		versionPart = strtok_s(NULL, ",.", &context);
		if (versionPart)
		{
			StrTrimA(versionPart, " ");
			_minor = atoi(versionPart);

			versionPart = strtok_s(NULL, ",.", &context);
			if (versionPart)
			{
				StrTrimA(versionPart, " ");
				_revision = atoi(versionPart);

				versionPart = strtok_s(NULL, ",.", &context);
				if (versionPart)
				{
					StrTrimA(versionPart, " ");
					_build = atoi(versionPart);
				}
			}
		}
	}
	delete[] versionCopy;
}


#ifdef _UNICODE

void PluginVersion::parseString(const TCHAR *version)
{
	if (NULL != _displayString)
	{
		delete[] _displayString;
		_displayString = NULL;
	}


	size_t versionSize = _tcslen(version) + 1;
	TCHAR* versionCopy = new TCHAR[versionSize];
	_tcscpy_s(versionCopy, versionSize, version);
	
	TCHAR* context;
	TCHAR* versionPart = _tcstok_s(versionCopy, _T(",."), &context);
	
	_major = _minor = _revision = _build = 0;

	if (versionPart)
	{
		StrTrim(versionPart, _T(" "));
		_major = _ttoi(versionPart);

		versionPart = _tcstok_s(NULL, _T(",."), &context);
		if (versionPart)
		{
			StrTrim(versionPart, _T(" "));
			_minor = _ttoi(versionPart);

			versionPart = _tcstok_s(NULL, _T(",."), &context);
			if (versionPart)
			{
				StrTrim(versionPart, _T(" "));
				_revision = _ttoi(versionPart);

				versionPart = _tcstok_s(NULL, _T(",."), &context);
				if (versionPart)
				{
					StrTrim(versionPart, _T(" "));
					_build = _ttoi(versionPart);
				}
			}
		}
	}
	delete[] versionCopy;
}

#endif


int PluginVersion::compare(const PluginVersion &lhs, const PluginVersion &rhs) const
{


	int difference = lhs._major - rhs._major;
	if (difference != 0)
		return difference;

	difference = lhs._minor - rhs._minor;
	if (difference != 0)
		return difference;

	difference = lhs._revision - rhs._revision;
	if (difference != 0)
		return difference;


	difference = lhs._build - rhs._build;
	return difference;


}

TCHAR* PluginVersion::getDisplayString()
{
	
	if (NULL == _displayString)
	{
		basic_stringstream<TCHAR> display;
		if (_major == 0 && _minor == 0 && _revision == 0 && _build == 0)
			display << _T("Unknown");
		else 
		{
			display << _major << _T(".") << _minor;
			if (_revision + _build > 0)
				display << _T(".") << _revision;

			if (_build > 0)
				display  << _T(".") << _build;

			if (_isBad)
				display << _T(" (unstable)");
		}

		unsigned int length = static_cast<unsigned int>(display.tellp());
		length++;
		_displayString = new TCHAR[length];
		_tcscpy_s(_displayString, static_cast<rsize_t>(length), display.str().c_str());
	}

	return _displayString;
}


bool PluginVersion::getIsBad()
{
	return _isBad;
}

void PluginVersion::setIsBad(bool isBad)
{
	_isBad = isBad;
}



bool operator <	(const PluginVersion &lhs, const PluginVersion &rhs)
{
	return (lhs.compare(lhs, rhs) < 0);
}
