#include "FrameworkPlugin/CPluginManager.h"
#include "FrameworkPlugin/CPlugin.h"
#include "FrameworkLUA/CWrapperLua.h"
#include <algorithm>
#include <set>

using namespace FrameworkPlugin;
using namespace FrameworkLUA;
using namespace FrameworkCore;
using namespace std;

// protorype of the function for loading the plugin
typedef CPlugin *(*CREATE_FCN_TYPE)();
typedef void (*DESTROY_FCN_TYPE)();
typedef bool (*TEST_FCN_TYPE)();
typedef bool (*ISDEBUG_FCN_TYPE)();

/*
  ____ ____  _             _       __  __
 / ___|  _ \| |_   _  __ _(_)_ __ |  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |   | |_) | | | | |/ _` | | '_ \| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___|  __/| | |_| | (_| | | | | | |  | | (_| | | | | (_| | (_| |  __/ |
 \____|_|   |_|\__,_|\__, |_|_| |_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                     |___/                                  |___/
*/
CPluginManager::CPluginManager(std::string fname)  : _managedClass("") {
    SetErrorClassPrefix("CPluginManager");
    _loadedPlugins.clear();
    _moduleHandle.clear();
    InitializeSearchPath();
    if (fname.length() > 0)
        LoadPluginsFromFile(fname);
    }

/*
 /\/|___ ____  _             _       __  __
|/\/ ___|  _ \| |_   _  __ _(_)_ __ |  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |   | |_) | | | | |/ _` | | '_ \| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___|  __/| | |_| | (_| | | | | | |  | | (_| | | | | (_| | (_| |  __/ |
   \____|_|   |_|\__,_|\__, |_|_| |_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                       |___/                                  |___/
*/
CPluginManager::~CPluginManager() {
    // unload all plugins that are still loaded
    UnloadAllPlugins();
    _searchPath.clear();
    _managedClass = "";
    }

/*
 ____       _   ____  _             _        ____ _
/ ___|  ___| |_|  _ \| |_   _  __ _(_)_ __  / ___| | __ _ ___ ___
\___ \ / _ \ __| |_) | | | | |/ _` | | '_ \| |   | |/ _` / __/ __|
 ___) |  __/ |_|  __/| | |_| | (_| | | | | | |___| | (_| \__ \__ \
|____/ \___|\__|_|   |_|\__,_|\__, |_|_| |_|\____|_|\__,_|___/___/
                              |___/
*/
void CPluginManager::SetPluginClass(std::string plugins_class) {
    _managedClass = plugins_class;
    }

/*
    _                               _ ____                      _     ____       _   _
   / \   _ __  _ __   ___ _ __   __| / ___|  ___  __ _ _ __ ___| |__ |  _ \ __ _| |_| |__
  / _ \ | '_ \| '_ \ / _ \ '_ \ / _` \___ \ / _ \/ _` | '__/ __| '_ \| |_) / _` | __| '_ \
 / ___ \| |_) | |_) |  __/ | | | (_| |___) |  __/ (_| | | | (__| | | |  __/ (_| | |_| | | |
/_/   \_\ .__/| .__/ \___|_| |_|\__,_|____/ \___|\__,_|_|  \___|_| |_|_|   \__,_|\__|_| |_|
        |_|   |_|
*/
void CPluginManager::AppendSearchPath(std::string path) {
    // adds the path at the end of the list
    replace(path.begin(), path.end(), '/', '\\');
    _searchPath.push_back(path);
    }

/*
 ___                     _   ____                      _     ____       _   _
|_ _|_ __  ___  ___ _ __| |_/ ___|  ___  __ _ _ __ ___| |__ |  _ \ __ _| |_| |__
 | || '_ \/ __|/ _ \ '__| __\___ \ / _ \/ _` | '__/ __| '_ \| |_) / _` | __| '_ \
 | || | | \__ \  __/ |  | |_ ___) |  __/ (_| | | | (__| | | |  __/ (_| | |_| | | |
|___|_| |_|___/\___|_|   \__|____/ \___|\__,_|_|  \___|_| |_|_|   \__,_|\__|_| |_|
*/
void CPluginManager::InsertSearchPath(std::string path) {
    // adds the path at the beginning of the list
    replace(path.begin(), path.end(), '/', '\\');
    _searchPath.push_front(path);
    }

