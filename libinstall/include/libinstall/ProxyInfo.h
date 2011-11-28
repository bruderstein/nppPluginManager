#ifndef _PROXYINFO_H
#define _PROXYINFO_H

#include <curl/curl.h>
#include "tstring.h"


class ProxyInfo
{
public:
	void setCurlOptions(CURL *curl);
	void setProxy(const char *proxy) { _proxy = proxy; };
	void setProxyPort(int proxyPort) { _proxyPort = proxyPort;};
	void setUsername(const char *username)  { _username = username; };
	void setPassword(const char *password)  { _password = password; };
	

private:
	std::string _proxy;
	int _proxyPort;

	std::string _username;
	std::string _password;

};


#endif