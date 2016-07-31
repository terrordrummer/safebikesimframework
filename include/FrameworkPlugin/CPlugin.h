#pragma once

#include "FrameworkCore/CError.h"
#include "FrameworkPlugin/common_defines.h"

/** \brief Plugin classes namespace.
 *
 * The plugin mechanism allows to create a main program which can be extended with some
 * features that are splitted in several external libraries that are loaded at runtime.\n\n
 * The mechanism that loads these libraries is implemented by the CPluginManager class
 * which is able to search for the plugins and load/unload them. A library that wants to
 * be loaded by the CPluginManager must inherit the CPlugin class and must specialize its 
 * virtual methods.\n
 * Generally, the plugin mechanism requires a plugin manager which searches the
 * plugin files and loads them automatically. Once loaded the plugins instances can be
 * reached by a class pointer.\n
 * See CPluginManager and CPlugin for more specific details.
 */
namespace FrameworkPlugin {
    
    /** \brief Basic plugin class.
     *
     * This is the basic class that has to be inerhited to create a plugin.
     * All the virtual methods must be specialized. When the plugin is loaded
     * the method Plugin_Initialize() is called once with its configuration filename as
     * parameter. When the plugin is unloaded by the plugin manager the method
     * Plugin_Shutdown() is called.
     */
    class framework_plugin_class CPlugin {
    protected:
        std::string _loadingPath;
    public:
        /** \brief Plugin initialization.                           */
        virtual bool        Plugin_Initialize(std::string config_file);
        /** \brief Plugin shutdown.                                 */
        virtual void        Plugin_Shutdown();
        /** \brief Get the plugin name.                             */
        virtual std::string Plugin_GetName();
        /** \brief Get the plugin version.                          */
        virtual std::string Plugin_GetVersion();
        /** \brief Get the plugin class.                            */
        virtual std::string Plugin_GetClass();

        /** \brief Get the path from where the plugin was loaded.   */
        std::string Plugin_GetLoadingPath();
        /** \brief Store the path from where the plugin was loaded. */
        void Plugin_StoreLoadingPath(std::string path);
        };


#ifdef DEBUG_MODE
#define FRAMEWORK_EXPORT_PLUGIN(class_name) static class_name *plugin_class_pointer_local;                      \
                                            extern "C" __declspec(dllexport)                                    \
                                            FrameworkPlugin::CPlugin *framework_plugin_create() {               \
                                                plugin_class_pointer_local = new class_name();                  \
                                                return plugin_class_pointer_local;                              \
                                            }                                                                   \
                                            extern "C" __declspec(dllexport)                                    \
                                            bool is_debug() {                                                   \
                                                return true;                                                    \
                                            }                                                                   \
                                            extern "C" __declspec(dllexport)                                    \
                                            void framework_plugin_destroy() {                                   \
                                                delete plugin_class_pointer_local;                              \
                                            }
#else
#define FRAMEWORK_EXPORT_PLUGIN(class_name) static class_name* plugin_class_pointer_local;                            \
                                            extern "C" __declspec(dllexport)                                    \
                                            FrameworkPlugin::CPlugin *framework_plugin_create() {               \
                                                plugin_class_pointer_local = new class_name();                  \
                                                return plugin_class_pointer_local;                              \
                                            }                                                                   \
                                            extern "C" __declspec(dllexport)                                    \
                                            bool is_debug() {                                                   \
                                                return false;                                                   \
                                            }                                                                   \
                                            extern "C" __declspec(dllexport)                                    \
                                            void framework_plugin_destroy() {                                   \
                                                delete plugin_class_pointer_local;                              \
                                            }
#endif

}