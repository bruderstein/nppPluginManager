#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <boost/function.hpp>
#include "libinstall/InstallStep.h"
#include "libinstall/DeleteStep.h"

#include "libinstall/tstring.h"

using namespace std;


DeleteStep::DeleteStep(const TCHAR *file)
{
	_file = file;
}

StepStatus DeleteStep::perform(tstring& /*basePath*/, TiXmlElement* forGpup, 
							 boost::function<void(const TCHAR*)> setStatus,
							 boost::function<void(const int)> stepProgress)
{
	StepStatus status = STEPSTATUS_FAIL;

	tstring statusString = _T("Deleting ");
	statusString.append(_file);
	setStatus(statusString.c_str());


	BOOL deleteSuccess = ::DeleteFile(_file.c_str());

	if (!deleteSuccess)
	{
				status = STEPSTATUS_NEEDGPUP;
				// Add delete file to forGpup doc
				
				TiXmlElement* deleteElement = new TiXmlElement(_T("delete"));
				
				deleteElement->SetAttribute(_T("file"), _file.c_str());
				
				forGpup->LinkEndChild(deleteElement);

	}
	else
		status = STEPSTATUS_SUCCESS;

	return status;
}
