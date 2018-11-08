# nppPluginManager

[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/bruderstein/npppluginmanager?branch=master&svg=true)](https://ci.appveyor.com/project/bruderstein/npppluginmanager)
[![GitHub release](https://img.shields.io/github/release/bruderstein/npppluginmanager.svg)](https://github.com/bruderstein/nppPluginManager/releases)
[![Github Releases](https://img.shields.io/github/downloads/bruderstein/nppPluginManager/latest/total.svg)](https://github.com/bruderstein/nppPluginManager/releases)

Plugin Manager is a plugin for [Notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus) that allows you to install, update and remove plugins from Notepad++. A centrally hosted XML file holds the list of plugins, that the plugin manager downloads and processes against the list of installed plugins. 

 ### Installation
 
To install the plugin manager, simply download ([release section](https://github.com/bruderstein/nppPluginManager/releases)) the .zip, and place the PluginManager.dll file in the Notepad++ plugins directory, and the gpup.exe in the updater directory under your Notepad++ program directory. (e.g. "C:\Program Files\Notepad++\updater")

In fact, if you prefer, you can just add the PluginManager.dll to the plugins directory, then do a reinstall of Plugin Manager from the plugin itself, which will place the file in the right place! Of course, if you're already using an earlier version of the plugin manager, you'll be able to just update from the update tab (or when you get the notification that the update has happened).

Additionally for the x64 version there is a Chocolatey package for [nppPluginManager](https://chocolatey.org/packages/notepadplusplus-npppluginmanager) so that it can be installed with one command for N++ installer version:

`choco install notepadplusplus-nppPluginManager`

### **Documentation**

More information available in the **[DOC section](https://rawgit.com/bruderstein/nppPluginManager/master/doc/index.html)**.

