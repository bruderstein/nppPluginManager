#ifndef _VALIDATE_H

#define _VALIDATE_H
#ifdef ALLOW_OVERRIDE_XML_URL
#define VALIDATE_BASEURL          _T("http://www.brotherstone.co.uk/npp/pm/admin/validate.php?md5=")
#else
#define VALIDATE_BASEURL          _T("http://npppluginmgr.sourceforge.net/app/validate.php?md5=")
#endif
#define VALIDATE_RESULT_OK        "ok"
#define VALIDATE_RESULT_UNKNOWN   "unknown"
#define VALIDATE_RESULT_BANNED    "banned"

enum ValidateStatus
{
	VALIDATE_OK,
	VALIDATE_UNKNOWN,
	VALIDATE_BANNED
};

class ProxyInfo;
class ModuleInfo;

namespace Validator {
	ValidateStatus validate(const tstring& file, const ModuleInfo *moduleInfo);
}

#endif