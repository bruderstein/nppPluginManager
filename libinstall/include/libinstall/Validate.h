#ifndef _VALIDATE_H

#define _VALIDATE_H

#define VALIDATE_BASEURL          _T("http://notepad-plus.sourceforge.net/commun/pluginManager/validate.php?md5=")
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
	ValidateStatus validate(const tstring& file, ProxyInfo *proxyInfo, const ModuleInfo *moduleInfo);
}

#endif