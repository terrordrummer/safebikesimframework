#include "FrameworkIPC/CIPC.h"
#include "FrameworkCore/CGenericType.h"
#include "FrameworkCore/CTime.h"

#include "math.h"

using namespace std;
using namespace FrameworkIPC;
using namespace FrameworkCore;

//#define BASIC_TEST
//#define LATENCY_TEST
#define CHANNEL_SYNCH_TEST


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef BASIC_TEST

bool ThreadTerminated = false;

DWORD WINAPI SenderThread(
  __in  LPVOID lpParameter
  ) {
      CIPC c;
    // SENDER
    if (!c.Join())
        cout << c.PopErrorStack();
    else {
        CGenericType T;
        for (unsigned int i=0; i<500; i++) {
            T = i;
            c.WriteDataToChannel("ciao",T);
            if (c.HasError()) {
                cout << c.PopErrorStack() << endl;
            }
            else
                cout << "written!" << endl;
            Sleep(1000);
            }
        }
    ThreadTerminated = true;
    return 0;
}

DWORD WINAPI ReceiverThread(
  __in  LPVOID lpParameter
) {
      CIPC c;
    // RECEIVER
    if (!c.Join()) {
        cout << c.PopErrorStack();
        } 
    else {
        CGenericType T;
        for (unsigned int i=0; i<500; i++) {
            c.ReadDataFromChannel("ciao" ,T);
            if (!c.HasError()) {
                cout  << (T).toString() << endl;
                } 
            else
                cout << c.PopErrorStack() << endl;
            }
        }
    ThreadTerminated = true;
    return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LATENCY_TEST

bool ThreadTerminated = false;
bool Sender = true;

DWORD WINAPI ThreadProc(
  __in  LPVOID lpParameter
  ) {
      CIPC c;
    // SENDER
    if (!c.Join())
        cout << c.PopErrorStack();
    else {
        CGenericType data;
        double send_mean    = 0.0;
        double receive_mean = 0.0;
        double total_mean   = 0.0;
        CTime Smean, Rmean, Tmean;
        for (unsigned int i=0; i<50000; i++) {
            Tmean.Tic();
            data = i;
            Smean.Tic();
            if (Sender)
                c.WriteDataToChannel("ch1",data);
            else
                c.WriteDataToChannel("ch2",data);
            send_mean = 0.9*send_mean + 0.1*Smean.Toc();
            Rmean.Tic();
            if (Sender)
                c.ReadDataFromChannel("ch2",data);
            else
                c.ReadDataFromChannel("ch1",data);
            receive_mean = 0.9*receive_mean + 0.1*Rmean.Toc();
            total_mean = 0.9*total_mean + 0.1*Tmean.Toc();
            if (i % 1000 == 0) {
                cout << endl << endl << endl << "transmit : " << send_mean << " s\n";
                cout                         << "receive  : " << receive_mean << " s\n";
                cout                         << "total    : " << total_mean << " s\n";
                }
            }
        }
    ThreadTerminated = true;
    return 0;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef CHANNEL_SYNCH_TEST

bool ThreadTerminated = false;
bool Sender = true;

DWORD WINAPI ThreadProc(
  __in  LPVOID lpParameter
  ) {
      CIPC c;
    // SENDER
    if (!c.Join())
        cout << c.PopErrorStack();
    else {
        if (Sender) {
            CGenericType data;
            for (unsigned int i=0; i<500; i++) {
                data % "value" =  i;
                data % "random" = rand() % 100;
                
                // sender
                c.WriteDataToChannel("ch1", data);
                CGenericType aux;
                aux % "FIRST" = "CIAO";
                aux % "SECOND" = 2*i;
                c.WriteDataToChannel("ch2", aux);
                cout << "written: " << data.Dump() << endl;
                Sleep(1000 + (rand() % 1000));
                }
            c.Leave();
            }
        else {
            // receiver
            CGenericType data;
            c.EnableDataSynchronization("ch1");
            c.EnableDataSynchronization("ch2");
            for (unsigned int i=0; i<500; i++) {
                // get data
                c.GetChannelData("ch1", data);
                cout << "data: " << data.Dump() << endl;
                if (rand() % 10 == 1) {
                    // update also ch2 data
                    CGenericType aux;
                    c.GetChannelData("ch2", aux);
                    cout << "aux: " << aux.Dump() << endl;
                    }
                Sleep(100);

                }
            c.Leave();

            }
        }
    ThreadTerminated = true;
    return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

#ifdef BASIC_TEST
    CIPC c;
    
    if (argc > 1)
        CreateThread(NULL, 0, SenderThread, NULL, 0, &threadID);
    else
        CreateThread(NULL, 0, ReceiverThread, NULL, 0, &threadID);

#endif

#ifdef LATENCY_TEST
    if (argc > 1)
        Sender = true;    
    else
        Sender = false;
    // launch
    CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadID);
#endif


#ifdef CHANNEL_SYNCH_TEST
    if (argc > 1)
        Sender = true;    
    else
        Sender = false;
    // launch
    CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadID);
#endif

    while (!ThreadTerminated) Sleep(100);

    cout << "DONE";
    char endchar;
    cin >> endchar;

 }