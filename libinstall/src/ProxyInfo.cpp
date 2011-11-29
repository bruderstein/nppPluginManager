#include "precompiled_headers.h"
#include "libinstall/ProxyInfo.h"
#include <curl/curl.h>

void ProxyInfo::setCurlOptions(CURL *curl)
{
	if (!_proxy.empty())
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, _proxy.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYPORT, _proxyPort);
	}

	if (!_username.empty())
	{
		curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, _username.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, _password.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
	}

}
