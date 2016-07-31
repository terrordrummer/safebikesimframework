#include "FrameworkCore/CThread.h"
#include "FrameworkCore/CSynch.h"
#include <iostream>

using namespace FrameworkCore;


// -------------------------------------- RUNNING CALLBACK -------------------------------------------
DWORD WINAPI TransmitterThread_thread_function(__in  LPVOID class_pointer) {
    /* this class inherit the CThread class and implements the public method run_thread which launch 
     * the protected
     * method RunManager (which should not normally be accessible
     */
    class aux_thread : public CThread {
    public:
        void MyRunManager() {RunManager();};
        };

    CThread *PP = (CThread *) class_pointer;
    //aux_thread *p = static_cast< aux_thread * > (class_pointer);
    aux_thread *p = static_cast< aux_thread * > (PP);
    p->MyRunManager();
    return 0;
    };
// ---------------------------------------------------------------------------------------------------

#include <iostream>

/*
  ____ _____ _                        _
 / ___|_   _| |__  _ __ ___  __ _  __| |
| |     | | | '_ \| '__/ _ \/ _` |/ _` |
| |___  | | | | | | | |  __/ (_| | (_| |
 \____| |_| |_| |_|_|  \___|\__,_|\__,_|
*/
CThread::CThread() : _threadLaunchFlag(false)  {
    _runningThreads.clear();
    _terminationRequest.clear();
    _launchingThreadName.clear();
    _threadIDs.clear();
    }

/*
 /\/|___ _____ _                        _
|/\/ ___|_   _| |__  _ __ ___  __ _  __| |
  | |     | | | '_ \| '__/ _ \/ _` |/ _` |
  | |___  | | | | | | | |  __/ (_| | (_| |
   \____| |_| |_| |_|_|  \___|\__,_|\__,_|
*/
CThread::~CThread() {
    // if object is destroyed the threads are terminated too
    // ask the threads to terminate
    std::set<std::string>::iterator it = _runningThreads.begin();
    while (it != _runningThreads.end()) {
        // terminate the thread
        ThreadTerminate(*it);
        it = _runningThreads.begin();
        }
    _runningThreads.clear();
    _terminationRequest.clear();
    _threadIDs.clear();
    }

/*
 ____              __  __
|  _ \ _   _ _ __ |  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |_) | | | | '_ \| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
|  _ <| |_| | | | | |  | | (_| | | | | (_| | (_| |  __/ |
|_| \_\\__,_|_| |_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                            |___/
*/
/**
 *
 * This is the method that will run inside threaded. It implements a do-while loop that calls 
 * the ThreadedMethod until the thread's termination flag is setted.
 */
void CThread::RunManager() {
    // store locally the thread name and the pointer params
    std::string thread_name = _launchingThreadName;
    void *params = _launchingThreadParams;
    CSynch::IntraProcessLock("_runningThreads");
    _runningThreads.insert(thread_name);
    CSynch::IntraProcessUnlock("_runningThreads");
    // notify that thread is launched
    _threadLaunchFlag = false;
    // wait the launcher method to terminate
    CSynch::IntraProcessLock("launching_thread");
    CSynch::IntraProcessUnlock("launching_thread");
    // enter the thread loop
    do {
        ThreadedMethod(thread_name, params);
        } while (!_terminationRequest[thread_name]);
    _runningThreads.erase(thread_name);
    _threadIDs.erase(thread_name);
    }

/*
 _____ _                        _          _ __  __      _   _               _
|_   _| |__  _ __ ___  __ _  __| | ___  __| |  \/  | ___| |_| |__   ___   __| |
  | | | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | |\/| |/ _ \ __| '_ \ / _ \ / _` |
  | | | | | | | |  __/ (_| | (_| |  __/ (_| | |  | |  __/ |_| | | | (_) | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|\___|\__,_|_|  |_|\___|\__|_| |_|\___/ \__,_|
*/
/**
 *  \param thread_name string containing the name identifier of the running thread
 *  \param thread_params pointer to the thread parameters (if allocated)
 *
 * This method has to be specialized by the inheriting class and implements the code that will be 
 * runned in threads. In each thread, the thread's name and the parameters pointer are passed to 
 * the method allowing the threaded code to select the actions and use the correct parameter's pointer.
 */
