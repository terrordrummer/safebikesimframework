// list f modules whose static data has to be initialized when the DLL is loaded
#include "FrameworkCore/CTrace.h"
#include "FrameworkCore/CSynch.h"
#include "FrameworkCore/CTime.h"

using namespace FrameworkCore;
using namespace std;

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
         // Initialize once for each process.
         // Return FALSE to fail DLL load.
            CSynch::staticInitializer();
            CTime::staticInitializer();
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}