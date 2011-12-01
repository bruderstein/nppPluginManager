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
#ifndef _DOWNLOADSTEP_H
#define _DOWNLOADSTEP_H
#include "InstallStep.h"

class ProxyInfo;
class ModuleInfo;

class DownloadStep : public InstallStep
{
public:
	DownloadStep(const TCHAR* url, const TCHAR* filename, ProxyInfo *proxyInfo);
	~DownloadStep() {};
	
	StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
		boost::function<void(const TCHAR*)> setStatus,
    	boost::function<void(const int)> stepProgress, const ModuleInfo *moduleInfo);

private:
	tstring	_url;
	tstring _filename;
	ProxyInfo *_proxyInfo;

};

#endif
