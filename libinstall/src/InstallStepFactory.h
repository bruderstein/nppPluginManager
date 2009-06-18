#ifndef _INSTALLSTEPFACTORY_H
#define _INSTALLSTEPFACTORY_H
#include <boost/shared_ptr.hpp>
#include "InstallStep.h"
#include "tinyxml.h"

class InstallStepFactory
{
public:
	static boost::shared_ptr<InstallStep> create(TiXmlElement* element);

};


#endif