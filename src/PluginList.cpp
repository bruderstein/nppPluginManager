#include "PluginList.h"
#include "PluginManager.h"
#include "tinyxml.h"
#include <strsafe.h>
#include <windows.h>

using namespace std;

typedef BOOL (__cdecl * PFUNCISUNICODE)();


PluginList::PluginList(void)
{
}

PluginList::~PluginList(void)
{
}



BOOL PluginList::parsePluginFile(TCHAR *filename)
{
	int len = _tcslen(filename) + 2;
	char *cFilename = new char[len];
	wcstombs(cFilename, filename, len);
	TiXmlDocument doc(cFilename);
	doc.LoadFile();
	TiXmlNode *pluginsDoc = doc.FirstChildElement("plugins");
	if (pluginsDoc)
	{
		TiXmlElement *pluginNode = pluginsDoc->FirstChildElement("plugin");
		Plugin *plugin;

		while(pluginNode)
		{
			plugin = new Plugin();

			plugin->setName(pluginNode->Attribute("name"));
			 
			TiXmlElement *unicodeUrlElement = pluginNode->FirstChildElement("unicodeURL");
			if (unicodeUrlElement && unicodeUrlElement->FirstChild())
				plugin->setUnicodeUrl(unicodeUrlElement->FirstChild()->Value());

			TiXmlElement *ansiUrlElement = pluginNode->FirstChildElement("ansiURL");
			if (ansiUrlElement && ansiUrlElement->FirstChild())
				plugin->setAnsiUrl(ansiUrlElement->FirstChild()->Value());

			if (g_isUnicode)
			{
				TiXmlElement *versionUrlElement = pluginNode->FirstChildElement("unicodeVersion");
				if (versionUrlElement && versionUrlElement->FirstChild())
					plugin->setVersion(PluginVersion(versionUrlElement->FirstChild()->Value()));
			}
			else 
			{
				TiXmlElement *versionUrlElement = pluginNode->FirstChildElement("ansiVersion");
				if (versionUrlElement && versionUrlElement->FirstChild())
					plugin->setVersion(PluginVersion(versionUrlElement->FirstChild()->Value()));

			}

			TiXmlElement *descriptionUrlElement = pluginNode->FirstChildElement("description");
			if (descriptionUrlElement && descriptionUrlElement->FirstChild())
				plugin->setDescription(descriptionUrlElement->FirstChild()->Value());

			TiXmlElement *filenameUrlElement = pluginNode->FirstChildElement("filename");
			if (filenameUrlElement && filenameUrlElement->FirstChild())
				plugin->setFilename(filenameUrlElement->FirstChild()->Value());
			
			_plugins[plugin->getName()] = plugin;
			
			pluginNode = (TiXmlElement *)pluginsDoc->IterateChildren(pluginNode);
		}
	}
	return TRUE;
}

BOOL PluginList::checkInstalledPlugins(TCHAR *pluginPath)
{
	tstring pluginsFullPathFilter = pluginPath;
	pluginsFullPathFilter += _T("\\plugins\\*.dll");

	WIN32_FIND_DATA foundData;
	HANDLE hFindFile = ::FindFirstFile(pluginsFullPathFilter.c_str(), &foundData);


	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			
			tstring pluginFilename(pluginPath);
			pluginFilename += _T("\\plugins\\");
			pluginFilename += foundData.cFileName;
			BOOL pluginOK = false;
			tstring pluginName;
			try 
			{
				pluginName = getPluginName(pluginFilename);
				pluginOK = true;
			} 
			catch (...)
			{ 
				pluginOK = false;
			}

			
			
			if (pluginOK)
			{
			PluginContainer::iterator knownPlugin = _plugins.find(pluginName);

				if (knownPlugin != _plugins.end())
				{
		
					Plugin* plugin = knownPlugin->second;
					plugin->setFilename(pluginFilename);

					setInstalledVersion(pluginFilename, plugin);
					if (plugin->getVersion() == plugin->getInstalledVersion())
						_installedPlugins.push_back(plugin);
					else if (plugin->getVersion() > plugin->getInstalledVersion())
						_updateablePlugins.push_back(plugin);
					else 
						_installedPlugins.push_back(plugin);
				}
				else
				{
					// plugin is not known, so just fill in the details we know
					Plugin* plugin = new Plugin();
					plugin->setName(pluginName);
					plugin->setFilename(pluginFilename);
					setInstalledVersion(pluginFilename, plugin);
					_installedPlugins.push_back(plugin);
				}

			}
			
		} while(::FindNextFile(hFindFile, &foundData));

		PluginContainer::iterator iter = _plugins.begin();
		while (iter != _plugins.end())
		{
			if (!iter->second->isInstalled())
				_availablePlugins.push_back(iter->second);
			++iter;
		}


	}

		
	return TRUE;
}


tstring PluginList::getPluginName(tstring pluginFilename)
{
	HINSTANCE pluginInstance = ::LoadLibrary(pluginFilename.c_str());
	if (pluginInstance)
	{
		PFUNCISUNICODE pFuncIsUnicode = (PFUNCISUNICODE)GetProcAddress(pluginInstance, "isUnicode");
		BOOL isUnicode;
		if (!pFuncIsUnicode || !pFuncIsUnicode())
			isUnicode = FALSE;
		else
			isUnicode = TRUE;

		PFUNCGETNAME pFuncGetName = (PFUNCGETNAME)GetProcAddress(pluginInstance, "getName");
		if (pFuncGetName)
		{
			CONST TCHAR* pluginName = pFuncGetName();
			if (pluginName)
			{
				tstring tpluginName = pluginName;
				return tpluginName;
			}
		}
		else
		{
			throw tstring(_T("Plugin name call not defined"));
		}
	}
	else
	{
		throw tstring(_T("Load library failed"));
	}
	
}

void PluginList::setInstalledVersion(tstring pluginFilename, Plugin* plugin)
{
	DWORD handle;
	DWORD bufferSize = ::GetFileVersionInfoSize(pluginFilename.c_str(), &handle);
	unsigned char* buffer = new unsigned char[bufferSize];
	::GetFileVersionInfo(pluginFilename.c_str(), handle, bufferSize, buffer);
	
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	UINT cbTranslate;

	VerQueryValue(buffer, 
              _T("\\VarFileInfo\\Translation"),
              (LPVOID*)&lpTranslate,
              &cbTranslate);

	if (cbTranslate)
	{

		HRESULT hr;
		TCHAR subBlock[50];
		hr = StringCchPrintf(subBlock, 50,
				_T("\\StringFileInfo\\%04x%04x\\FileVersion"),
				lpTranslate[0].wLanguage,
				lpTranslate[0].wCodePage);

		if (FAILED(hr))
		{
			return;
		}
		else
		{
			TCHAR *fileVersion;
			UINT fileVersionLength;
			::VerQueryValue(buffer, subBlock, reinterpret_cast<LPVOID *>(&fileVersion), &fileVersionLength);

			plugin->setInstalledVersion(PluginVersion(fileVersion));

		}
	}


}


PluginListContainer& PluginList::getInstalledPlugins()
{
	return _installedPlugins;
}

PluginListContainer& PluginList::getAvailablePlugins()
{
	return _availablePlugins;
}

PluginListContainer& PluginList::getUpdateablePlugins()
{
	return _updateablePlugins;
}