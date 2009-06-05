#include <tchar.h>
#include <string.h>
#include <string>
#include <shlwapi.h>

#include "PluginVersion.h"

using namespace std;

PluginVersion::PluginVersion(void)
{
	_displayString = NULL;
	_major = _minor = _revision = _build = 0;
}

PluginVersion::PluginVersion(const char *version)
{
	_displayString = NULL;
	char *str = new char[strlen(version) + 1];
	strcpy(str, version);
	parseString(str);
	delete[] str;
}


PluginVersion::PluginVersion(string version)
{
	_displayString = NULL;
	char *str = new char[version.size() + 1];
	strcpy(str, version.c_str());
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(tstring version)
{
	_displayString = NULL;
	TCHAR *str = new TCHAR[version.size() + 1];
	_tcscpy(str, version.c_str());
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(const TCHAR *version)
{
	_displayString = NULL;
	TCHAR *str = new TCHAR[_tcslen(version) + 1];
	_tcscpy(str, version);
	parseString(str);
	delete[] str;
	
}

PluginVersion::PluginVersion(int major, int minor, int revision, int build)
{
	_displayString = NULL;
	_major		= major;
	_minor		= minor;
	_revision	= revision;
	_build		= build;
	
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
	char *str = new char[strlen(rhs) + 1];
	strcpy(str, rhs);
	parseString(str);
	delete[] str;
	return *this;
}

PluginVersion &PluginVersion::operator=(const TCHAR *rhs)
{
	TCHAR *str = new TCHAR[_tcslen(rhs) + 1];
	_tcscpy(str, rhs);
	parseString(str);
	delete[] str;
	return *this;
}

PluginVersion &PluginVersion::operator=(tstring &rhs)
{
	TCHAR *str = new TCHAR[rhs.size() + 1];
	_tcscpy(str, rhs.c_str());
	parseString(str);
	delete[] str;
	return *this;
}

PluginVersion &PluginVersion::operator=(string &version)
{
	char *str = new char[version.size() + 1];
	strcpy(str, version.c_str());
	parseString(str);
	delete[] str;
	return *this;
}



void PluginVersion::parseString(const char *version)
{

	if (NULL != _displayString)
	{
		delete[] _displayString;
	}

	char* versionCopy = new char[strlen(version) + 1];
	strcpy(versionCopy, version);
	char* versionPart = strtok(versionCopy, ",.");
	
	_major = _minor = _revision = _build = 0;

	if (versionPart)
	{
		StrTrimA(versionPart, " ");
		_major = atoi(versionPart);

		versionPart = strtok(NULL, ",.");
		if (versionPart)
		{
			StrTrimA(versionPart, " ");
			_minor = atoi(versionPart);

			versionPart = strtok(NULL, ",.");
			if (versionPart)
			{
				StrTrimA(versionPart, " ");
				_revision = atoi(versionPart);

				versionPart = strtok(NULL, ",.");
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
	}

	TCHAR* versionCopy = new TCHAR[_tcslen(version) + 1];
	_tcscpy(versionCopy, version);
	TCHAR* versionPart = _tcstok(versionCopy, _T(",."));
	
	_major = _minor = _revision = _build = 0;

	if (versionPart)
	{
		StrTrim(versionPart, _T(" "));
		_major = _ttoi(versionPart);

		versionPart = _tcstok(NULL, _T(",."));
		if (versionPart)
		{
			StrTrim(versionPart, _T(" "));
			_minor = _ttoi(versionPart);

			versionPart = _tcstok(NULL, _T(",."));
			if (versionPart)
			{
				StrTrim(versionPart, _T(" "));
				_revision = _ttoi(versionPart);

				versionPart = _tcstok(NULL, _T(",."));
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
		_displayString = new TCHAR[30];
		_sntprintf(_displayString, 30, _T("%d.%d.%d.%d"), _major, _minor, _revision, _build);
	}
	return _displayString;
}