/*
 ____                               ____                      _     ____       _   _
|  _ \ ___ _ __ ___   _____   _____/ ___|  ___  __ _ _ __ ___| |__ |  _ \ __ _| |_| |__
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \___ \ / _ \/ _` | '__/ __| '_ \| |_) / _` | __| '_ \
|  _ <  __/ | | | | | (_) \ V /  __/___) |  __/ (_| | | | (__| | | |  __/ (_| | |_| | | |
|_| \_\___|_| |_| |_|\___/ \_/ \___|____/ \___|\__,_|_|  \___|_| |_|_|   \__,_|\__|_| |_|
*/
void CPluginManager::RemoveSearchPath(std::string path) {
    // cycle through the elements into the queue and remove the path if founded
    replace(path.begin(), path.end(), '/', '\\');
    _searchPathListType::iterator it = _searchPath.begin();
    while (it != _searchPath.end()) {
        if (path.compare(*it) == 0) {
            _searchPath.erase(it);
            return;
            }
        it++;
        }
    }

/*
  ____ _                 ____                      _     ____       _
 / ___| | ___  __ _ _ __/ ___|  ___  __ _ _ __ ___| |__ |  _ \ __ _| |_
| |   | |/ _ \/ _` | '__\___ \ / _ \/ _` | '__/ __| '_ \| |_) / _` | __|
| |___| |  __/ (_| | |   ___) |  __/ (_| | | | (__| | | |  __/ (_| | |_
 \____|_|\___|\__,_|_|  |____/ \___|\__,_|_|  \___|_| |_|_|   \__,_|\__|
*/
void CPluginManager::ClearSearchPath() {
    _searchPath.clear();
}
/*
  ____      _   ____                      _     ____       _   _     _     _     _
 / ___| ___| |_/ ___|  ___  __ _ _ __ ___| |__ |  _ \ __ _| |_| |__ | |   (_)___| |_
| |  _ / _ \ __\___ \ / _ \/ _` | '__/ __| '_ \| |_) / _` | __| '_ \| |   | / __| __|
| |_| |  __/ |_ ___) |  __/ (_| | | | (__| | | |  __/ (_| | |_| | | | |___| \__ \ |_
 \____|\___|\__|____/ \___|\__,_|_|  \___|_| |_|_|   \__,_|\__|_| |_|_____|_|___/\__|
*/
vector<string> CPluginManager::GetSearchPathList() {
    vector<string> list;
    list.reserve(_searchPath.size());
    _searchPathListType::iterator it = _searchPath.begin();
    while (it != _searchPath.end()) {
        list.push_back(*it);
        it++;
        }
    return list;
    }

/*
 ____                  _____          ____  _             _       ____       _   _
/ ___|  ___ __ _ _ __ |  ___|__  _ __|  _ \| |_   _  __ _(_)_ __ |  _ \ __ _| |_| |__  ___
\___ \ / __/ _` | '_ \| |_ / _ \| '__| |_) | | | | |/ _` | | '_ \| |_) / _` | __| '_ \/ __|
 ___) | (_| (_| | | | |  _| (_) | |  |  __/| | |_| | (_| | | | | |  __/ (_| | |_| | | \__ \
|____/ \___\__,_|_| |_|_|  \___/|_|  |_|   |_|\__,_|\__, |_|_| |_|_|   \__,_|\__|_| |_|___/
                                                    |___/
*/
vector<string> CPluginManager::ScanForPluginPaths() {
    WIN32_FIND_DATA ffd;
    vector<string> list;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    list.clear();
    // get all the files listed in the plugins directory and try to load the dll as plugins
    _searchPathListType::iterator it = _searchPath.begin();
    // start search the plugin
    while (it != _searchPath.end()) {
        // list the dll files into the actual plugin path
        char szDir[MAX_PATH];
        string cmd = *it + string("*.dll");
        strcpy(szDir,cmd.c_str());
        hFind = FindFirstFile(szDir, &ffd);
        // look for some results
        if (hFind != INVALID_HANDLE_VALUE) {
            // some result founded, try to load the dlls
            do {
                if (LoadPlugin(string(ffd.cFileName),true)) {
                    // plugin test OK
                    list.push_back(*it + string(ffd.cFileName));
                    }
                } while (FindNextFile(hFind, &ffd) != 0);
            }
        it++;
        }
    return list;
    }

