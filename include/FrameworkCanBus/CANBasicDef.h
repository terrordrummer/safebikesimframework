// DLL import/export macro definition.
#ifdef _USRDLL
    #ifndef __DLL_EXPORT_DEFINE_
        #define __DLL_EXPORT_DEFINE_
        #ifdef __CANMANAGER_DEVELOPER
            #define dll_class __declspec(dllexport)
            #pragma message("Can Manager Developer DLLEXPORT")
        #else
            #define dll_class __declspec(dllimport)
            #pragma message("Can Manager User DLLIMPORT")
        #endif
    #endif
#else
    #define dll_class
#endif
// disable the following warnings
#pragma warning(disable : 4251 4275)