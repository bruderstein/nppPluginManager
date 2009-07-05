
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <set>

#include "PluginList.h"
#include "PluginManager.h"

#include "tinyxml.h"
#include "InstallStep.h"
#include "DownloadStep.h"
#include "InstallStepFactory.h"
#include "md5.h"

#include <strsafe.h>


using namespace std;
using namespace boost;

typedef BOOL (__cdecl * PFUNCISUNICODE)();


PluginList::PluginList(void)
{
	_variableHandler = NULL;
}

PluginList::~PluginList(void)
{
	if (_variableHandler)
		delete _variableHandler;
}

void PluginList::init(NppData *nppData)
{
	_nppData = nppData;
	TCHAR configDir[MAX_PATH];
	TCHAR nppDir[MAX_PATH];
	TCHAR pluginDir[MAX_PATH];

	::SendMessage(nppData->_nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(configDir));
	::SendMessage(nppData->_nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(nppDir));
	_tcscpy_s(pluginDir, MAX_PATH, nppDir);
	_tcscat_s(pluginDir, MAX_PATH, _T("\\plugins"));
	
	_variableHandler = new VariableHandler(nppDir, pluginDir, configDir);
}



BOOL PluginList::parsePluginFile(CONST TCHAR *filename)
{
	/*int len = _tcslen(filename) + 2;
	char *cFilename = new char[len];
	wcstombs(cFilename, filename, len);
	*/
	TiXmlDocument doc(filename);
	
	doc.LoadFile();
	if (doc.Error())
	{
		return FALSE;
	}

	TiXmlNode *pluginsDoc = doc.FirstChildElement(_T("plugins"));
	if (pluginsDoc)
	{
		TiXmlElement *pluginNode = pluginsDoc->FirstChildElement(_T("plugin"));
		Plugin *plugin;

		while(pluginNode)
		{
			plugin = new Plugin();

			plugin->setName(pluginNode->Attribute(_T("name")));
			 
			BOOL available = FALSE;

			if (g_isUnicode)
			{
				TiXmlElement *versionUrlElement = pluginNode->FirstChildElement(_T("unicodeVersion"));
				if (versionUrlElement && versionUrlElement->FirstChild())
				{
					plugin->setVersion(PluginVersion(versionUrlElement->FirstChild()->Value()));
					available = TRUE;
				}
			}
			else 
			{
				TiXmlElement *versionUrlElement = pluginNode->FirstChildElement(_T("ansiVersion"));
				if (versionUrlElement && versionUrlElement->FirstChild())
				{
					plugin->setVersion(PluginVersion(versionUrlElement->FirstChild()->Value()));
					available = TRUE;
				}

			}

			TiXmlElement *descriptionUrlElement = pluginNode->FirstChildElement(_T("description"));
			if (descriptionUrlElement && descriptionUrlElement->FirstChild())
				plugin->setDescription(descriptionUrlElement->FirstChild()->Value());

			TiXmlElement *filenameUrlElement = pluginNode->FirstChildElement(_T("filename"));
			if (filenameUrlElement && filenameUrlElement->FirstChild())
				plugin->setFilename(filenameUrlElement->FirstChild()->Value());
			
			TiXmlElement *versionsUrlElement = pluginNode->FirstChildElement(_T("versions"));
			
			if (versionsUrlElement)
			{
				TiXmlElement *versionUrlElement = versionsUrlElement->FirstChildElement(_T("version"));
				while(versionUrlElement)
				{
					plugin->addVersion(versionUrlElement->Attribute(_T("md5")), PluginVersion(versionUrlElement->Attribute(_T("number"))));
					versionUrlElement = (TiXmlElement *)versionsUrlElement->IterateChildren(versionUrlElement);
				}
			}

			TiXmlElement *installElement = pluginNode->FirstChildElement(_T("install"));
			
			addInstallSteps(plugin, installElement);
			
			

			TiXmlElement *dependencies = pluginNode->FirstChildElement(_T("dependencies"));
			if (dependencies && !dependencies->NoChildren())
			{
				TiXmlElement *dependency = dependencies->FirstChildElement();
				while (dependency)
				{
					// If dependency is another plugin (currently the only supported dependency)
					if (!_tcscmp(dependency->Value(), _T("plugin")))
					{
						const TCHAR* dependencyName = dependency->Attribute(_T("name"));
						if (dependencyName)
							plugin->addDependency(dependencyName);
					}

					dependency = reinterpret_cast<TiXmlElement*>(dependencies->IterateChildren(dependency));
				}
			}


			

			if (available)
				_plugins[plugin->getName()] = plugin;
			
	

			pluginNode = (TiXmlElement *)pluginsDoc->IterateChildren(pluginNode);
		}
	}
	return TRUE;
}

