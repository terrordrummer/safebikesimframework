#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_LUA_CLASS_DEFINE_
        #define _FRAMEWORK_LUA_CLASS_DEFINE_
        #ifdef _FRAMEWORK_LUA_DEVELOPER
            #define framework_lua_class __declspec(dllexport)
            #pragma message("FrameworkLUA DLLEXPORT")
        #else
            #define framework_lua_class __declspec(dllimport)
            #pragma message("FrameworkLUA DLLIMPORT")
        #endif
    #endif
#else
    #define framework_lua_class
    #pragma message("FrameworkLUA STATIC")
#endif

#pragma warning(disable : 4251 4275 4800)