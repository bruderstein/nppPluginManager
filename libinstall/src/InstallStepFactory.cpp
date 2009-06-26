#include "InstallStepFactory.h"
#include "DownloadStep.h"
#include "CopyStep.h"
#include "tstring.h"
#include "VariableHandler.h"

using namespace boost;
using namespace std;

InstallStepFactory::InstallStepFactory(VariableHandler* variableHandler)
{
	_variableHandler = variableHandler;
}



shared_ptr<InstallStep> InstallStepFactory::create(TiXmlElement* element)
{
	shared_ptr<InstallStep> installStep;

	if (!_tcscmp(element->Value(), _T("download")) && element->FirstChild())
	{
		installStep.reset(new DownloadStep(element->FirstChild()->Value(), element->Attribute(_T("filename"))));
	}
	else if (!_tcscmp(element->Value(), _T("copy")))
	{
		const TCHAR *tFrom = element->Attribute(_T("from"));
		const TCHAR *tTo = element->Attribute(_T("to"));
		const TCHAR *tReplace = element->Attribute(_T("replace"));
		BOOL attemptReplace = FALSE;

		if (tReplace && _tcscmp(tReplace, _T("true")))
			attemptReplace = TRUE;

		tstring from;
		tstring to;
		if (tFrom)
			from = tFrom;
		if (tTo)
			to = tTo;


		if (_variableHandler)
		{
			_variableHandler->replaceVariables(from);
			_variableHandler->replaceVariables(to);
		}

		installStep.reset(new CopyStep(from.c_str(), to.c_str(), attemptReplace));
	}

	return installStep;

}



