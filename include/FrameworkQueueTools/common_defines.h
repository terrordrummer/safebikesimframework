#pragma once

// in case of a DLL project classes has to be declared DLLexport

#ifndef _FRAMEWORK_QUEUE_TOOLS_CLASS_DEFINE_
    #define _FRAMEWORK_QUEUE_TOOLS_CLASS_DEFINE_
    #ifdef _FRAMEWORK_QUEUE_TOOLS_DEVELOPER
        #define framework_queue_tools_class __declspec(dllexport)
        #pragma message("FrameworkQueueTools DLLEXPORT")
    #else
        #define framework_queue_tools_class __declspec(dllimport)
        #pragma message("FrameworkQueueTools DLLIMPORT")
    #endif
#endif


#pragma warning(disable : 4251 4275)