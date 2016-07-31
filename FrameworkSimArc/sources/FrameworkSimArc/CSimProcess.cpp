#include "FrameworkSimArc/CSimProcess.h"
#include "FrameworkLUA/CWrapperLua.h"
#include "Psapi.h"

// namespaces
using namespace FrameworkSimArc;
using namespace FrameworkCore;
using namespace FrameworkLUA;
using namespace std;


/*
 ____       _   ____                             ____       _            _ _
/ ___|  ___| |_|  _ \ _ __ ___   ___ ___ ___ ___|  _ \ _ __(_) ___  _ __(_) |_ _   _
\___ \ / _ \ __| |_) | '__/ _ \ / __/ _ Y __/ __| |_) | '__| |/ _ \| '__| | __| | | |
 ___) |  __/ |_|  __/| | | (_) | (_|  __|__ \__ \  __/| |  | | (_) | |  | | |_| |_| |
|____/ \___|\__|_|   |_|  \___/ \___\___|___/___/_|   |_|  |_|\___/|_|  |_|\__|\__, |
                                                                               |___/
*/
void SetProcessPriority(std::string priority) {
    // only valid values are applied
    if      (priority == "REALTIME")
        SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
    else if (priority == "HIGH")
        SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
    else if (priority == "NORMAL")
        SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
    else if (priority == "LOW")
        SetPriorityClass(GetCurrentProcess(),IDLE_PRIORITY_CLASS);
}

/*
  ____ ____  _           ____
 / ___/ ___|(_)_ __ ___ |  _ \ _ __ ___   ___ ___ ___ ___
| |   \___ \| | '_ ` _ \| |_) | '__/ _ \ / __/ _ Y __/ __|
| |___ ___) | | | | | | |  __/| | | (_) | (_|  __|__ \__ \
 \____|____/|_|_| |_| |_|_|   |_|  \___/ \___\___|___/___/
*/
CSimProcess::CSimProcess() : _pulseFrequency(0) {
    SetErrorClassPrefix("CSimProcess");

    // join the IPC group
    _ipc.Join();
    // Synchronize to process control channel
    _ipc.EnableDataSynchronization(PROCESS_CONTROL_CHANNEL_NAME);

    // --------------- set the default process data ---------------
    // store the process file name
    char aux[256];
    GetModuleFileName(NULL, aux, 256);
    _processData % "file_name" = aux;
    // set the process name as the same of the executable without the extension
    if (!_processData.hasField("name")) {
        GetModuleBaseName(GetCurrentProcess(), NULL, aux, 256);
        _processData % "name" = aux;
    }
    // set the configuration file name
    string CFGfileName = (_processData % "name").toString();
    int pos = (int)CFGfileName.find_last_of(".");
    string fname;
    if (pos != string::npos)
        fname = CFGfileName.substr(0, pos);
    else
        fname = CFGfileName;
#ifdef DEBUG_MODE
    // in debug mode check if there is the suffix _d in the file, in this case remove it from the config file name
    if (fname.substr(fname.length()-2, 2) == "_d")
        fname = fname.substr(0, fname.length()-2);
#endif
    _processData % "configFileName" = fname + string(".cfg");
    // process state is requested to be STOP
    _processData % "RQstate" = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_STOP);
    // ------------------------------------------------------------

    // ---------------- read process configuration ----------------
    CWrapperLua *WL = new CWrapperLua();
    WL->SetErrorLevel(CWrapperLua::CatchAndShow);
    WL->ParseFile((_processData % "configFileName").toString());
    
    int iVal;
    if (!WL->GetInt("frequency", iVal))
        iVal = 100;
    _processData % "synch_frequency" = iVal;

    // read and set the process priority
    string priority;
    if (!WL->GetString("processPriority", priority))
        priority = "NORMAL";
    SetProcessPriority(priority);
    _processData % "priority" = priority;

    // read and set the process affinity
    int affinity;
    if (!WL->GetInt("affinity", affinity))
        affinity = 0xFF;
    DWORD procMask = affinity;
    SetProcessAffinityMask(GetCurrentProcess() ,procMask);
    _processData % "affinity" = affinity;

    delete WL;
    // -------------------------------------------------------------

    // set the initial process state
    _processState = e_ProcessState::PROCESS_STATE_LOADED;

    // reset time counter
    _time.Tic();

    // set trace file name
    _trace = new CTrace((_processData % "name").toString() + string(".log"));

    _processDelegate = NULL;
    }