void CThread::ThreadedMethod(std::string name, void* params) {
    CSynch::IntraProcessLock("cout");
    std::cout << "thread_ThreadedMethod <" << name << "> not specialized" << std::endl;
    CSynch::IntraProcessUnlock("cout");
    Sleep(1000);
    }

/*
 _____ _                        _ ____               ___
|_   _| |__  _ __ ___  __ _  __| |  _ \ _   _ _ __  / _ \ _ __   ___ ___
  | | | '_ \| '__/ _ \/ _` |/ _` | |_) | | | | '_ \| | | | '_ \ / __/ _ \
  | | | | | | | |  __/ (_| | (_| |  _ <| |_| | | | | |_| | | | | (_|  __/
  |_| |_| |_|_|  \___|\__,_|\__,_|_| \_\\__,_|_| |_|\___/|_| |_|\___\___|
*/
/**
 *  \param thread_name name identifier of the thread to be runned once
 *  \param params memory pointer to the thread parameters
 *
 * This method runs once the ThreadedMethod with the given thread identifier string. Some parameters can be
 * passed to the thread using params pointer. The thread method should implement a switch to determine
 * the action to do depending on the thread identifier value.
 */
void CThread::ThreadRunOnce(std::string name, void *params) {
    CSynch::IntraProcessLock("_runningThreads");
    if (_runningThreads.find(name) != _runningThreads.end()) {
        // a thread with this name is already running
        CSynch::IntraProcessUnlock("_runningThreads");
        return;
        }
    CSynch::IntraProcessUnlock("_runningThreads");

    CSynch::IntraProcessLock("launching_thread");
    _launchingThreadName = name;
    _launchingThreadParams = params;
    _terminationRequest[name] = true;
    _threadLaunchFlag = true;
    DWORD TransmitterThread_id;
    //HANDLE res = CreateThread(NULL,0,&TransmitterThread_thread_function,this,0,&TransmitterThread_id);
    CThread *PP = dynamic_cast<CThread *> (this);
    HANDLE res = CreateThread(NULL,0,&TransmitterThread_thread_function,(LPVOID)PP,0,&TransmitterThread_id);
    _threadIDs[name] = (unsigned long long)res;
    while(_threadLaunchFlag) {Sleep(1);}
    _runningThreads.insert(name);
    CSynch::IntraProcessUnlock("launching_thread");
    }

/*
 _____ _                        _ ____
|_   _| |__  _ __ ___  __ _  __| |  _ \ _   _ _ __
  | | | '_ \| '__/ _ \/ _` |/ _` | |_) | | | | '_ \
  | | | | | | | |  __/ (_| | (_| |  _ <| |_| | | | |
  |_| |_| |_|_|  \___|\__,_|\__,_|_| \_\\__,_|_| |_|
*/
/**
 *  \param thread_name name identifier of the thread to be runned once
 *  \param params memory pointer to the thread parameters
 *
 * This method runs ciclically the ThreadedMethod with the given thread name identified and the parameters
 * pointer by params. To stop the thread execution the ThreadTerminate method has to be called with the
 * same thread_name parameter.
 */
void CThread::ThreadRun(std::string name, void *params) {
    // allow a single thread name instance
    CSynch::IntraProcessLock("_runningThreads");
    if (_runningThreads.find(name) != _runningThreads.end()) {
        // a thread with this name is already running
        CSynch::IntraProcessUnlock("_runningThreads");
        return;
        }
    CSynch::IntraProcessUnlock("_runningThreads");

    CSynch::IntraProcessLock("launching_thread");
    _launchingThreadName = name;
    _launchingThreadParams = params;
    _terminationRequest[name] = false;
    _threadLaunchFlag = true;
    DWORD TransmitterThread_id;
    //HANDLE res = CreateThread(NULL,0,&TransmitterThread_thread_function,this,0,&TransmitterThread_id);
    CThread *PP = dynamic_cast<CThread *> (this);
    HANDLE res = CreateThread(NULL,0,&TransmitterThread_thread_function,(LPVOID)PP,0,&TransmitterThread_id);
    _threadIDs[name] = (unsigned long long)res;
    while(_threadLaunchFlag) {Sleep(1);}
    _runningThreads.insert(name);
    CSynch::IntraProcessUnlock("launching_thread");
    }

