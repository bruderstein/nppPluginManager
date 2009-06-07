#include "PluginList.h"
#include "PluginManager.h"

#include "tinyxml.h"
#include <strsafe.h>
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include "InstallStep.h"
#include "DownloadStep.h"
#include "md5.h"

using namespace std;
using namespace boost;

typedef BOOL (__cdecl * PFUNCISUNICODE)();


PluginList::PluginList(void)
{
}

PluginList::~PluginList(void)
{
}

void PluginList::init(NppData *nppData)
{
	_nppData = nppData;
}


BOOL PluginList::parsePluginFile(TCHAR *filename)
{
	int len = _tcslen(filename) + 2;
	char *cFilename = new char[len];
	wcstombs(cFilename, filename, len);
	TiXmlDocument doc(cFilename);
	
	doc.LoadFile();
	if (doc.Error())
	{
		const char *er = doc.ErrorDesc();
		int l = strlen(er);
	}

	TiXmlNode *pluginsDoc = doc.FirstChildElement("plugins");
	if (pluginsDoc)
	{
		TiXmlElement *pluginNode = pluginsDoc->FirstChildElement("plugin");
		Plugin *plugin;

		while(pluginNode)
		{
			plugin = new Plugin();

			plugin->setName(pluginNode->Attribute("name"));
			 
			BOOL available;

			if (g_isUnicode)
			{
				TiXmlElement *versionUrlElement = pluginNode->FirstChildElement("unicodeVersion");
				if (versionUrlElement && versionUrlElement->FirstChild())
				{
					plugin->setVersion(PluginVersion(versionUrlElement->FirstChild()->Value()));
					available = TRUE;
				}
			}
			else 
			{
				TiXmlElement *versionUrlElement = pluginNode->FirstChildElement("ansiVersion");
				if (versionUrlElement && versionUrlElement->FirstChild())
				{
					plugin->setVersion(PluginVersion(versionUrlElement->FirstChild()->Value()));
					available = TRUE;
				}

			}

			TiXmlElement *descriptionUrlElement = pluginNode->FirstChildElement("description");
			if (descriptionUrlElement && descriptionUrlElement->FirstChild())
				plugin->setDescription(descriptionUrlElement->FirstChild()->Value());

			TiXmlElement *filenameUrlElement = pluginNode->FirstChildElement("filename");
			if (filenameUrlElement && filenameUrlElement->FirstChild())
				plugin->setFilename(filenameUrlElement->FirstChild()->Value());
			
			TiXmlElement *versionsUrlElement = pluginNode->FirstChildElement("versions");
			
			if (versionsUrlElement)
			{
				TiXmlElement *versionUrlElement = versionsUrlElement->FirstChildElement("version");
				while(versionUrlElement)
				{
					plugin->addVersion(versionUrlElement->Attribute("md5"), PluginVersion(versionUrlElement->Attribute("number")));
					versionUrlElement = (TiXmlElement *)versionsUrlElement->IterateChildren(versionUrlElement);
				}
			}

			TiXmlElement *installElement = pluginNode->FirstChildElement("install");
			if (installElement)
			{
				
				TiXmlElement *installStepElement = installElement->FirstChildElement();
				while (installStepElement)
				{
					if (!strcmp(installStepElement->Value(), "download"))
					{
						shared_ptr<DownloadStep> downloadStep(new DownloadStep(installStepElement->FirstChild()->Value()));
						plugin->addInstallStep(downloadStep);
					}
					installStepElement = (TiXmlElement *)installElement->IterateChildren(installStepElement);
				}
			}

			if (available)
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
					
					TCHAR hashBuffer[(MD5LEN * 2) + 1];
					
					if (MD5::hash(pluginFilename.c_str(), hashBuffer, (MD5LEN * 2) + 1))
					{
						tstring hash = hashBuffer;
						plugin->setInstalledVersionFromHash(hash);
					}
				
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
					/*
					TCHAR hashBuffer[(MD5LEN * 2) + 1];
					tstring plugintext = _T("");
					plugintext.append(_T("<plugin name=\""));
					plugintext.append(pluginName);
					plugintext.append(_T("\"><unicodeVersion>"));
					plugintext.append(plugin->getInstalledVersion().getDisplayString());
					plugintext.append(_T("</unicodeVersion>"));
					plugintext.append(_T("<ansiVersion>"));
					plugintext.append(plugin->getInstalledVersion().getDisplayString());
					plugintext.append(_T("</ansiVersion>"));

					if (plugin->getInstalledVersion() == PluginVersion(0,0,0,0))
					{
						plugintext.append(_T("<versions><version number=\"1.0.0.0\" md5=\""));
						MD5::hash(pluginFilename.c_str(), hashBuffer, (MD5LEN * 2) + 1);
						plugintext.append(hashBuffer);
						plugintext.append(_T("\"/></versions>"));
					}
					plugintext.append(_T("<sourceURL></sourceURL>"));
					
					plugintext.append(_T("<install><download></download><ansi></ansi><unicode></unicode></install>\r\n"));


					plugintext.append(_T("</plugin>"));
					char *dest = new char[plugintext.size() + 1];
					wcstombs(dest, plugintext.c_str(), plugintext.size() + 1);
					::SendMessage(_nppData->_scintillaMainHandle, SCI_INSERTTEXT, 0, reinterpret_cast<LPARAM>(dest));
					*/
					plugin->setDescription("Unknown plugin");

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
				tstring::size_type ampPosition = tpluginName.find(_T("&"));
				while(ampPosition != tstring::npos)
				{
					tpluginName.replace(ampPosition, 1, _T(""));
					ampPosition = tpluginName.find(_T("&"));
				}

				::FreeLibrary(pluginInstance);
				
				return tpluginName;
			}

			::FreeLibrary(pluginInstance);
		}
		else
		{
			::FreeLibrary(pluginInstance);
			throw tstring(_T("Plugin name call not defined"));
		}
	}
	else
	{
		throw tstring(_T("Load library failed"));
	}
	
}

BOOL PluginList::setInstalledVersion(tstring pluginFilename, Plugin* plugin)
{
	DWORD handle;
	DWORD bufferSize = ::GetFileVersionInfoSize(pluginFilename.c_str(), &handle);
	
	if (bufferSize <= 0) 
		return FALSE;

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
			return FALSE;
		}
		else
		{
			TCHAR *fileVersion;
			UINT fileVersionLength;
			::VerQueryValue(buffer, subBlock, reinterpret_cast<LPVOID *>(&fileVersion), &fileVersionLength);

			plugin->setInstalledVersion(PluginVersion(fileVersion));
			
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;


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