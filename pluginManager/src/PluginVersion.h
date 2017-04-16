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
#ifndef _PLUGINVERSION_H
#define _PLUGINVERSION_H



class PluginVersion
{
public:
	PluginVersion(void);
	explicit PluginVersion(const char *version);
	explicit PluginVersion(std::string version);

#ifdef _UNICODE
	explicit PluginVersion(const TCHAR *version);
	explicit PluginVersion(tstring version);
#endif
	
	
	PluginVersion(int major, int minor, int revision, int build);


	PluginVersion& operator= (const char *rhs);
	PluginVersion& operator= (std::string &rhs);

#ifdef _UNICODE
	PluginVersion& operator= (const TCHAR *rhs);
	PluginVersion& operator= (tstring &rhs);
#endif

	bool		operator <  (const PluginVersion &rhs);
	bool		operator<=	(const PluginVersion &rhs);
	bool		operator>	(const PluginVersion &rhs);
	bool		operator>=	(const PluginVersion &rhs);
	bool		operator==	(const PluginVersion &rhs);
	bool		operator!=  (const PluginVersion &rhs);

	TCHAR* getDisplayString();
	bool		getIsBad();
	void		setIsBad(bool isBad);
	int compare(const PluginVersion &lhs, const PluginVersion &rhs) const;

private:

	/* Private version members */

	int _major, _minor, _revision, _build;
	bool _isBad;

	
	void parseString(const char *version);

#ifdef _UNICODE
	void parseString(const TCHAR *version);
#endif

	TCHAR* _displayString;

};


bool operator <	(const PluginVersion &lhs, const PluginVersion &rhs);


#endif