/*
 ____                  _____          ____  _             _       _   _
/ ___|  ___ __ _ _ __ |  ___|__  _ __|  _ \| |_   _  __ _(_)_ __ | \ | | __ _ _ __ ___   ___ ___
\___ \ / __/ _` | '_ \| |_ / _ \| '__| |_) | | | | |/ _` | | '_ \|  \| |/ _` | '_ ` _ \ / _ Y __|
 ___) | (_| (_| | | | |  _| (_) | |  |  __/| | |_| | (_| | | | | | |\  | (_| | | | | | |  __|__ \
|____/ \___\__,_|_| |_|_|  \___/|_|  |_|   |_|\__,_|\__, |_|_| |_|_| \_|\__,_|_| |_| |_|\___|___/
                                                    |___/
*/
vector<string> CPluginManager::ScanForPluginNames() {
    WIN32_FIND_DATA ffd;
    vector<string> list;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::set<std::string> valid_plugins;

    list.clear();
    valid_plugins.clear();
    // get all the files listed in the plugins directory and try to load the dll as plugins
    _searchPathListType::iterator it = _searchPath.begin();
    // start search the plugin
    while (it != _searchPath.end()) {
        // list the dll files into the actual plugin path
        char szDir[MAX_PATH];
        string cmd = *it + string("*.dll");
        strcpy(szDir,cmd.c_str());
        hFind = FindFirstFile(szDir, &ffd);
        // look for some results
        if (hFind != INVALID_HANDLE_VALUE) {
            // some result founded, try to load the dlls
            do {
                string plugin_name = string(ffd.cFileName);
                // remove ".dll" from the plugin name and remove _d if present
                string pname = plugin_name.substr(0,plugin_name.length()-4);
                if (pname.substr(pname.length()-2)== "_d" || pname.substr(pname.length()-2)== "_D")
                    pname.resize(pname.length()-2);
                if (LoadPlugin(pname,true)) {
                    if (valid_plugins.find(pname) == valid_plugins.end()) {
                        // plugin test OK, no double names allowed
                        list.push_back(pname);
                        valid_plugins.insert(pname);
                        }
                    }
                } while (FindNextFile(hFind, &ffd) != 0);
            }
        it++;
        }
    return list;

    }
/*
 ___       _ _   _       _ _         ____                      _     ____       _   _
|_ _|_ __ (_) |_(_) __ _| (_)_______/ ___|  ___  __ _ _ __ ___| |__ |  _ \ __ _| |_| |__
 | || '_ \| | __| |/ _` | | |_  / _ \___ \ / _ \/ _` | '__/ __| '_ \| |_) / _` | __| '_ \
 | || | | | | |_| | (_| | | |/ /  __/___) |  __/ (_| | | | (__| | | |  __/ (_| | |_| | | |
|___|_| |_|_|\__|_|\__,_|_|_/___\___|____/ \___|\__,_|_|  \___|_| |_|_|   \__,_|\__|_| |_|
*/
void CPluginManager::InitializeSearchPath() {
    // clear list
    _searchPath.clear();
    // get application path
    char result[ MAX_PATH ];
    string application_path = string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
    // remove the application name from the path
    for (unsigned int i = (unsigned int)application_path.length(); application_path[i] != '\\' && i>0; i--) 
        application_path.resize(i);
    // store search path order
    _searchPath.push_back( application_path + string("plugins\\") );
    _searchPath.push_back( application_path );
    _searchPath.push_back( string(".\\") );
    _searchPath.push_back( string(".\\plugins\\") );
    }
