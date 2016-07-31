#include "FrameworkSimArc/CSimProcess.h"
#include "FrameworkSimArc/CSimControl.h"

// Dvelopment Framework
#include "FrameworkCore/CGenericType.h"

using namespace std;
using namespace FrameworkSimArc;
using namespace FrameworkCore;


////////////////////////////////////////////////////////////////////////////////////////
#define SINGLEPROCESS_AND_A_SUPERVISOR


DWORD WINAPI SimulationProcessThread(
  __in  LPVOID lpParameter
  ) {
      CSimControl C;
      CGenericType processes;

      for (int i=0; i<100; i++) {
          C.GetProcessList(processes);
          cout << processes.Dump() << endl;
          cout << "- - - - - - - - - - - - - -" << endl << endl;
          Sleep(1000);
      }

    return 0;
}

DWORD WINAPI SimulationControlThread(
  __in  LPVOID lpParameter
) {
    CSimProcess P;

    P.SetState(PROCESS_STATE_LOADING);
    Sleep(1300);

    P.SetState(PROCESS_STATE_READY);
    Sleep(1300);

    P.SetState(PROCESS_STATE_RUN);
    Sleep(1300);

    P.SetState(PROCESS_STATE_PAUSE);
    Sleep(1300);

    P.SetState(PROCESS_STATE_KILLED);

    return 0;
}

/*
                 _
 _ __ ___   __ _(_)_ __
| '_ ` _ \ / _` | | '_ \
| | | | | | (_| | | | | |
|_| |_| |_|\__,_|_|_| |_|
*/
 int __cdecl main(int argc, char argv[]) {
    cout << "TEST" << endl;
    DWORD threadID;

    CreateThread(NULL, 0, SimulationProcessThread, NULL, 0, &threadID);
    CreateThread(NULL, 0, SimulationControlThread, NULL, 0, &threadID);

    char endchar;
    cin >> endchar;
    }


 ////////////////////////////////////////////////////////////////////////////////////////