#include "precompiled_headers.h"
#include "libinstall/Validate.h"
#include "libinstall/DownloadManager.h"
#include "libinstall/tstring.h"
#include "libinstall/md5.h"
#include "libinstall/CancelToken.h"

namespace Validator
{

ValidateStatus validate(const tstring& validateBaseUrl, const tstring& file, CancelToken& cancelToken, const ModuleInfo* moduleInfo)
{
    DownloadManager download(cancelToken);
    
    download.disableCache();

    TCHAR localMD5[(MD5::HASH_LENGTH * 2) + 1];
    MD5::hash(file.c_str(), localMD5, (MD5::HASH_LENGTH * 2) + 1);
    tstring validateUrl(validateBaseUrl);
    validateUrl.append(localMD5);
    std::string validateResult;
    if (download.getUrl(validateUrl.c_str(), validateResult, moduleInfo))
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