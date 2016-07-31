#include "FrameworkSimArc/CSimControl.h"

#include <vector>

// namespaces
using namespace FrameworkSimArc;
using namespace FrameworkCore;
using namespace std;

#

/*
  ____ ____  _            ____            _             _
 / ___/ ___|(_)_ __ ___  / ___|___  _ __ | |_ _ __ ___ | |
| |   \___ \| | '_ ` _ \| |   / _ \| '_ \| __| '__/ _ \| |
| |___ ___) | | | | | | | |__| (_) | | | | |_| | | (_) | |
 \____|____/|_|_| |_| |_|\____\___/|_| |_|\__|_|  \___/|_|
*/
CSimControl::CSimControl() : _controlStateSet(false) {

    SetErrorClassPrefix("CSimProcess");
    _activeProcessesTimeouts.clear();
    _activeProcesses    = 0;

    // Join the IPC group
    _ipc.Join();
    _ipc.EnableDataSynchronization(PROCESS_STATE_CHANNEL_NAME);

    // set default simulation state target processes: ALL
    _simStateControl % "process_name"  = "ALL";

    // Run the control thread
    ThreadRun("ProcessDataThread");
    ThreadRun("TimeoutedProcessCleanerThread");
}

/*
 /\/|___ ____  _            ____            _             _
|/\/ ___/ ___|(_)_ __ ___  / ___|___  _ __ | |_ _ __ ___ | |
  | |   \___ \| | '_ ` _ \| |   / _ \| '_ \| __| '__/ _ \| |
  | |___ ___) | | | | | | | |__| (_) | | | | |_| | | (_) | |
   \____|____/|_|_| |_| |_|\____\___/|_| |_|\__|_|  \___/|_|
*/
CSimControl::~CSimControl() {

    DisableSimControl();

    ThreadTerminate("ProcessDataThread");
    ThreadTerminate("TimeoutedProcessCleanerThread");

    // Leave the ipc group
    _ipc.DisableAllDataSynchronization();
    _ipc.Leave();

    _dataSynch.InstanceLock("_activeProcesses");
    _activeProcessesTimeouts.clear();
    _activeProcesses = 0;
    _dataSynch.InstanceUnlock("_activeProcesses");

}

/** \brief Enable the simulation control.                                   */
void CSimControl::EnableSimControl() {
    _controlStateSet = false;
    ThreadRun("SIMCONTROL");
}

/** \brief Disable the simulation control.                                  */
void CSimControl::DisableSimControl() {
    if (ThreadIsRunning("SIMCONTROL"))
        ThreadTerminate("SIMCONTROL");
    _controlStateSet = false;
}

/*
 ____  _             _
/ ___|| |_ __ _ _ __| |_
\___ \| __/ _` | '__| __|
 ___) | || (_| | |  | |_
|____/ \__\__,_|_|   \__|
*/
void CSimControl::Start() {
    // send the START control state to all processes
    if (!ThreadIsRunning("SIMCONTROL"))
        return;

    _dataSynch.InstanceLock("simControl");
    _simStateControl % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_START);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, _simStateControl);  
    _dataSynch.InstanceUnlock("simControl");
    _controlStateSet = true;
}

/*
 ____  _
/ ___|| |_ ___  _ __
\___ \| __/ _ \| '_ \
 ___) | || (_) | |_) |
|____/ \__\___/| .__/
               |_|
*/
void CSimControl::Stop() {
    // send the STOP control state to all processes
    if (!ThreadIsRunning("SIMCONTROL"))
        return;

    _dataSynch.InstanceLock("simControl");
    _simStateControl % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_STOP);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, _simStateControl);  
    _dataSynch.InstanceUnlock("simControl");
    _controlStateSet = true;
}

/*
 ____
|  _ \ __ _ _   _ ___  ___
| |_) / _` | | | / __|/ _ \
|  __/ (_| | |_| \__ \  __/
|_|   \__,_|\__,_|___/\___|
*/
void CSimControl::Pause() {
    // send the PAUSE control state to all processes
    if (!ThreadIsRunning("SIMCONTROL"))
        return;

    _dataSynch.InstanceLock("simControl");
    _simStateControl % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_PAUSE);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, _simStateControl);  
    _dataSynch.InstanceUnlock("simControl");
    _controlStateSet = true;
}

