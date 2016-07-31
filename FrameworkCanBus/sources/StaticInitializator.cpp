// include all modules which has to initialize some static data when DLL is loaded
#include "FrameworkCanBus/CANHardwareManager.h"
#include "FrameworkCore/CTrace.h"
#include "FrameworkCore/CSynch.h"

using namespace FrameworkCanBus;
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
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            CANHardwareManager::staticInitializer();
            CSynch::staticInitializer();
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