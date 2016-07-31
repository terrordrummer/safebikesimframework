#pragma once

#include <map>
#include <deque>
#include <vector>
#include <string>
#include <windows.h>
#include "FrameworkPlugin/CPlugin.h"
#include "FrameworkCore/CError.h"


namespace FrameworkPlugin {
    /** \brief Plugin manager class.
     *
     * This class implements the plugin manager that loads/unloads the plugins and let the user to get the 
     * plugin class pointers. \n
     * SetPluginClass() method defines the plugin class name compatible with the manager, it will
     * load only the plugins with the same class name. As a consequence, several plugin managers
     * can be used to load different class plugins that can be founded in the same path. \n
     * The searching path is manager by the AppendSearchPath(), InsertSearchPath(), RemoveSearchPath()
     * and ClearSearchPath() mehods.\n
     * The list of available plugins (founded in the search path) can be retrived calling 
     * ScanForPluginPaths() to get the path to the library file or ScanForPluginNames() 
     * to get the list of the available plugin names.\n
     * To load the plugins use one of the following:
     * \li LoadPluginsFromFile() to read the list of plugins from a text file
     * \li LoadPlugin() to load or to test is a plugin with the given name is loadable
     * \li LoadAvailablePlugins() to load all plugins founded in the searching path
     * \li UnloadPlugin() to unload a plugin given its name
     * \li UnloadAllPlugins() to unload all loaded plugins
     *
     * InitializeAllPlugins() method initializes all plugins with the same text file.\n
     * ShutdownAllPlugins() shutsdown all plugins.\n
     * \n
     * GetPlugin() reurns the pointer to the class instance of the loaded plugin.\n
     * \n
     * GetLoadedPluginsName() returns the list of loaded plugins.
     */
    class framework_plugin_class CPluginManager : virtual private FrameworkCore::CError {
    protected:
        /** \brief Container type for the loaded plugin class.
         *
         * Association is between plugin name and plugin class.
         */
        typedef std::map<std::string, CPlugin *>    _nameClassPluginType;
        /** \brief Container type for the valid plugin search path list.        */
        typedef std::deque<std::string>             _searchPathListType;
        /** \brief Module handle container type.                                */
        typedef std::map<std::string, HMODULE>      _moduleHandleType;
        /** \brief Container of the loaded plugin classes.                      */
        _nameClassPluginType _loadedPlugins;
        /** \brief Search path list.                                            */
        _searchPathListType _searchPath;
        /** \brief Plugins module handle container.                             */
        _moduleHandleType _moduleHandle;
        /** \brief Active managed class.                                        */
        std::string _managedClass;

        /** \brief Setup of the search path for the plugins.                    */
        void InitializeSearchPath();
    public:

        /** \brief Initialize and loads plugins from a list file.
         *  \param fname Name of the LUA file containing the list of plugins to be loaded.
         *
         * If a file name is given then the manager loads immediatly the plugins
         * defined inside the file, otherwise the manager is only initialized.
         */
        CPluginManager(std::string fname = "");
        /** \brief Default destructor.                                          */
        ~CPluginManager();

        /** \brief Sets the plugin manager class.    
         *  \param plugins_class Plugin class name.
         *
         * Any plugin stores the name of its class, this parameter stores the 
         * plugin's class name the manager is allowed to load. Any plugin
         * with different class name will be discarded by the manager.
         * By default, class name is empty i.e. all classes will be managed.
         */
        void SetPluginClass(std::string plugins_class);

        /** \brief Adds a path to the end of the search path list.
         *  \param path Path to be appended at the end of the actual list.
         */
        void AppendSearchPath(std::string path);
        /** \brief Adds a path at the beginning of the search path list.
         *  \param path Path to be inserted at the beginning of the actual list.
         */
        void InsertSearchPath(std::string path);
        /** \brief Removes a search path.
         *  \param path Path to be removed from the search list.
         */
        void RemoveSearchPath(std::string path);
        /** \brief Remove all paths in the searhc path list.                    */
        void ClearSearchPath();
        /** \brief Get the search path list.
         *  \return Path list as a vector of strings.
         */
        std::vector<std::string> GetSearchPathList();

        /** \brief Scan the plugin path and returns the paths to the available
         *         plugins.
         *  \return List of libraries file containing loadable plugins.
         */
        std::vector<std::string> ScanForPluginPaths();
        /** \brief Scan the plugin path and returns the names of the available
         *         plugins.
         *  \return List of loadable plugin names founded in the search path.
         */
        std::vector<std::string> ScanForPluginNames();

        /** \brief Loads plugins reading the name list from a text file.
         *
         * The file format must be LUA. A table named <b>plugins</b> must
         * be present with the following format:
         * \code
         *   plugins = {
         *       "<PluginName 1>", "<cfg file name for plugin 1>"
         *      ,"<PluginName 2>", "<cfg file name for plugin 2>"
         *      ,"<PluginName 3>", "<cfg file name for plugin 3>"
         *      ,"<PluginName 4>", "<cfg file name for plugin 4>"
         *   }
         *
         * \endcode
         * The manager will read the name of each plugin and the name of its configuration file
         * and will try to load it.
         */
        void LoadPluginsFromFile(std::string fname);
        /** \brief Load a plugin, name is not case-sensitive.
         *  \param plugin_name The name of the plugin (non case-sensitive)
         *  \param onlytest <b>true</b> to load the plugin, <b>false</b> to test only the plugin
         *  \return <b>true</b> if plugin is loaded or loadable, <b>false</b> otherwise.
         *
         * The manager will search for a plugin with the given name in the search path,
         * if founded then
         * \li if onlytest is <b>true</b> then the plugin is tested to be loadable
         * \li if onlytest is <b>false</b> then the plugin is loaded
         *
         * If loaded, the plugin's class instance is retrived using GetPlugin().
         */
        bool LoadPlugin(std::string plugin_name, bool onlytest = false);
        /** \brief Loads all available plugins which will match the actual 
         *         class name.                                                  */
        void LoadAvailablePlugins();
        /** \brief Unload a plugin, name is not case-sensitive.
         *  \param plugin_name Non case-sensitive name of the plugin that has to be unloaded.
         */
        void UnloadPlugin(std::string plugin_name);
        /** \brief Unload all plugins.                                          */
        void UnloadAllPlugins();

        /** \brief Initialize all plugins with the same config file name.
         *  \param config_file Name of the file that has to be used as configuration file 
         *         for all plugins.
         */
        void InitializeAllPlugins(std::string config_file);
        /** \brief Shutdown all loaded plugins.                                 */
        void ShutdownAllPlugins();

        /** \brief Get a plugin pointer given the plugin name.
         *  \param plugin_name Non case-sensitive name of the plugin to be retrived.
         *  \return Pointer to the plugin class instance.
         *
         */
        CPlugin *GetPlugin(std::string plugin_name);

        /** \brief Get the list of loaded plugins.
         *  \return Name list of the loaded plugins as a vector of string.
         */
        std::vector<std::string> GetLoadedPluginsName();

        };

    }