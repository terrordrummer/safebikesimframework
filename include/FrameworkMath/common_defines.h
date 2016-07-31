#pragma once

// in case of a DLL project classes has to be declared DLLexport
#if ( defined(_USRDLL) || !defined(_USE_STATIC_FRAMEWORK) )
    #ifndef _FRAMEWORK_MATH_CLASS_DEFINE_
        #define _FRAMEWORK_MATH_CLASS_DEFINE_
        #ifdef _FRAMEWORK_MATH_DEVELOPER
            #define framework_math_class __declspec(dllexport)
            #pragma message("FrameworkMath DLLEXPORT")
        #else
            #define framework_math_class __declspec(dllimport)
            #pragma message("FrameworkMath DLLIMPORT")
        #endif
    #endif
#else
    #define framework_math_class
    #pragma message("FrameworkMath STATIC")
#endif

#pragma warning(disable : 4251 4275)