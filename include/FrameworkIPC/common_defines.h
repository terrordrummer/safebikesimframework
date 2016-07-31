#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_IPC_CLASS_DEFINE_
        #define _FRAMEWORK_IPC_CLASS_DEFINE_
        #ifdef _FRAMEWORK_IPC_DEVELOPER
            #define framework_ipc_class __declspec(dllexport)
            #pragma message("FrameworkIPC DLLEXPORT")
        #else
            #define framework_ipc_class __declspec(dllimport)
            #pragma message("FrameworkIPC DLLIMPORT")
        #endif
    #endif
#else
    #define framework_ipc_class
    #pragma message("FrameworkIPC STATIC")
#endif

#pragma warning(disable : 4251 4275)