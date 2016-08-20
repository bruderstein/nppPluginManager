## Plugin Manager

* * *

[Features](#features)   [Changes](#changes)   [Download](#download)   [Installation](#install)   [XML File documentation](#XML)   [Plugin Developers](#developer)   [Submitting changes](#submitting)   [Credits](#credits)   [Contact](#contact)  

* * *

Plugin Manager is a plugin for Notepad++ that allows you to install, update and remove plugins from Notepad++. A centrally hosted XML file holds the list of plugins, that the plugin manager downloads and processes against the list of installed plugins.

<span class="important">Important note for Vista / Windows 7:</span> In order to update the plugin manager to 1.0.8 , you need to run Notepad++ as administrator, update plugin manager, then everything will run properly under a normal user. Future updates to Plugin Manager will automatically run as administrator.
[**Click here for help for update to Plugin Manager 1.0.8**](update.htm)<a name="features"></a>

### Features

*   Install, Update and Remove all existing plugins
*   Notify when an installed plugin has an update available, and allow the user to update directly
*   Calculate dependencies between plugins, any plugin can depend on another plugin, which will be installed automatically if the user installs the initial plugin
*   Install all supplemental files as well as the plugin file itself (e.g. config, doc, extra libraries)
*   Automatic installation of the correct version (ANSI/Unicode) for your Notepad++
*   Validation of DLLs copied, against a central list of valid files. This slightly mitigates the risk of automatically downloading updates.
*   Only plugins that are compatible with your Notepad++ are shown
*   Plugins can be put into categories - a suggestion was made of "Official, Common and Others". Currently all plugins are in the "Others" category, as I think there needs to be some agreement as to what determines an "Official" plugin and which plugins are "Common".
*   Plugin Manager is able to update itself, and will notify the user when an update is available
*   Proxies are supported, just change the setting
*   Updates can be ignored, and the user will not be alerted to that update again, until a further new version of that plugin is released.
*   The local plugin list file is only downloaded when something has changed - the MD5sum of the file is compared to the server's version, and if it is different, then a new copy is downloaded.
*   New for 0.9: Works with plugins that are installed multiple times - update them all or individually

<a name="changes"></a>

### Changes

1.3.5

*   Ignore cache for file validation

1.3.4

*   Fix broken fix in gpup.

1.3.3

*   Fix gpup version check to accept newer versions of gpup.exe (needed for PM update). <span class="important">This sadly does not fix people updating from 1.0.8, who will still get a warning when updating PM to 1.3.x, that the GPUP version doesn't match.</span>

1.3.2

*   Remove proxy settings from settings dialog

1.3.1

*   Remove unneeded proxy username and password from ini settings file if they're there

1.3.0

*   Handle proxy authentication with the system dialog

1.2.0

*   Downloads now using WinInet API instead of [libcurl](http://curl.haxx.se/libcurl/). Hence now using system proxy (same as IE and Chrome), and smaller binaries.
*   Better UAC support. If you start Notepad++ as a normal user, then install something, N++ will be restarted as the same user (not as admin as per previous versions)

1.0.8

*   Checks gpup.exe version is correct in order to ensure upgrade from previous version was successful

1.0.7

*   Fix for updating Plugin Manager itself
*   Now uses main FileVersion instead of translated FileVersion to detect plugin version

1.0.5

*   "Library" plugins properly supported
*   Validation url changed to npppluginmgr project on sourceforge

1.0.4

*   Fixed crypt32.dll loading dynamically (static link was removed)

1.0.3

*   Uninstall fix for appdata plugins
*   Infinite loop bug fixed when download location is actually an html site
*   Crypt32.dll now loaded dynamically, if it's available (for encrypting proxy authentication details)
*   Timeout for connection added when downloading

1.0.2

*   Support for authenticated proxies (SSPI (windows automatic) authenticated proxies are not supported)
*   Plugin list now hosted on sourceforge's file distribution system
*   Plugin list now zipped, reducing its size from (currently 120KiB to 29KiB)
*   Improved handling for UAC on Windows Vista and above
*   Support for the new AppData plugins - Notepad++ 5.9.7 and above
*   Improved download support - better recognition of .zip files, and ability to download files as "naked" files
*   Basic support for plugin removal steps added (meaning that removal of a plugin should any secondary files)
*   Improved Notepad++ restart handling - faster restarts
*   Initial support for a "run" install step added
*   Recursive copy support
*   Option to set how often plugin manager should check for plugin updates - minimum 5 days
*   Column headers can now be clicked to sort the plugin in lists in different orders
*   Path for plugin config files shown in settings dialog, making it easier to identify the type of installation you have

0.9.3.1

*   Fixed issue with Windows 7 / Vista, so you now get a UAC prompt on restart of N++
*   Fixed issue with Proxy Server settings being ignored when checking the MD5s of downloaded files

0.9.3

*   Fixed issue with extended characters in the Notepad++ path, or the user's home directory
*   Added aliases entry to XML file, so a plugin can be known by different names in different versions
*   Fixed an issue copying all files from a plugin's zip file (i.e. a *.* copy)

0.9.2.1

*   Fixed a minor issue if an variable was badly used in the XML
*   Changed "Show Unstable Plugins" to default to off

0.9.2

*   Limit the automatic check for updated plugins to once every 3 days, which reduces the load on our servers, and also quickens the startup of Notepad++ for the user.
*   Latest update added, so, on future updates, the update notice will be shown on the notify window, and also on the updates tab.
*   Stability column added to the list, with the ability to hide the unstable ones (enabled by default)
*   Newlines can now be added in the description, using \n, which makes them easier to read
*   Ability to highlight a version as unstable (aka "bad"), which means that a lower version number is notified for update.
    e.g. Plugin version 1.2 is good, then 1.5 is released. Plugin manager updates to 1.5, then stability reports come in about 1.5 not being stable. The XML can be updated such that the latest version is 1.2, and 1.5 marked as unstable. Anyone who has upgraded to 1.5 will then be asked if they want to "update" back to 1.2.
*   Plugins.xml and the validation database are now hosted on Sourceforge
*   Fixed an issue with removing plugins - the plugin file is now removed properly
*   Fixed a potential crash issue if a temporary download file could not be opened for writing
*   Fixed an issue if the plugin config directory needed multiple levels creating
*   A couple of code tidy ups

0.9

*   Plugins that are installed multiple times are now supported, and can be updated individually
*   The updater (gpup.exe) now shows a progress bar, useful feedback if you've installed or updated a lot of plugins
*   User will be presented with a warning if there is an update to the plugin manager that they haven't taken - it's always best to update the plugin manager first, then install or update other plugins
*   Rework of the variable handling in the XML - all variables are now processed at install time
*   New variable available, $PLUGINFILENAME$, which is (unsurprisingly) the filename of the installed plugin
*   Copy now supports a "toFile" attribute, which copies to a filename rather than a directory
*   Added facility in XML to set the name of an unknown plugin based on a hash value - so a plugin that returns a dynamic name can still be supported (the dynamic name will still be shown in the list to the user however, but install and update will be possible)
*   The user-agent header for all downloads now contains the version number of the plugin manager
*   Fixed an issue if the user said no to restarting after removing plugins

So, I hope you find this plugin useful, and it gives a bit more exposure to plugins, and makes it easier to install some of the bigger plugins.

If you don't see your plugin in the list, let me know (see contact at the end) and I'll make sure it goes in.

<a name="install"></a>

### Installation

To install the plugin manager, simply download (links below) the .zip, and place the PluginManager.dll file in the Notepad++ plugins directory, and the gpup.exe in the updater directory under your Notepad++ program directory. (e.g. "C:\Program Files\Notepad++\updater")

In fact, if you prefer, you can just add the PluginManager.dll to the plugins directory, then do a reinstall of Plugin Manager from the plugin itself, which will place the file in the right place! Of course, if you're already using an earlier version of the plugin manager, you'll be able to just update from the update tab (or when you get the notification that the update has happened).

<a name="download"></a>

### Download links

Official home: [http://sourceforge.net/projects/npppluginmgr/files/v1.3.5](http://sourceforge.net/projects/npppluginmgr/files/v1.3.5)
Unicode: [http://www.brotherstone.co.uk/npp/pm/PluginManager_1.3.5_UNI.zip](http://www.brotherstone.co.uk/npp/pm/PluginManager_1.3.5_UNI.zip)
ANSI (v.1.0.8): [http://www.brotherstone.co.uk/npp/pm/PluginManager_1.0.8_ANSI.zip](http://www.brotherstone.co.uk/npp/pm/PluginManager_1.0.8_ANSI.zip)
Source code is on github: [http://github.com/bruderstein/nppPluginManager](http://github.com/bruderstein/nppPluginManager)
MD5 Sums:
bed7b9b3c9cd4ae7eacb19f6f64eb55e PluginManager_1.3.5_UNI.zip
824573cdf9d08c1a8de369a9a92bce41 PluginManager_1.3.4_UNI.zip
931e1d021dc6fc0a80890df75da4a583 PluginManager_1.3.3_UNI.zip
5367b29d2be4e40632859bbb4bfb7d57 PluginManager_1.0.8_UNI.zip
ad273cfb1b0137a8e79b4df83a1e4d3e PluginManager_1.0.8_ANSI.zip
96f96623bc66713c6b7d13522f282f95 PluginManager_1.0.2_ANSI.zip
b646aae090998a1901bb04f816ee1372 PluginManager_1.0.2_UNI.zip

<u>Older versions</u> Older versions can be obtained from [http://sourceforge.net/projects/npppluginmgr/files/](http://sourceforge.net/projects/npppluginmgr/files/)

* * *

<a name="XML"></a>

### XML File Structure and Features

_**The XML structure is documented here for completeness. To edit the XML please use the** _[administration system](http://www.brotherstone.co.uk/npp/pm/admin)

The XML file consists of <plugin> elements, each with a name attribute, containing the information about each plugin. The name attribute must match (include case) the name provided by the plugins getName() function, i.e. that which appears in the Plugins menu in Notepad++.

From 0.9 onwards, a **pluginNames** element can be used, containing **pluginName** elements, to specify a plugin's name, if it reports it dynamically. The MD5sum is given of the file to identify it.
e.g.
<pluginNames>
    <pluginName md5="48ae59abb0224ecc252d35e953ce0496" name="NppExport" comment="ansi 0.2.8"/>
    <pluginName md5="ad32f0f59e70823de864387f65debcea" name="NppExport" comment="unicode 0.2.8"/>
</pluginNames>

The following elements can be contained within the plugin element(all are optional)

*   **unicodeVersion**: The version number of the latest Unicode version. If this element is not present, then the plugin will not be shown to Unicode users
*   **ansiVersion**: The version number of the latest ANSI version. If this element is not present, then the plugin will not be shown to ANSI users
*   **description**: A description of the plugin, which appears in the box below the list
*   **author**: The authors name (and optionally email address etc), which is also displayed below the list
*   **homepage**: The home page / web site of the plugin, also displayed in the box below the list
*   **sourceUrl**: The URL of the source of the plugin
*   **latestUpdate**: The latest updates the plugin has had. Shown on the notification of an update, and on the updates tab.
*   **stability**: If included, and not "Good", then the plugin is excluded if the user has "Show Unstable Plugins" disabled. Shown in the plugin list.
*   **badVersions**: Contains a list of unstable version numbers, that will always come below the latest version identified in unicodeVersion or ansiVersion tags.

    *   **version**: Attribute **number** identifies the version number as bad, attribute **report** is the status report, which is shown in the updates tab, and the notify window.
*   **aliases**: A list of **alias** elements with a **name** attribute, identifying other names this plugin is known by.
*   **install**: This is the set of install steps.
    The steps can be contained in <ansi> or <unicode> elements, which means that the steps are only performed for the relevant version. A step which is not inside a unicode or ansi element will be performed for both. Steps are run in the order in which they are listed in the XML.
    The steps that are available at the moment are:
    *   **download**: This downloads from the URL contained within the elment. A zip file is automatically unzipped to a temporary location.
        If the URL points to an HTML page which contains a direct link (like the old SourceForge download system... guess when I wrote this!) then the direct link can be searched for, and will be followed. To do this, just add a filename attribute to the download element, containing the filename that the direct link ends in.
    *   **copy**: This copies files from the temporary unzipped location to a destination directory. A destination directory must start with a variable, either $PLUGINDIR$, $CONFIGDIR$ or $NPPDIR$ which refer to the plugins directory, plugin config directory and the Notepad++ directory respectively. In 0.9, a variable $PLUGINFILENAME$ is available that is the current installed filename of the plugin. Note that this will be empty if the plugin is not installed. Directories that do not exist will automatically be created. The **from** attribute can be wildcarded, and the **to** attribute _must_ be a directory.
        If **validate="true"** is included in the copy, then the file's MD5sum is checked against a known list of valid files at the server - an "ok" response lets it continue, and "unknown" response means the user is prompted that the file is unknown, and should not be copied. The user can copy the file anyway, if they want. A "banned" response means that the user is prompted that this file has been marked as dangerous or unstable, so should not be copied. They can still choose to copy the file if they wish.
        If **backup="true"** is include in the copy element, then if the destination file exists, it will be backed up to the same filename with ".backup" appended. If the backup file exists, it will be backed up to a ".backup2" file, and so on. This is normally used for config files, such that user's config is not lost, but a "good" config can be installed.
        In 0.9 onwards, the **toFile** attribute (case sensitive) can be used instead of the **to** attribute, to specify that the destination is a file, rather than a directory. If **toFile** specifies an existing directory, or the name ends in a backslash (\), the destination is assumed to be a directory, and hence the file is simply copied with the same name. This can be used in conjunction with the $PLUGINFILENAME$ variable, to support plugins that can be installed multiple times. e.g.
        <copy from="ThePlugin.dll" toFile="$PLUGINDIR$\$PLUGINFILENAME$" />
        This will install normally (as toFile will spot that "$PLUGINDIR$\" is an existing directory, so copy the file as ThePlugin.dll, but updates will copy "ThePlugin.dll" to whatever that instance of the plugin is installed as (e.g. ThePlugin_Copy1.dll).
        If the **recursive** attribute is 'true', then all files in child directories are copied.
*   **versions**: A collection of **version** elements that identify a version if the DLL does not report a version (or reports an incorrect version). Each **version** element contains a **number** attribute, which is the correct version number, and an **md5** attribute which is the md5sum of the file. Optionally a **comment** attribute can be included to name the version (e.g. ANSI, UNICODE etc), which can help with maintaining the XML file.

Here's an example plugin definition, using f0dder's switcher plugin to switch between related files, such s .h and .cpp :

<pre><plugin name="switcher">
		<unicodeVersion>1.0</unicodeVersion>
		<ansiVersion>1.0</ansiVersion>
		<homepage>http://f0dder.dcmembers.com/npp_plugins/</homepage>
		<description>Switcher: a plugin to switch between 'associated' files. Currently it handles switching between asm&lt;&gt;inc, cpp&lt;&gt;h, cc&lt;&gt;h and c&lt;&gt;h. Useful when assigned to a hotkey.</description>
		<author>f0dder</author>
		<versions>
			<version number="1.0" md5="301c72341ba758b3cdfc8a56d576c7f0" comment="ansi"/>
			<version number="1.0" md5="5a19bd60aa18e4454f1c4a6432ae5a72" comment="unicode"/>
		</versions>
		<aliases>
			<alias name="Extension Switcher" />
		</aliases>
		<install>
			<download>http://f0dder.dcmembers.com/npp_plugins/npp_plugins.zip</download>
			<ansi>
				<copy from="ansi\switcher.dll" to="$PLUGINDIR$" validate="true"/>
			</ansi>
			<unicode>
				<copy from="unicode\switcher.dll" to="$PLUGINDIR$" validate="true"/>
			</unicode>
			<copy from="readme.txt" to="$PLUGINDIR$\doc\switcher"/>
		</install>
</plugin>
</pre>

Note that you can have as many steps as you like within the <ansi> and <unicode> elements, and you can have as many of these as you like also. So:

<pre><install>
			<unicode>
				<download>http://downloads.sourceforge.net/sourceforge/npp-plugins/SpeechPlugin_0_2_1_dll.zip</download>
				<copy from="SpeechPlugin_0_2_1_dll\SpeechPlugin.dll" to="$PLUGINDIR$" validate="true"/>
			</unicode>
			<ansi>
				<download>http://downloads.sourceforge.net/sourceforge/npp-plugins/SpeechPlugin_0_2_dll.zip</download>
				<copy from="SpeechPlugin_0_2_dll\SpeechPlugin.dll" to="$PLUGINDIR$" validate="true"/>
			</ansi>
</install>
</pre>

is just as valid as, and does the same job as

<pre><install>
			<unicode>
				<download>http://downloads.sourceforge.net/sourceforge/npp-plugins/SpeechPlugin_0_2_1_dll.zip</download>
			</unicode>
			<ansi>
				<download>http://downloads.sourceforge.net/sourceforge/npp-plugins/SpeechPlugin_0_2_dll.zip</download>
			</ansi>

			<unicode>
				<copy from="SpeechPlugin_0_2_1_dll\SpeechPlugin.dll" to="$PLUGINDIR$" validate="true"/>
			</unicode>
			<ansi>
				<copy from="SpeechPlugin_0_2_dll\SpeechPlugin.dll" to="$PLUGINDIR$" validate="true"/>
			</ansi>
</install>
</pre>

The second is obviously just longer, and the first would probably be preferred.

* * *

<a name="developer"></a>

### Plugin developers

If you're a plugin developer or maintainer, and you don't want your plugin in the list, or you want to wait a while until it's stable, then just let me know. I won't be releasing any updated plugins as soon as they are released, only after a few days when any bug reports have had chance to come in - unless there's a very good reason.

There's a web based system for adding or updating your plugin - please use this to update your plugin

#### How to get your plugin listed

Make an announcement on the [Plugin Development forum](http://sourceforge.net/projects/notepad-plus/forums/forum/482781/) of the Notepad++ project. Then enter your plugin (or update to your plugin) on the administration system. Your update will go out with the next batch of updates.
**IMPORTANT:** If you haven't announced your plugin (or new version) on the Plugin development forum, it doesn't go in Plugin Manager. Period. This is a safety feature, so we don't distribute a random binary through Plugin Manager, without first giving people the opportunity to download it themselves, virus check it, and, most importantly check it doesn't do anything harmful.

Due to pressure of time, updates tend to be bunched up over a few weeks (shorter if there have been a lot of updates). This eases the burden on testing, and also means that the XML isn't downloaded by 1-2 million users TOO often (that's nearly 200GB download every time we update).

* * *

<a name="submitting"></a>

### Submitting changes to the XML Plugin List

As of October 2011, there's a new web based system for plugin developers and maintainers to update and add their plugins.
This system was kindly developed and donated by Pedro Sland, and I'm very grateful for his work on it.

Plugin developers and maintainers and welcome to register for an account at [http://www.brotherstone.co.uk/npp/pm/admin](http://www.brotherstone.co.uk/npp/pm/admin)

It's obviously important to test that your modifications work, so for that we have a testing version of the Plugin Manager.

1.  Download the testing version of Plugin Manager here: [Unicode](http://www.brotherstone.co.uk/npp/pm/PluginManager_TEST_UNI.zip) / [ANSI](http://www.brotherstone.co.uk/npp/pm/PluginManager_TEST_ANSI.zip)
2.  Add the following lines to your PluginManager.ini (under plugins\Config, or %APPDATA%\notepad++\plugins\config)

    <pre>xmlurl=http://www.brotherstone.co.uk/npp/pm/admin/plugins/generate_xml
    md5url=http://www.brotherstone.co.uk/npp/pm/admin/dummy.md5.txt
    </pre>

This version downloads the XML directly from the admin site, and validates the MD5s against the hashes that have been added on the admin site. Please test your changes.

This is also a "debug" build, so if something goes wrong, you can step through it. The PDBs are also included in the zip file.

The current XML file is located **[here](http://sourceforge.net/projects/npppluginmgr/files/xml)**, if you want to check it out.

**The old git based XML modification system has been discontinued, and we are now exclusively using the web based system**

* * *

<a name="planned"></a>Next steps after 1.0.2, who knows:

*   7zip support. Currently only zip files are supported, but 7zip is smaller, and being able to download the smaller version of plugins is always a plus. I've moved this from 1.0 as I don't think it's that necessary, just a nice to have.
*   A secondary (larger) download version of Plugin Manager that supports SSPI authenticated proxies.
*   Rollback to a previously installed version.
*   Some form of plugin feedback / rating system

Enjoy, and please let me know what you think, any feedback is gratefully received! You can either post a comment to the Plugin Development forum, or sometimes find me in #notepad++ on freenode.<a name="credit"></a>

* * *

### Credits

I must say a big thank you to Joce, who helped a lot in the development of this plugin.
A big thanks to Pedro Sland, for developing the [administration system](http://www.brotherstone.co.uk/npp/pm/admin). This has massively reduced the workload for updating the list, and enabled all plugin developers to
The good people on #notepad++ (especially joce and Thell) who've helped out with ideas and code reviews
Don Ho and all the contributors, for bringing us this great editor, and providing such a neat plugin architecture that developers can easily add stuff<a name="contact"></a>

* * *

### Contact

Dave Brotherstone
Email is ![](email.png)
Find me on IRC in [#notepad++](irc://irc.freenode.net/#notepad++) on Freenode, davegb3.