/*
 ____  _           _      _
/ ___|| |__  _   _| |_ __| | _____      ___ __
\___ \| '_ \| | | | __/ _` |/ _ \ \ /\ / / '_ \
 ___) | | | | |_| | || (_| | (_) \ V  V /| | | |
|____/|_| |_|\__,_|\__\__,_|\___/ \_/\_/ |_| |_|
*/
void CSimControl::Shutdown() {
    // send the SHUTDOWN control state to all processes
    if (!ThreadIsRunning("SIMCONTROL"))
        return;

    _dataSynch.InstanceLock("simControl");
    _simStateControl % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_SHUTDOWN);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, _simStateControl);  
    _dataSynch.InstanceUnlock("simControl");
    _controlStateSet = true;
}

/*
 ____               _
|  _ \ ___ ___  ___| |_
| |_) / _ Y __|/ _ \ __|
|  _ <  __|__ \  __/ |_
|_| \_\___|___/\___|\__|
*/
void CSimControl::Reset() {
    // send the RESET control state to all processes
    if (!ThreadIsRunning("SIMCONTROL"))
        return;

    CGenericType resetCommand;
    resetCommand = _simStateControl;
    // RESET is sent one time, then processes are required to stop
    _dataSynch.InstanceLock("simControl");
    _simStateControl % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_STOP);
    resetCommand % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_RESET);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, resetCommand);  
    _dataSynch.InstanceUnlock("simControl");
    _controlStateSet = true;
}

/*
 ____  _             _   ____
/ ___|| |_ __ _ _ __| |_|  _ \ _ __ ___   ___ ___ ___ ___
\___ \| __/ _` | '__| __| |_) | '__/ _ \ / __/ _ Y __/ __|
 ___) | || (_| | |  | |_|  __/| | | (_) | (_|  __|__ \__ \
|____/ \__\__,_|_|   \__|_|   |_|  \___/ \___\___|___/___/
*/
void CSimControl::StartProcess(std::string name) {
    // send the START control state to all processes
    CGenericType c;
    c % "process_name"  = name;
    c % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_START);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, c);
}

/*
 ____  _              ____
/ ___|| |_ ___  _ __ |  _ \ _ __ ___   ___ ___ ___ ___
\___ \| __/ _ \| '_ \| |_) | '__/ _ \ / __/ _ Y __/ __|
 ___) | || (_) | |_) |  __/| | | (_) | (_|  __|__ \__ \
|____/ \__\___/| .__/|_|   |_|  \___/ \___\___|___/___/
               |_|
*/
void CSimControl::StopProcess(std::string name) {
    // send the STOP control state to all processes
    CGenericType c;
    c % "process_name"  = name;
    c % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_STOP);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, c);
}

/*
 ____                      ____
|  _ \ __ _ _   _ ___  ___|  _ \ _ __ ___   ___ ___ ___ ___
| |_) / _` | | | / __|/ _ \ |_) | '__/ _ \ / __/ _ Y __/ __|
|  __/ (_| | |_| \__ \  __/  __/| | | (_) | (_|  __|__ \__ \
|_|   \__,_|\__,_|___/\___|_|   |_|  \___/ \___\___|___/___/
*/
void CSimControl::PauseProcess(std::string name) {
    // send the PAUSE control state to all processes
    CGenericType c;
    c % "process_name"  = name;
    c % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_PAUSE);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, c);
}

/*
 ____  _           _      _                     ____
/ ___|| |__  _   _| |_ __| | _____      ___ __ |  _ \ _ __ ___   ___ ___ ___ ___
\___ \| '_ \| | | | __/ _` |/ _ \ \ /\ / / '_ \| |_) | '__/ _ \ / __/ _ Y __/ __|
 ___) | | | | |_| | || (_| | (_) \ V  V /| | | |  __/| | | (_) | (_|  __|__ \__ \
|____/|_| |_|\__,_|\__\__,_|\___/ \_/\_/ |_| |_|_|   |_|  \___/ \___\___|___/___/
*/
void CSimControl::ShutdownProcess(std::string name) {
    // send the SHUTDOWN control state to all processes
    CGenericType c;
    c % "process_name"  = name;
    c % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_SHUTDOWN);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, c);
}

/*
 ____               _   ____
|  _ \ ___ ___  ___| |_|  _ \ _ __ ___   ___ ___ ___ ___
| |_) / _ Y __|/ _ \ __| |_) | '__/ _ \ / __/ _ Y __/ __|
|  _ <  __|__ \  __/ |_|  __/| | | (_) | (_|  __|__ \__ \
|_| \_\___|___/\___|\__|_|   |_|  \___/ \___\___|___/___/
*/
void CSimControl::ResetProcess(std::string name) {
    // send the RESET control state to all processes
    CGenericType c;
    c % "process_name"  = name;
    c % "controlState"  = PROCESS_CONTROL_TO_CGENERICTYPE(PROCESS_CONTROL_RESET);
    _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, c);
}