/*
 _                    _ ____  _             _           _____                    _____ _ _
| |    ___   __ _  __| |  _ \| |_   _  __ _(_)_ __  ___|  ___| __ ___  _ __ ___ |  ___(_) | ___
| |   / _ \ / _` |/ _` | |_) | | | | |/ _` | | '_ \/ __| |_ | '__/ _ \| '_ ` _ \| |_  | | |/ _ \
| |__| (_) | (_| | (_| |  __/| | |_| | (_| | | | | \__ \  _|| | | (_) | | | | | |  _| | | |  __/
|_____\___/ \__,_|\__,_|_|   |_|\__,_|\__, |_|_| |_|___/_|  |_|  \___/|_| |_| |_|_|   |_|_|\___|
                                      |___/
*/
void CPluginManager::LoadPluginsFromFile(std::string fname) {
    // parse the file and scan the plugin list
    CWrapperLua *WL = new CWrapperLua();
    WL->SetErrorLevel(CWrapperLua::CatchAndContinue);
    if (!WL->ParseFile(fname)) {
        PushError("LoadPluginsFromFile", string("[LoadPluginsFromFile] Error parsing the file \"") + fname + string("\""));
        return;
        }
    // get the number of elements
    WL->DoString("plugin_list_size = table.getn(plugin_list);");
    int n_plugins;
    WL->GetInt("plugin_list_size", n_plugins);
    vector<string> plugin_names;
    // get the plugin names
    for (int i = 0; i < n_plugins; i++) {
        stringstream lua_command;
        string plugin_name;
        string cfg_file;
        // get the plugin name
        lua_command.str("");
        lua_command << "plugin_list[" << i+1 << "]";
        WL->GetString(lua_command.str(), plugin_name);
        plugin_names.push_back(plugin_name);
        }

    delete WL;
    // load plugins
    for (unsigned int i=0; i<plugin_names.size(); i++)
        LoadPlugin(plugin_names[i]);
    }

/*
 _                    _ ____  _             _
| |    ___   __ _  __| |  _ \| |_   _  __ _(_)_ __
| |   / _ \ / _` |/ _` | |_) | | | | |/ _` | | '_ \
| |__| (_) | (_| | (_| |  __/| | |_| | (_| | | | | |
|_____\___/ \__,_|\__,_|_|   |_|\__,_|\__, |_|_| |_|
                                      |___/
*/
bool CPluginManager::LoadPlugin(std::string plugin_file_name, bool onlytest) {
    // plugin name has to be case-insensitive
    std::transform(plugin_file_name.begin(), plugin_file_name.end(), plugin_file_name.begin(), ::toupper);
    // avoid to load two plugins with the same name at runtime
    if (_loadedPlugins.find(plugin_file_name) != _loadedPlugins.end()) return false;
    // search the plugin following the search path order
    _searchPathListType::iterator it = _searchPath.begin();
    // start search the plugin
    while (it != _searchPath.end()) {
        // try to load the plugin from the actual path in list
        string plugin_module_name = (*it) + plugin_file_name;
#if defined(DEBUG_MODE)
        // in case of DEBUG version the manager will append '_d' to the name of the plugin if not present
        plugin_module_name.append("_d");
#pragma message("CPluginManager: _LOAD_DEBUG_PLUGINS")
#endif
        HMODULE module = LoadLibrary(plugin_module_name.c_str());
        if (module != NULL) {
            // RELEASE/DEBUG mode between manager and plugin must match
            ISDEBUG_FCN_TYPE is_debug = (ISDEBUG_FCN_TYPE)GetProcAddress(module, "is_debug");
            if (is_debug != NULL) {
#ifdef DEBUG_MODE
                if (is_debug() == FALSE) { 
#else
                if (is_debug() == TRUE) { 
#endif
                    FreeLibrary(module); 
                    } 
            } else {
                // is_debug not present, module is not a valid plugin
                FreeLibrary(module);
                it++;
                continue;
                }
            // a module with the correct name is loaded, check if it is a valid plugin module
            CREATE_FCN_TYPE framework_plugin_create = (CREATE_FCN_TYPE)GetProcAddress(module, "framework_plugin_create");
            if (framework_plugin_create != NULL) {
                // loading function is founded, try to instantiate the plugin class
                CPlugin *plugin_instance = framework_plugin_create();
                if (plugin_instance != NULL) {
                    // plugin created
                    std::string s = plugin_instance->Plugin_GetClass();
                    if (s.length()==0 || s.compare(_managedClass) == 0) {
                        // class matched
                        if (onlytest) {
                            // plugin only tested, unload it
                            DESTROY_FCN_TYPE destroy_plugin = (DESTROY_FCN_TYPE)GetProcAddress(module, "framework_plugin_destroy");
                            destroy_plugin();
                            FreeLibrary(module);
                            return true;
                        } else {
                            // store the path from which the plugin was loaded
                            plugin_instance->Plugin_StoreLoadingPath(plugin_module_name);
                            // plugin succesfully loaded and created, store its references
                            string plugin_name_ucase = plugin_instance->Plugin_GetName();
                            std::transform(plugin_name_ucase.begin(), plugin_name_ucase.end(), plugin_name_ucase.begin(), ::toupper);
                            _loadedPlugins[plugin_name_ucase] = plugin_instance;
                            _moduleHandle[plugin_name_ucase]  = module;
                            return true;
                        }
                    } else {
                        // class unmatched, failed to load plugin
                        DESTROY_FCN_TYPE destroy_plugin = (DESTROY_FCN_TYPE)GetProcAddress(module, "framework_plugin_destroy");
                        destroy_plugin();
                        FreeLibrary(module);
                    }
                } else {
                     // failed to load the plugin class, close the module and go ahead to the next paths
                    framework_plugin_create = NULL;
                    FreeLibrary(module);
                }
            } else
                // creation function not present, invalid library.
                FreeLibrary(module);
        } else {
			char msg[256];
			DWORD err = ::GetLastError();
			DWORD res = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, msg, 256, NULL );

			int i = 0;
			i++;

		}
        it++;
        }
    return false;
    }

