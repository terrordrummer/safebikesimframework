#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_CORE_CLASS_DEFINE_
        #define _FRAMEWORK_CORE_CLASS_DEFINE_
        #ifdef _FRAMEWORK_CORE_DEVELOPER
            #define framework_core_class __declspec(dllexport)
            #pragma message("FrameworkCore DLLEXPORT")
        #else
            #define framework_core_class __declspec(dllimport)
            #pragma message("FrameworkCore DLLIMPORT")
        #endif
    #endif
#else
    #define framework_core_class
    #pragma message("FrameworkCore STATIC")
#endif

#pragma warning(disable : 4251 4275)