#include "precompiled_headers.h"
#include "libinstall/Validate.h"
#include "libinstall/DownloadManager.h"
#include "libinstall/tstring.h"
#include "libinstall/md5.h"

namespace Validator
{

ValidateStatus validate(const tstring& file, ProxyInfo* proxyInfo)
{
	DownloadManager download;
	
	TCHAR localMD5[(MD5::HASH_LENGTH * 2) + 1];
	MD5::hash(file.c_str(), localMD5, (MD5::HASH_LENGTH * 2) + 1);
	tstring validateUrl = VALIDATE_BASEURL;
	validateUrl.append(localMD5);
	std::string validateResult;
	if (download.getUrl(validateUrl.c_str(), validateResult, proxyInfo))
	{
		if (validateResult == VALIDATE_RESULT_OK)
			return VALIDATE_OK;

		else if (validateResult == VALIDATE_RESULT_UNKNOWN)
			return VALIDATE_UNKNOWN;

		else if (validateResult == VALIDATE_RESULT_BANNED)
			return VALIDATE_BANNED;
		else 
			return VALIDATE_UNKNOWN;
	}
	else
		return VALIDATE_UNKNOWN;
}

}