/*
 _                    _    _             _ _       _     _      ____  _             _
| |    ___   __ _  __| |  / \__   ____ _(_) | __ _| |__ | | ___|  _ \| |_   _  __ _(_)_ __  ___
| |   / _ \ / _` |/ _` | / _ \ \ / / _` | | |/ _` | '_ \| |/ _ \ |_) | | | | |/ _` | | '_ \/ __|
| |__| (_) | (_| | (_| |/ ___ \ V / (_| | | | (_| | |_) | |  __/  __/| | |_| | (_| | | | | \__ \
|_____\___/ \__,_|\__,_/_/   \_\_/ \__,_|_|_|\__,_|_.__/|_|\___|_|   |_|\__,_|\__, |_|_| |_|___/
                                                                              |___/
*/
void CPluginManager::LoadAvailablePlugins() {
    // get the list of available plugins
    std::vector<std::string> list;
    list = ScanForPluginNames();
    // load all plugins in list without any config file
    for (unsigned int i=0; i<list.size(); i++)
        LoadPlugin(list[i],false);
    }

/*
 _   _       _                 _ ____  _             _
| | | |_ __ | | ___   __ _  __| |  _ \| |_   _  __ _(_)_ __
| | | | '_ \| |/ _ \ / _` |/ _` | |_) | | | | |/ _` | | '_ \
| |_| | | | | | (_) | (_| | (_| |  __/| | |_| | (_| | | | | |
 \___/|_| |_|_|\___/ \__,_|\__,_|_|   |_|\__,_|\__, |_|_| |_|
                                               |___/
*/
void CPluginManager::UnloadPlugin(std::string plugin_name) {
    // plugin name has to be case-insensitive
    std::transform(plugin_name.begin(), plugin_name.end(), plugin_name.begin(), ::toupper);
    // search the plugin and delete it if present
    _nameClassPluginType::iterator it = _loadedPlugins.find(plugin_name);
    if (it != _loadedPlugins.end()) {
        // shutdown and deallocate the plugin instance
        it->second->Plugin_Shutdown();
        // destroy the class
        DESTROY_FCN_TYPE destroy_plugin = (DESTROY_FCN_TYPE)GetProcAddress(_moduleHandle[plugin_name], "framework_plugin_destroy");
        destroy_plugin();
        _loadedPlugins.erase(it);
        // unload the plugin module
        FreeLibrary(_moduleHandle[plugin_name]);
        _moduleHandle.erase(plugin_name);
        }
    }

/*
 _   _       _                 _    _    _ _ ____  _             _
| | | |_ __ | | ___   __ _  __| |  / \  | | |  _ \| |_   _  __ _(_)_ __  ___
| | | | '_ \| |/ _ \ / _` |/ _` | / _ \ | | | |_) | | | | |/ _` | | '_ \/ __|
| |_| | | | | | (_) | (_| | (_| |/ ___ \| | |  __/| | |_| | (_| | | | | \__ \
 \___/|_| |_|_|\___/ \__,_|\__,_/_/   \_\_|_|_|   |_|\__,_|\__, |_|_| |_|___/
                                                           |___/
*/
void CPluginManager::UnloadAllPlugins() {
    // destroy all elements and empty queue
    _nameClassPluginType::iterator it = _loadedPlugins.begin();
    while (_loadedPlugins.size() != 0) {
        // unload the plugin
        UnloadPlugin(it->first);
        // get again the first plugin in the list
        it = _loadedPlugins.begin();
        }
    }