/*
  ____      _   ____                             _     _     _
 / ___| ___| |_|  _ \ _ __ ___   ___ ___ ___ ___| |   (_)___| |_
| |  _ / _ \ __| |_) | '__/ _ \ / __/ _ Y __/ __| |   | / __| __|
| |_| |  __/ |_|  __/| | | (_) | (_|  __|__ \__ \ |___| \__ \ |_
 \____|\___|\__|_|   |_|  \___/ \___\___|___/___/_____|_|___/\__|
*/
void CSimControl::GetProcessList(FrameworkCore::CGenericType &data) {
    data = 0;
    _dataSynch.InstanceLock("_activeProcesses");
    vector<string> list = _activeProcesses.GetFields();
    for (unsigned int i=0; i<list.size(); i++)
        data[i] = list[i];
    _dataSynch.InstanceUnlock("_activeProcesses");
}

/*
  ____      _   ____                             ____        _
 / ___| ___| |_|  _ \ _ __ ___   ___ ___ ___ ___|  _ \  __ _| |_ __ _
| |  _ / _ \ __| |_) | '__/ _ \ / __/ _ Y __/ __| | | |/ _` | __/ _` |
| |_| |  __/ |_|  __/| | | (_) | (_|  __|__ \__ \ |_| | (_| | || (_| |
 \____|\___|\__|_|   |_|  \___/ \___\___|___/___/____/ \__,_|\__\__,_|
*/
void CSimControl::GetProcessData(FrameworkCore::CGenericType &data) {
    data = 0;
    _dataSynch.InstanceLock("_activeProcesses");
    data = _activeProcesses;
    _dataSynch.InstanceUnlock("_activeProcesses");
}

/*
 _____ _                        _          _ __  __      _   _               _
|_   _| |__  _ __ ___  __ _  __| | ___  __| |  \/  | ___| |_| |__   ___   __| |
  | | | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | |\/| |/ _ \ __| '_ \ / _ \ / _` |
  | | | | | | | |  __/ (_| | (_| |  __/ (_| | |  | |  __/ |_| | | | (_) | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|\___|\__,_|_|  |_|\___|\__|_| |_|\___/ \__,_|
*/
void CSimControl::ThreadedMethod(std::string thread_name, void *thread_params) {
    // Input data coming from the active processes
    if ( thread_name == "ProcessDataThread" ) {
        if (_ipc.WaitDataFromChannel(PROCESS_STATE_CHANNEL_NAME, _multicastData, 100)) {
            // process data received, store the process state and reset its timer counter
            _dataSynch.InstanceLock("_activeProcesses");
            string name = (_multicastData % "name").toString();
            _activeProcesses % name = _multicastData;
            if (_activeProcessesTimeouts.find(name) == _activeProcessesTimeouts.end())
                _activeProcessesTimeouts[name] = CTime();
            _activeProcessesTimeouts[name].Tic();
            // check if the process state is KILLED then remove the process from the list of active processes
            if ( CGENERICTYPE_TO_PROCESS_STATE(_multicastData % "state") == PROCESS_STATE_KILLED ) {
                _activeProcesses.RemoveField(name);
                _activeProcessesTimeouts.erase(name);
            }
            _dataSynch.InstanceUnlock("_activeProcesses");
        }
    }

    else if (thread_name == "TimeoutedProcessCleanerThread") {
        
        _processCleanerSynch.SynchWithFrequency(0.2f);   // clean process data every 5 second
        _dataSynch.InstanceLock("_activeProcesses");

        // cycle and look for timedout processes
        std::map<std::string, FrameworkCore::CTime>::iterator it = _activeProcessesTimeouts.begin();
        while (it != _activeProcessesTimeouts.end()) {
            if (it->second.Toc() > PROCESS_TIMEOUT) {
                _activeProcesses.RemoveField(it->first);
                _activeProcessesTimeouts.erase(it->first);
                it = _activeProcessesTimeouts.begin();
            }
            else
                it++;
        }

        _dataSynch.InstanceUnlock("_activeProcesses");
    }
    else if (thread_name == "SIMCONTROL") {
        _processCleanerSynch.SynchWithFrequency(10);   // clean process data every 1 second
        
        if (_controlStateSet) {
            _dataSynch.InstanceLock("simControl");
            _ipc.WriteDataToChannel(PROCESS_CONTROL_CHANNEL_NAME, _simStateControl);    
            _dataSynch.InstanceUnlock("simControl");
        }
    }
}