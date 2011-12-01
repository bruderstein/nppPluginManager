#ifndef _PROXYCREDENTIALSDLG_H
#define _PROXYCREDENTIALSDLG_H


class ProxyInfo;
class ModuleInfo;

class ProxyCredentialsDlg
{

public:
	ProxyCredentialsDlg() {};
	~ProxyCredentialsDlg() {};    
    
	BOOL getCredentials(const ModuleInfo* moduleInfo, ProxyInfo* proxyInfo);
   	

protected :
	

private:
	
	static BOOL CALLBACK dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void initialiseCredentials();
	void setCredentials();
	


	ProxyInfo* _proxyInfo;
	HWND   _hSelf;
};



#endif // _PROXYCREDENTIALSDLG_H