/*
  ____ ____  _           ____
 / ___/ ___|(_)_ __ ___ |  _ \ _ __ ___   ___ ___ ___ ___
| |   \___ \| | '_ ` _ \| |_) | '__/ _ \ / __/ _ Y __/ __|
| |___ ___) | | | | | | |  __/| | | (_) | (_|  __|__ \__ \
 \____|____/|_|_| |_| |_|_|   |_|  \___/ \___\___|___/___/
 */
CSimProcess::CSimProcess(std::string processName) {
    // assign the process name
    _processData % "name" = processName;
    // default constructor
    CSimProcess();
    }

/*
 /\/|___ ____  _           ____
|/\/ ___/ ___|(_)_ __ ___ |  _ \ _ __ ___   ___ ___ ___ ___
  | |   \___ \| | '_ ` _ \| |_) | '__/ _ \ / __/ _ Y __/ __|
  | |___ ___) | | | | | | |  __/| | | (_) | (_|  __|__ \__ \
   \____|____/|_|_| |_| |_|_|   |_|  \___/ \___\___|___/___/
*/
CSimProcess::~CSimProcess() {
    // terminate the watchdog thread
    ThreadTerminationRequest("WatchDog");
    ThreadTerminationRequest("SIM");

    // stop synchronizing data and leave IPC group
    _ipc.DisableAllDataSynchronization();
    _ipc.Leave();

    delete _trace;

    }

/*
 ____       _   ____  _        _
/ ___|  ___| |_/ ___|| |_ __ _| |_ ___
\___ \ / _ \ __\___ \| __/ _` | __/ _ \
 ___) |  __/ |_ ___) | || (_| | ||  __/
|____/ \___|\__|____/ \__\__,_|\__\___|
*/
void CSimProcess::SetState(e_ProcessState state) {
    _synch.InstanceLock("_processData");
    if (_processState != state) {
        _processState = state;
        _processData % "state" = PROCESS_STATE_TO_CGENERICTYPE(state);
        _ipc.WriteDataToChannel(PROCESS_STATE_CHANNEL_NAME, _processData);
    }
    _synch.InstanceUnlock("_processData");
}

/*
    _            _             ____       _                  _
   / \   ___ ___(_) __ _ _ __ |  _ \  ___| | ___  __ _  __ _| |_ ___
  / _ \ / __/ __| |/ _` | '_ \| | | |/ _ \ |/ _ \/ _` |/ _` | __/ _ \
 / ___ \\__ \__ \ | (_| | | | | |_| |  __/ |  __/ (_| | (_| | ||  __/
/_/   \_\___/___/_|\__, |_| |_|____/ \___|_|\___|\__, |\__,_|\__\___|
                   |___/                         |___/
*/
void CSimProcess::AssignDelegate(CSimProcessDelegate *delegateProcess) {
    _processDelegate = delegateProcess;
    _processDelegate->SetProcessData(_processData);
    _processDelegate->AssignStream(_trace);
}

/*
  ____      _   ____  _        _
 / ___| ___| |_/ ___|| |_ __ _| |_ ___
| |  _ / _ \ __\___ \| __/ _` | __/ _ \
| |_| |  __/ |_ ___) | || (_| | ||  __/
 \____|\___|\__|____/ \__\__,_|\__\___|
*/
e_ProcessState CSimProcess::GetState() {
    return _processState;
}