/*
 ___       _ _   _       _ _            _    _ _ ____  _             _
|_ _|_ __ (_) |_(_) __ _| (_)_______   / \  | | |  _ \| |_   _  __ _(_)_ __  ___
 | || '_ \| | __| |/ _` | | |_  / _ \ / _ \ | | | |_) | | | | |/ _` | | '_ \/ __|
 | || | | | | |_| | (_| | | |/ /  __// ___ \| | |  __/| | |_| | (_| | | | | \__ \
|___|_| |_|_|\__|_|\__,_|_|_/___\___/_/   \_\_|_|_|   |_|\__,_|\__, |_|_| |_|___/
                                                               |___/
*/
void CPluginManager::InitializeAllPlugins(std::string config_file) {
    _nameClassPluginType::iterator it = _loadedPlugins.begin();
    while (it != _loadedPlugins.end()) {
        it->second->Plugin_Initialize(config_file);
        it++;
        }
    }

/*
 ____  _           _      _                        _    _ _ ____  _             _
/ ___|| |__  _   _| |_ __| | _____      ___ __    / \  | | |  _ \| |_   _  __ _(_)_ __  ___
\___ \| '_ \| | | | __/ _` |/ _ \ \ /\ / / '_ \  / _ \ | | | |_) | | | | |/ _` | | '_ \/ __|
 ___) | | | | |_| | || (_| | (_) \ V  V /| | | |/ ___ \| | |  __/| | |_| | (_| | | | | \__ \
|____/|_| |_|\__,_|\__\__,_|\___/ \_/\_/ |_| |_/_/   \_\_|_|_|   |_|\__,_|\__, |_|_| |_|___/
                                                                          |___/
*/
void CPluginManager::ShutdownAllPlugins() {
    _nameClassPluginType::iterator it = _loadedPlugins.begin();
    while (it != _loadedPlugins.end()) {
        it->second->Plugin_Shutdown();
        it++;
        }
    }


/*
  ____      _   ____  _             _
 / ___| ___| |_|  _ \| |_   _  __ _(_)_ __
| |  _ / _ \ __| |_) | | | | |/ _` | | '_ \
| |_| |  __/ |_|  __/| | |_| | (_| | | | | |
 \____|\___|\__|_|   |_|\__,_|\__, |_|_| |_|
                              |___/
*/
CPlugin *CPluginManager::GetPlugin(std::string plugin_name) {
    // plugin name has to be case-insensitive
    std::transform(plugin_name.begin(), plugin_name.end(), plugin_name.begin(), ::toupper);
    if (_loadedPlugins.find(plugin_name) != _loadedPlugins.end()) 
        return _loadedPlugins[plugin_name];
    else
        return NULL;
    }
/*
  ____      _   _                    _          _ ____  _             _           _   _
 / ___| ___| |_| |    ___   __ _  __| | ___  __| |  _ \| |_   _  __ _(_)_ __  ___| \ | | __ _ _ __ ___   ___
| |  _ / _ \ __| |   / _ \ / _` |/ _` |/ _ \/ _` | |_) | | | | |/ _` | | '_ \/ __|  \| |/ _` | '_ ` _ \ / _ \
| |_| |  __/ |_| |__| (_) | (_| | (_| |  __/ (_| |  __/| | |_| | (_| | | | | \__ \ |\  | (_| | | | | | |  __/
 \____|\___|\__|_____\___/ \__,_|\__,_|\___|\__,_|_|   |_|\__,_|\__, |_|_| |_|___/_| \_|\__,_|_| |_| |_|\___|
                                                                |___/
*/
std::vector<std::string> CPluginManager::GetLoadedPluginsName() {
    std::vector<std::string> list;
    // cycle through plugin map and store the names
    list.resize(_loadedPlugins.size());
    _nameClassPluginType::iterator it = _loadedPlugins.begin();
    unsigned int i=0;
    while (it != _loadedPlugins.end()) {
        list[i++] = it->first;
        it++;
        }
    return list;
    }