void PluginList::addInstallSteps(Plugin* plugin, TiXmlElement* installElement)
{
	if (!installElement)
		return;

	TiXmlElement *installStepElement = installElement->FirstChildElement();

	InstallStepFactory installStepFactory(_variableHandler);

	while (installStepElement)
	{
		// If it is a unicode tag, then only process the contents if it's a unicode N++
		// or if it's an ansi tag, then only process the contents if it's an ansi N++
		if ((g_isUnicode 
			&& !_tcscmp(installStepElement->Value(), _T("unicode")) 
			&& installStepElement->FirstChild())
			||
			(!g_isUnicode
			&& !_tcscmp(installStepElement->Value(), _T("ansi")) 
			&& installStepElement->FirstChild()))
		{
			addInstallSteps(plugin, installStepElement);
		}
		else 
		{

			shared_ptr<InstallStep> installStep = installStepFactory.create(installStepElement, g_options.proxy.c_str(), g_options.proxyPort);
			if (installStep.get()) 
				plugin->addInstallStep(installStep);

		}


		
		installStepElement = (TiXmlElement *)installElement->IterateChildren(installStepElement);
	}
}


BOOL PluginList::checkInstalledPlugins(TCHAR *pluginPath)
{
	tstring pluginsFullPathFilter;
	
	pluginsFullPathFilter.append(pluginPath);

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
					plugin->setDescription(_T("Unknown plugin"));

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
			else
				return _T("");

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

VariableHandler* PluginList::getVariableHandler()
{
	return _variableHandler;
}

Plugin* PluginList::getPlugin(tstring name)
{
	return _plugins[name];
}

BOOL PluginList::isInstallOrUpgrade(const tstring& name)
{
	Plugin* plugin = _plugins[name];
	if (plugin->isInstalled() && plugin->getVersion() <= plugin->getInstalledVersion())
		return FALSE;
	else
		return TRUE;
}




shared_ptr< list<tstring> > PluginList::calculateDependencies(shared_ptr< list<Plugin*> > selectedPlugins)
{
	set<tstring> toBeInstalled;
	shared_ptr< list<tstring> > installDueToDepends(new list<tstring>);


	// First add all selected plugins to a name map
	list<Plugin*>::iterator pluginIter = selectedPlugins->begin();
	while (pluginIter != selectedPlugins->end())
	{
		toBeInstalled.insert((*pluginIter)->getName());
		++pluginIter;
	}

	// Now check all dependencies are in the name map
	pluginIter = selectedPlugins->begin();
	while(pluginIter != selectedPlugins->end())
	{
		if ((*pluginIter)->hasDependencies())
		{
			
			list<tstring> dependencies = (*pluginIter)->getDependencies();
			list<tstring>::iterator depIter = dependencies.begin();
			while(depIter != dependencies.end())
			{
				if (toBeInstalled.count(*depIter) == 0)
				{
					// if not already selected to be installed, then add it to the list
					if (isInstallOrUpgrade(*depIter))
					{
						Plugin* dependsPlugin = getPlugin(*depIter);
						toBeInstalled.insert(*depIter);

						selectedPlugins->push_back(dependsPlugin);
						// Add the name to the list to show the message
						installDueToDepends->push_back(*depIter); 
					}
				}

				++depIter;
			}
		}
		++pluginIter;

	}

	return installDueToDepends;
}
