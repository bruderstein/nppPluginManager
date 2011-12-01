#ifndef _MODULEINFO_H
#define _MODULEINFO_H

class ModuleInfo
{
public:
	ModuleInfo() {}
	ModuleInfo(HMODULE hModule, HWND hParent)
		: _hModule(hModule), _hParent(hParent) {}

	const HMODULE getHModule() const { return _hModule; }
	const HWND    getHParent() const { return _hParent; }

	void setHModule(HMODULE hModule) { _hModule = hModule; }
	void setHParent(HWND hParent)    { _hParent = hParent; }

private:
	HMODULE _hModule;
	HWND    _hParent;
};


#endif