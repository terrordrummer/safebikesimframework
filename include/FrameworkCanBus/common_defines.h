#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_CANBUS_CLASS_DEFINE_
        #define _FRAMEWORK_CANBUS_CLASS_DEFINE_
        #ifdef _FRAMEWORK_CANBUS_DEVELOPER
            #define framework_canbus_class __declspec(dllexport)
            #pragma message("FrameworkCanBus DLLEXPORT")
        #else
            #define framework_canbus_class __declspec(dllimport)
            #pragma message("FrameworkCanBus DLLIMPORT")
        #endif
    #endif
#else
    #define framework_canbus_class
    #pragma message("FrameworkCanBus STATIC")
#endif

//#pragma warning(disable : 0)