/*
  ____      _   ____  _        _       ____  _
 / ___| ___| |_/ ___|| |_ __ _| |_ ___/ ___|| |_ _ __
| |  _ / _ \ __\___ \| __/ _` | __/ _ \___ \| __| '__|
| |_| |  __/ |_ ___) | || (_| | ||  __/___) | |_| |
 \____|\___|\__|____/ \__\__,_|\__\___|____/ \__|_|
*/
std::string  CSimProcess::GetStateDesc(e_ProcessState state) {
    switch (state) {
        case PROCESS_STATE_LOADED:
            return "LOADED";
        case PROCESS_STATE_ININTIALIZING:
            return "ININTIALIZING";
        case PROCESS_STATE_STOPPED:
            return "STOPPED";
        case PROCESS_STATE_GOINGTOSTART:
            return "GOINGTOSTART";
        case PROCESS_STATE_RUNNING:
            return "RUNNING";
        case PROCESS_STATE_GOINGTOPAUSE:
            return "GOINGTOPAUSE";
        case PROCESS_STATE_PAUSED:
            return "PAUSED";
        case PROCESS_STATE_GOINGTORESUME:
            return "GOINGTORESUME";
        case PROCESS_STATE_GOINGTOSTOP:
            return "GOINGTOSTOP";
        case PROCESS_STATE_SHUTTINGDOWN:
            return "SHUTTINGDOWN";
        case PROCESS_STATE_ERROR:
            return "ERROR";
        case PROCESS_STATE_KILLED:
            return "KILLED";
        case PROCESS_STATE_NOTRESPONDING:
            return "NOTRESPONDING";
    }
}

/*
 ____        _
|  _ \ _   _| |___  ___
| |_) | | | | / __|/ _ \
|  __/| |_| | \__ \  __/
|_|    \__,_|_|___/\___|
*/
void CSimProcess::Pulse() {
    // frequency computation
    _pulseFrequency = _pulseFrequency*0.9 + 0.1/_time.Toc(); // max 1MHz frequency
    // reset counter
    _time.Tic();
    // store process frequency
    _synch.InstanceLock("_processData");
    _processData % "frequency" = _pulseFrequency;
    _synch.InstanceUnlock("_processData");
    
    
}


/*
 _____                     _
| ____|_  _____  ___ _   _| |_ ___
|  _| \ \/ / _ \/ __| | | | __/ _ \
| |___ >  <  __/ (__| |_| | ||  __/
|_____/_/\_\___|\___|\__,_|\__\___|
*/
bool CSimProcess::Execute() {
    // start process threads
    ThreadRun("WatchDog",NULL);
    ThreadRun("SIM",NULL);

    // wait the process to terminate
    while ( _processState != PROCESS_STATE_KILLED &&
            _processState != PROCESS_STATE_ERROR)
            Sleep(100);

    // terminate the threads
    ThreadTerminate("SIM");
    ThreadTerminate("WatchDog");
    
    if (_processState == PROCESS_STATE_ERROR)
        return false;
    else
        return true;
}

/*
 _____                     _       ___          _  _____ _                        _
| ____|_  _____  ___ _   _| |_ ___|_ _|_ __    / \|_   _| |__  _ __ ___  __ _  __| |
|  _| \ \/ / _ \/ __| | | | __/ _ \| || '_ \  / _ \ | | | '_ \| '__/ _ \/ _` |/ _` |
| |___ >  <  __/ (__| |_| | ||  __/| || | | |/ ___ \| | | | | | | |  __/ (_| | (_| |
|_____/_/\_\___|\___|\__,_|\__\___|___|_| |_/_/   \_\_| |_| |_|_|  \___|\__,_|\__,_|
*/
void CSimProcess::ExecuteInAThread() {
    // start process threads
    ThreadRun("WatchDog",NULL);
    ThreadRun("SIM",NULL);
    ThreadRun("ThreadedExecution",NULL);
}

