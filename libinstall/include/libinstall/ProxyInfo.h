#ifndef _PROXYINFO_H
#define _PROXYINFO_H

#include <curl/curl.h>


class ProxyInfo
{
public:
	void setCurlOptions(CURL *curl);
	void setProxy(const char *proxy) { _proxy = proxy; }
	void setProxyPort(int proxyPort) { _proxyPort = proxyPort;}
	void setUsername(const char *username)  { _username = username; }
	void setPassword(const char *password)  { _password = password; }
	
	const char *getProxy() { return _proxy.c_str(); }
	int getProxyPort() { return _proxyPort; }
	
	const char *getUsername() { return _username.c_str(); }
	const char *getPassword() { return _password.c_str(); }

private:
	std::string _proxy;
	int _proxyPort;

	std::string _username;
	std::string _password;

};


#endif