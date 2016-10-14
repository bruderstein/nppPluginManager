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

#ifndef _COPYSTEP_H
#define _COPYSTEP_H
#include "InstallStep.h"
#include "Validate.h"

class VariableHandler;
class CancelToken;

enum ToDestination
{
    TO_DIRECTORY,
    TO_FILE
};



class CopyStep : public InstallStep
{
public:
    CopyStep(const TCHAR* from, const TCHAR* to, const TCHAR* toFile, BOOL attemptReplace, BOOL validate, 
        BOOL isGpup,
        BOOL backup, 
        BOOL recursive,
		const tstring& validateBaseUrl);

    ~CopyStep() {};
    
    StepStatus perform(tstring& basePath, TiXmlElement* forGpup,
         std::function<void(const TCHAR*)> setStatus,
         std::function<void(const int)> stepProgress, 
         const ModuleInfo* moduleInfo, 
         CancelToken& cancelToken);

    void replaceVariables(VariableHandler *variableHandler);

private:
    
    ValidateStatus Validate(tstring& file);
    void copyGpup(const tstring& basePath, const tstring& toPath);
    void callGpup(const TCHAR *gpupPath, const TCHAR *arguments);

    StepStatus copyDirectory(tstring& fromPath, tstring& toPath, 
                     TiXmlElement* forGpup,
                     std::function<void(const TCHAR*)> setStatus,
                     std::function<void(const int)> stepProgress, 
                     const ModuleInfo* moduleInfo,
                     CancelToken& cancelToken);

    
    tstring	_from;
    tstring _to;
    tstring _toFile;
    tstring _validateBaseUrl;


    ToDestination _toDestination;

    
    BOOL    _failIfExists;
    BOOL    _validate;
    BOOL	_backup;
    BOOL    _isGpup;
    BOOL	_recursive;
};

#endif
