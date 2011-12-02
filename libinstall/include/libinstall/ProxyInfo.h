#ifndef _PROXYINFO_H
#define _PROXYINFO_H

#include <curl/curl.h>


enum SAVECRED
{
	SAVECRED_UNKNOWN = 0,
	SAVECRED_YES = 1,
	SAVECRED_NO = 2
};


class ProxyInfo
{
public:
	void setCurlOptions(CURL *curl);
	void setProxy(const char *proxy) { _proxy = proxy; }
	void setProxyPort(int proxyPort) { _proxyPort = proxyPort;}
	void setUsername(const char *username)  { _username = username; }
	void setPassword(const char *password)  { _password = password; }
	void setSaveCredentials(const SAVECRED saveCredentials) { _saveCred = saveCredentials; }

	const char *getProxy() { return _proxy.c_str(); }
	int getProxyPort() { return _proxyPort; }
	
	const char *getUsername() const { return _username.c_str(); }
	const char *getPassword() const { return _password.c_str(); }
	const SAVECRED getSaveCredentials() const { return _saveCred; }

private:
	std::string _proxy;
	int _proxyPort;
	SAVECRED _saveCred;

	std::string _username;
	std::string _password;

};


#endif