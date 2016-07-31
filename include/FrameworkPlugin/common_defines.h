#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_PLUGIN_CLASS_DEFINE_
        #define _FRAMEWORK_PLUGIN_CLASS_DEFINE_
        #ifdef _FRAMEWORK_PLUGIN_DEVELOPER
            #define framework_plugin_class __declspec(dllexport)
            #pragma message("FrameworkPlugin DLLEXPORT")
        #else
            #define framework_plugin_class __declspec(dllimport)
            #pragma message("FrameworkPlugin DLLIMPORT")
        #endif
    #endif
#else
    #define framework_plugin_class
    #pragma message("FrameworkPlugin STATIC")
#endif

//#pragma warning(disable : 0)