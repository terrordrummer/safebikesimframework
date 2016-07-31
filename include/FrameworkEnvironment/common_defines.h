#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_ENVIRONMENT_CLASS_DEFINE_
        #define _FRAMEWORK_ENVIRONMENT_CLASS_DEFINE_
        #ifdef _FRAMEWORK_ENVIRONMENT_DEVELOPER
            #define framework_environment_class __declspec(dllexport)
            #pragma message("FrameworkEnvironment DLLEXPORT")
        #else
            #define framework_environment_class __declspec(dllimport)
            #pragma message("FrameworkEnvironment DLLIMPORT")
        #endif
    #endif
#else
    #define framework_environment_class
    #pragma message("FrameworkEnvironment STATIC")
#endif

#pragma warning(disable : 4251 4275 4800)