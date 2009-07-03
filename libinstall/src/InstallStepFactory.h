#ifndef _INSTALLSTEPFACTORY_H
#define _INSTALLSTEPFACTORY_H
#include <boost/shared_ptr.hpp>
#include "InstallStep.h"
#include "VariableHandler.h"
#include "tinyxml.h"



class InstallStepFactory
{
public:
	InstallStepFactory(VariableHandler* handler);

	boost::shared_ptr<InstallStep> create(TiXmlElement* element, const char* proxy, long proxyPort);

private:
	VariableHandler* _variableHandler;

};


#endif