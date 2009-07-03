#include <tchar.h>
#include <string.h>
#include <string>
#include <shlwapi.h>
#include <sstream>
#include "PluginVersion.h"

using namespace std;

PluginVersion::PluginVersion(void)
{
	_major = _minor = _revision = _build = 0;
	_displayString = NULL;
}

PluginVersion::PluginVersion(const char *version)
{
	_displayString = NULL;
	size_t versionSize = strlen(version) + 1;
	char *str = new char[versionSize];
	strcpy_s(str, versionSize, version);
	parseString(str);
	delete[] str;
}


PluginVersion::PluginVersion(string version)
{
	_displayString = NULL;
	char *str = new char[version.size() + 1];
	strcpy_s(str, version.size() + 1, version.c_str());
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(tstring version)
{
	_displayString = NULL;
	TCHAR *str = new TCHAR[version.size() + 1];
	_tcscpy_s(str, version.size() + 1, version.c_str());
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(const TCHAR *version)
{
	_displayString = NULL;
	size_t versionSize = _tcslen(version) + 1;
	TCHAR *str = new TCHAR[versionSize];
	_tcscpy_s(str, versionSize, version);
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(int major, int minor, int revision, int build)
{
	_major		= major;
	_minor		= minor;
	_revision	= revision;
	_build		= build;
	_displayString = NULL;
}


bool PluginVersion::operator< (PluginVersion &rhs)
{
	return (compare(*this, rhs) < 0);
}

bool PluginVersion::operator<= (PluginVersion &rhs)
{
	return (compare(*this, rhs) <= 0);
}

bool PluginVersion::operator> (PluginVersion &rhs)
{
	return (compare(*this, rhs) > 0);
}

bool PluginVersion::operator>= (PluginVersion &rhs)
{
	return (compare(*this, rhs) >= 0);
}

bool PluginVersion::operator== (PluginVersion &rhs)
{
	return (compare(*this, rhs) == 0);
}

bool PluginVersion::operator!= (PluginVersion &rhs)
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


int PluginVersion::compare(PluginVersion &lhs, PluginVersion &rhs)
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
		
		display << _major << _T(".") << _minor;
		if (_revision + _build > 0)
			display << _T(".") << _revision;

		if (_build > 0)
			display  << _T(".") << _build;
		
		size_t length = display.tellp();
		length++;
		_displayString = new TCHAR[length];
		_tcscpy_s(_displayString, length, display.str().c_str());
	}

	return _displayString;
}
