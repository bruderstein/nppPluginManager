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
#pragma once

#include "CancelToken.h"

class ModuleInfo;


class DownloadManager
{
public:
    DownloadManager(CancelToken& cancelToken);
    ~DownloadManager();
    BOOL getUrl(const TCHAR *url, tstring& filename, tstring& contentType, const ModuleInfo *moduleInfo);
    BOOL getUrl(const TCHAR *url, std::string& result, const ModuleInfo *moduleInfo);
    void cancelDownload();
    void disableCache();

    void setProgressFunction(std::function<void(int)> progressFunction);

    static void setUserAgent(const TCHAR* userAgent);


private:
    std::function<void(int)> _progressFunction;
    BOOL					   _progressFunctionSet;
    static tstring				_userAgent;

    CancelToken                m_cancelToken;
    BOOL                       m_disableCache;
};
