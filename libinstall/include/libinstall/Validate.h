#ifndef _VALIDATE_H

#define _VALIDATE_H
#define VALIDATE_RESULT_OK        "ok"
#define VALIDATE_RESULT_UNKNOWN   "unknown"
#define VALIDATE_RESULT_BANNED    "banned"

enum ValidateStatus
{
	VALIDATE_OK,
	VALIDATE_UNKNOWN,
	VALIDATE_BANNED
};

class ModuleInfo;
class CancelToken;

namespace Validator {
	ValidateStatus validate(const tstring& validateBaseUrl, const tstring& file, CancelToken& cancelToken, const ModuleInfo *moduleInfo);
}

#endif