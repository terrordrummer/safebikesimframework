#pragma once

// in case of a DLL project classes has to be declared DLLexport
#ifndef _FRAMEWORK_SIMARC_CLASS_DEFINE_
    #define _FRAMEWORK_SIMARC_CLASS_DEFINE_
    #ifdef _FRAMEWORK_SIMARC_DEVELOPER
        #define framework_simarc_class __declspec(dllexport)
        #pragma message("FrameworkSimArc DLLEXPORT")
    #else
        #define framework_simarc_class __declspec(dllimport)
        #pragma message("FrameworkSimArc DLLIMPORT")
    #endif
#endif


#pragma warning(disable : 4482)    // utilizzata estensione non standard x enumerativi