/*
 _____ _                        _ _____                   _             _   _             ____                           _
|_   _| |__  _ __ ___  __ _  __| |_   _|__ _ __ _ __ ___ (_)_ __   __ _| |_(_) ___  _ __ |  _ \ ___  __ _ _   _  ___ ___| |_
  | | | '_ \| '__/ _ \/ _` |/ _` | | |/ _ \ '__| '_ ` _ \| | '_ \ / _` | __| |/ _ \| '_ \| |_) / _ \/ _` | | | |/ _ Y __| __|
  | | | | | | | |  __/ (_| | (_| | | |  __/ |  | | | | | | | | | | (_| | |_| | (_) | | | |  _ <  __/ (_| | |_| |  __|__ \ |_
  |_| |_| |_|_|  \___|\__,_|\__,_| |_|\___|_|  |_| |_| |_|_|_| |_|\__,_|\__|_|\___/|_| |_|_| \_\___|\__, |\__,_|\___|___/\__|
                                                                                                       |_|
*/
/**
 *  \param thread_name name identifier of the thread to be stopped
 *
 * This method ask the thread identified by the thread_name string to terminate. The method exits immediatly
 * and the thread will be terminated as soon as possible.
 */
void CThread::ThreadTerminationRequest(std::string name) {
    // thread with this name must exist
    CSynch::IntraProcessLock("_runningThreads");
    if (_runningThreads.find(name) == _runningThreads.end()) {
        // a thread with this name does not exist
        CSynch::IntraProcessUnlock("_runningThreads");
        return;
    }
    _terminationRequest[name] = true;
    CSynch::IntraProcessUnlock("_runningThreads");
    }

/*
 _____ _                        _ _____                   _             _
|_   _| |__  _ __ ___  __ _  __| |_   _|__ _ __ _ __ ___ (_)_ __   __ _| |_ ___
  | | | '_ \| '__/ _ \/ _` |/ _` | | |/ _ \ '__| '_ ` _ \| | '_ \ / _` | __/ _ \
  | | | | | | | |  __/ (_| | (_| | | |  __/ |  | | | | | | | | | | (_| | ||  __/
  |_| |_| |_|_|  \___|\__,_|\__,_| |_|\___|_|  |_| |_| |_|_|_| |_|\__,_|\__\___|
*/
/**
 *  \param thread_name name identifier of the thread to be stopped
 *
 * This method ask the thread identified by the thread_name string to terminate. The method will not exit
 * until the thread will be terminated.
 */
void CThread::ThreadTerminate(std::string name) {
    // ask the thread to terminate
    _terminationRequest[name] = true;
    while (1) {
        CSynch::IntraProcessLock("_runningThreads");
        // check if thread name is removed from the thread list
        if (_runningThreads.find(name) == _runningThreads.end())
            break;

        // check if thread is still alive
        DWORD res = 0;
        GetExitCodeThread((HANDLE)_threadIDs[name], &res);
        if (res != STILL_ACTIVE) {
            // thread is dead, clear its data
            _threadIDs.erase(name);
            _runningThreads.erase(name);
            break;
            }
        
        CSynch::IntraProcessUnlock("_runningThreads");
        Sleep(100);
        }
    CSynch::IntraProcessUnlock("_runningThreads");
    }

/*
 _____ _                        _ ___     ____                    _
|_   _| |__  _ __ ___  __ _  __| |_ _|___|  _ \ _   _ _ __  _ __ (_)_ __   __ _
  | | | '_ \| '__/ _ \/ _` |/ _` || |/ __| |_) | | | | '_ \| '_ \| | '_ \ / _` |
  | | | | | | | |  __/ (_| | (_| || |\__ \  _ <| |_| | | | | | | | | | | | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|___|___/_| \_\\__,_|_| |_|_| |_|_|_| |_|\__, |
                                                                          |___/
*/
/**
 *  \param thread_name name identifier of the thread to be stopped
 *  \return running status flag
 * 
 * This method returns the running status flag of the thread identified by the thread_name string. As return
 * vaule will be returned TRUE if there exist a thread with that name that is running, FALSE otherwise. 
 */
bool CThread::ThreadIsRunning(std::string name) {
    bool res;
    CSynch::IntraProcessLock("_runningThreads");
    res = _runningThreads.find(name) != _runningThreads.end();
    CSynch::IntraProcessUnlock("_runningThreads");
    return res;
    }