/*
 _____ _                        _          _ __  __      _   _               _
|_   _| |__  _ __ ___  __ _  __| | ___  __| |  \/  | ___| |_| |__   ___   __| |
  | | | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | |\/| |/ _ \ __| '_ \ / _ \ / _` |
  | | | | | | | |  __/ (_| | (_| |  __/ (_| | |  | |  __/ |_| | | | (_) | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|\___|\__,_|_|  |_|\___|\__|_| |_|\___/ \__,_|
*/
void CSimProcess::ThreadedMethod(std::string thread_name, void *thread_params) {
    
    if (thread_name == "WatchDog") {    // process watchdog thread
        
        // synhronize with 10Hz
        _watchdogSynch.SynchWithFrequency(10);

        // signal the process state
        _synch.InstanceLock("_processData");
        _ipc.WriteDataToChannel(PROCESS_STATE_CHANNEL_NAME, _processData);
        _synch.InstanceUnlock("_processData");

    }
    else if (thread_name == "SIM") {
        // synch and pulse process
        _synch.SynchWithFrequency((_processData % "synch_frequency").toFloat());
        Pulse();

        // get the last state required for the proces
        _ipc.GetChannelData(PROCESS_CONTROL_CHANNEL_NAME, _processControlState);
        if (_processControlState % "process_name" == _processData % "name" ||
            (_processControlState % "process_name").toString() == "ALL")
            _processData % "RQstate" = _processControlState % "controlState";
        
        e_ProcessControl nextState;
        nextState = CGENERICTYPE_TO_PROCESS_CONTROL(_processData % "RQstate");

        //// delegate must be assigned
        //if (_processState != PROCESS_STATE_ERROR && _processDelegate == NULL) {
        //    SetState(PROCESS_STATE_ERROR);
        //    *_trace << "Delegate not assigned" << endl;
        //}

        // simulation thread cycle, switch the state
        switch(_processState) {
            /*
             ___ ___  ___   ___ ___ ___ ___   ___ _____ _ _____ ___   _    ___   _   ___  ___ ___
            | _ \ _ \/ _ \ / __| __/ __/ __| / __|_   _/_\_   _| __| | |  / _ \ /_\ |   \| __|   \
            |  _/   / (_) | (__| _|\__ \__ \ \__ \ | |/ _ \| | | _|  | |_| (_) / _ \| |) | _|| |) |
            |_| |_|_\\___/ \___|___|___/___/_|___/ |_/_/ \_\_| |___|_|____\___/_/ \_\___/|___|___/
                                          |___|                   |___|
            */
            case e_ProcessState::PROCESS_STATE_LOADED:  
                // LOADED, NOT INITIALIZED
                // initialize the process
                SetState(PROCESS_STATE_ININTIALIZING);
                if (_processDelegate != NULL) {
                    if (!_processDelegate->Initialize()) {
                        SetState(PROCESS_STATE_ERROR);
                        break;
                    }
                }
                // switch to STOPPED state
                SetState(PROCESS_STATE_STOPPED);
                break;

                /*
                 ___ ___  ___   ___ ___ ___ ___   ___ _____ _ _____ ___   ___ _____ ___  ___ ___ ___ ___
                | _ \ _ \/ _ \ / __| __/ __/ __| / __|_   _/_\_   _| __| / __|_   _/ _ \| _ \ _ \ __|   \
                |  _/   / (_) | (__| _|\__ \__ \ \__ \ | |/ _ \| | | _|  \__ \ | || (_) |  _/  _/ _|| |) |
                |_| |_|_\\___/ \___|___|___/___/_|___/ |_/_/ \_\_| |___|_|___/ |_| \___/|_| |_| |___|___/
                                              |___|                   |___|
                */
            case e_ProcessState::PROCESS_STATE_STOPPED:
                // execute the stop task
                if (_processDelegate != NULL) {
                    if (!_processDelegate->Stop()) {
                        SetState(PROCESS_STATE_ERROR);
                        break;
                    }
                }

                // check the contol state
                if (nextState == PROCESS_CONTROL_START   || 
                    nextState == PROCESS_CONTROL_PAUSE       ) {
                    // process is required to start
                    SetState(PROCESS_STATE_GOINGTOSTART);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->IsGoingToStart()) {
                            SetState(PROCESS_STATE_ERROR);
                            break;
                        }
                    }
                    // started
                    SetState(PROCESS_STATE_RUNNING);
                }
                else if (nextState == PROCESS_CONTROL_SHUTDOWN) {
                    // process is required to be stopped
                    SetState(PROCESS_STATE_SHUTTINGDOWN);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->Shutdown())
                            SetState(PROCESS_STATE_ERROR);
                    }
                    SetState(PROCESS_STATE_KILLED);
 
                }
                else if (nextState == PROCESS_CONTROL_RESET) {
                    // process is required to be stopped
                    SetState(PROCESS_STATE_SHUTTINGDOWN);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->Shutdown())
                            SetState(PROCESS_STATE_ERROR);
                    }
                    SetState(PROCESS_STATE_ININTIALIZING);
                }
                break;

                /*
                 ___ ___  ___   ___ ___ ___ ___   ___ _____ _ _____ ___   ___ _   _ _  _ _  _ ___ _  _  ___
                | _ \ _ \/ _ \ / __| __/ __/ __| / __|_   _/_\_   _| __| | _ \ | | | \| | \| |_ _| \| |/ __|
                |  _/   / (_) | (__| _|\__ \__ \ \__ \ | |/ _ \| | | _|  |   / |_| | .` | .` || || .` | (_ |
                |_| |_|_\\___/ \___|___|___/___/_|___/ |_/_/ \_\_| |___|_|_|_\\___/|_|\_|_|\_|___|_|\_|\___|
                                              |___|                   |___|
                */
            case e_ProcessState::PROCESS_STATE_RUNNING:
                // execute the run task
                if (_processDelegate != NULL) {
                    if (!_processDelegate->Run()) {
                        SetState(PROCESS_STATE_ERROR);
                        break;
                    }
                }

                // check the control state
                if (nextState == PROCESS_CONTROL_STOP        ||
                    nextState == PROCESS_CONTROL_SHUTDOWN    ||
                    nextState == PROCESS_CONTROL_RESET           ) {
                    // process is required to be stopped
                    SetState(PROCESS_STATE_GOINGTOSTOP);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->IsGoingToStop()) {
                            SetState(PROCESS_STATE_ERROR);
                            break;
                        }
                    }
                    SetState(PROCESS_STATE_STOPPED);
                }
                else if (nextState == PROCESS_CONTROL_PAUSE) {
                    // process is required to be stopped
                    SetState(PROCESS_STATE_GOINGTOPAUSE);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->IsGoingToPause()) {
                            SetState(PROCESS_STATE_ERROR);
                            break;
                        }
                    }
                    SetState(PROCESS_STATE_PAUSED);
                }
                break;

                /*
                 ___ ___  ___   ___ ___ ___ ___   ___ _____ _ _____ ___   ___  _  _   _ ___ ___ ___
                | _ \ _ \/ _ \ / __| __/ __/ __| / __|_   _/_\_   _| __| | _ \/_\| | | / __| __|   \
                |  _/   / (_) | (__| _|\__ \__ \ \__ \ | |/ _ \| | | _|  |  _/ _ \ |_| \__ \ _|| |) |
                |_| |_|_\\___/ \___|___|___/___/_|___/ |_/_/ \_\_| |___|_|_|/_/ \_\___/|___/___|___/
                                              |___|                   |___|
                */
            case e_ProcessState::PROCESS_STATE_PAUSED:
                // execute the run task
                if (_processDelegate != NULL) {
                    if (!_processDelegate->Pause()) {
                        SetState(PROCESS_STATE_ERROR);
                        break;
                    }
                }

                // check the control state
                if (nextState == PROCESS_CONTROL_STOP        ||
                    nextState == PROCESS_CONTROL_SHUTDOWN    ||
                    nextState == PROCESS_CONTROL_RESET           ) {
                    // process is required to be stopped
                    SetState(PROCESS_STATE_GOINGTOSTOP);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->IsGoingToStop()) {
                            SetState(PROCESS_STATE_ERROR);
                            break;
                        }
                    }
                    SetState(PROCESS_STATE_STOPPED);
                }
                else if (nextState == PROCESS_CONTROL_START) {
                    // process is required to be stopped
                    SetState(PROCESS_STATE_GOINGTORESUME);
                    if (_processDelegate != NULL) {
                        if (!_processDelegate->IsGoingToResume()) {
                            SetState(PROCESS_STATE_ERROR);
                            break;
                        }
                    }
                    SetState(PROCESS_STATE_RUNNING);
                }
                break;
        }
    }
    else if (thread_name == "ThreadedExecution") {
        
        // synhronize with 10Hz
        _watchdogSynch.SynchWithFrequency(10);

        // check the process state
        if ( _processState == PROCESS_STATE_KILLED ||
            _processState == PROCESS_STATE_ERROR)
        {
            // terminate the threads
            ThreadTerminate("SIM");
            ThreadTerminate("WatchDog");
            ThreadTerminationRequest("ThreadedExecution");
        }
    }
}