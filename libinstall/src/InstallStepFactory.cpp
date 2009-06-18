#include "InstallStepFactory.h"
#include "DownloadStep.h"

using namespace boost;


shared_ptr<InstallStep> InstallStepFactory::create(TiXmlElement* element)
{
	shared_ptr<InstallStep> installStep;

	if (!_tcscmp(element->Value(), _T("download")) && element->FirstChild())
	{
		installStep.reset(new DownloadStep(element->FirstChild()->Value(), element->Attribute(_T("filename"))));
	}

	return installStep;

}

