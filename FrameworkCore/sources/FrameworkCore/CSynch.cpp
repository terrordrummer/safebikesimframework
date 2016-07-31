#include "FrameworkCore/CSynch.h"
#include <sstream>

using namespace FrameworkCore;

// --------------------------------- static decaration
int CSynch::_instances = 0;

CSynch::_synchMapType CSynch::_intraprocessSynch;
CRITICAL_SECTION CSynch::_intraprocessLockCs;
CRITICAL_SECTION CSynch::_intraprocessLockTimeoutCs;
CRITICAL_SECTION CSynch::_intraprocessUnlockCs;

CSynch::_synchMapType CSynch::_interprocessSynch;
CRITICAL_SECTION CSynch::_interprocessLockCs;
CRITICAL_SECTION CSynch::_interprocessLockTimeoutCs;
CRITICAL_SECTION CSynch::_interprocessUnlockCs;
// ---------------------------------------------------

/*
     _        _   _      ___       _ _   _       _ _
 ___| |_ __ _| |_(_) ___|_ _|_ __ (_) |_(_) __ _| (_)_______ _ __
/ __| __/ _` | __| |/ __|| || '_ \| | __| |/ _` | | |_  / _ \ '__|
\__ \ || (_| | |_| | (__ | || | | | | |_| | (_| | | |/ /  __/ |
|___/\__\__,_|\__|_|\___|___|_| |_|_|\__|_|\__,_|_|_/___\___|_|
*/
void CSynch::staticInitializer() {
    _instances = 0;
    }

/*
  ____ ____                   _
 / ___/ ___| _   _ _ __   ___| |__
| |   \___ \| | | | '_ \ / __| '_ \
| |___ ___) | |_| | | | | (__| | | |
 \____|____/ \__, |_| |_|\___|_| |_|
             |___/
*/
CSynch::CSynch() :    _SWFLastCALLTime(0)
                    , _SWFFirstPass(true) {
    StaticInit();
    _instances++;
    _instanceSynch.clear();
    _events.clear();
    InitializeCriticalSection(&_instanceLockCs);
    InitializeCriticalSection(&_instanceLockTimeoutCs);
    InitializeCriticalSection(&_instanceUnlockCs);
    };

/*
 /\/|___ ____                   _
|/\/ ___/ ___| _   _ _ __   ___| |__
  | |   \___ \| | | | '_ \ / __| '_ \
  | |___ ___) | |_| | | | | (__| | | |
   \____|____/ \__, |_| |_|\___|_| |_|
               |___/
*/
CSynch::~CSynch() {
    if (--_instances == 0) {
        // unlock intraprocess mutexes
        CSynch::_synchMapType::iterator mutex_it = _intraprocessSynch.begin();
        while (mutex_it != _intraprocessSynch.end()) {
            IntraProcessUnlock(mutex_it->first);
            DESTROY_MUTEX(mutex_it->second);
            mutex_it++;
            };
        
        // unlock interprocess mutexes
        mutex_it = _interprocessSynch.begin();
        while (mutex_it != _interprocessSynch.end()) {
            InterProcessUnlock(mutex_it->first);
            DESTROY_MUTEX(mutex_it->second);
            mutex_it++;
            };

        // signal all _events
        CSynch::_eventMapType::iterator event_it = _events.begin();
        while (event_it != _events.end()) {
            NotifyEvent(event_it->first);
            event_it++;
            };
        // clear containers
        _interprocessSynch.clear();
        _intraprocessSynch.clear();
        _events.clear();
        DeleteCriticalSection(&_interprocessLockCs);
        DeleteCriticalSection(&_interprocessLockTimeoutCs);
        DeleteCriticalSection(&_interprocessUnlockCs);
        DeleteCriticalSection(&_intraprocessLockCs);
        DeleteCriticalSection(&_intraprocessLockTimeoutCs);
        DeleteCriticalSection(&_intraprocessUnlockCs);
        }
    // unlock and close local handles
    _synchMapType::iterator it = _instanceSynch.begin();
    while (it != _instanceSynch.end()) {
        InstanceUnlock(it->first);
        DESTROY_MUTEX(it->second);
        it++;
        }
    _instanceSynch.clear();
    DeleteCriticalSection(&_instanceLockCs);
    DeleteCriticalSection(&_instanceLockTimeoutCs);
    DeleteCriticalSection(&_instanceUnlockCs);
    };

/*
 ____  _        _   _      ___       _ _
/ ___|| |_ __ _| |_(_) ___|_ _|_ __ (_) |_
\___ \| __/ _` | __| |/ __|| || '_ \| | __|
 ___) | || (_| | |_| | (__ | || | | | | |_
|____/ \__\__,_|\__|_|\___|___|_| |_|_|\__|
*/
void CSynch::StaticInit() {
    if (_instances == 0) {
        // initialize intraprocess vars
        InitializeCriticalSection(&_intraprocessLockCs);
        InitializeCriticalSection(&_intraprocessLockTimeoutCs);
        InitializeCriticalSection(&_intraprocessUnlockCs);
        _intraprocessSynch.clear();
        // initialize interprocess vars
        InitializeCriticalSection(&_interprocessLockCs);
        InitializeCriticalSection(&_interprocessLockTimeoutCs);
        InitializeCriticalSection(&_interprocessUnlockCs);
        _interprocessSynch.clear();
        // increase to 1 the number of _instances
        _instances++;
        }
    };



// -------------------------------------------------------------------------------
// ------------------------- INSTANCE LOCK/UNLOCK
// -------------------------------------------------------------------------------

/*
 ___           _                       _               _
|_ _|_ __  ___| |_ __ _ _ __   ___ ___| |    ___   ___| | __
 | || '_ \/ __| __/ _` | '_ \ / __/ _ \ |   / _ \ / __| |/ /
 | || | | \__ \ || (_| | | | | (_|  __/ |__| (_) | (__|   <
|___|_| |_|___/\__\__,_|_| |_|\___\___|_____\___/ \___|_|\_\
*/
/** 
 *  \param mutex_name name of the mutex to lock
 *
 * This method waits for a mutex to be available and then locks it. If the mutex is already locked
 * by another thread then the thread will lock until the mutex is released by the owner thread and
 * the new lock is executed by this thread.
 */
void CSynch::InstanceLock(std::string name) {
    StaticInit();
    EnterCriticalSection(&_instanceLockCs);
    std::string mutex_name = std::string("Local\\") + name;
    MUTEX_TYPE mx;
    // check the existence of the mutex
    _synchMapType::const_iterator it = _instanceSynch.find(mutex_name);
    if (it == _instanceSynch.end()) {
        // mutex does not exist yet, create it
        CREATE_INSTANCE_MUTEX(mx);
        // add mutex to the map
        _instanceSynch[mutex_name] = mx;
        }
    else
        mx = it->second;
    LeaveCriticalSection(&_instanceLockCs);
    // lock mutex
    DWORD res = WAITFORMUTEX(mx,MUTEX_INFINITE_TIMEOUT);
    };

/*
 ___           _                       _               _
|_ _|_ __  ___| |_ __ _ _ __   ___ ___| |    ___   ___| | __
 | || '_ \/ __| __/ _` | '_ \ / __/ _ \ |   / _ \ / __| |/ /
 | || | | \__ \ || (_| | | | | (_|  __/ |__| (_) | (__|   <
|___|_| |_|___/\__\__,_|_| |_|\___\___|_____\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to lock
 *  \param ms timeout in ms
 *  \return success flag
 *
 * This method try to execute a standard lock to the named mutex taking into account the specified 
 * timeout value. \n
 * The return value is:
 * \li \b true if the lock as success
 * \li \b false if timeout occurred
 */
bool CSynch::InstanceLock(std::string name, int ms) {
    StaticInit();
    EnterCriticalSection(&_instanceLockTimeoutCs);
    // check the existence of the mutex
    std::string mutex_name = std::string("Local\\") + name;
    MUTEX_TYPE mx;
    // look for the mutex name
    _synchMapType::const_iterator it = _instanceSynch.find(mutex_name);
    if (it == _instanceSynch.end()) {
        // mutex does not exist yet, create it
        CREATE_INSTANCE_MUTEX(mx);
        // add mutex to the map
        _instanceSynch[mutex_name] = mx;
        }
    else
        mx = it->second;
    LeaveCriticalSection(&_instanceLockTimeoutCs);
    // lock mutex
    THREADAPI_RESULT res = WAITFORMUTEX(mx,ms);
    // check the final result
    if (res != MUTEX_TIMEOUT_CODE)
        return true;
    else
        return false;
    };

/*
 ___           _                       _   _       _            _
|_ _|_ __  ___| |_ __ _ _ __   ___ ___| | | |_ __ | | ___   ___| | __
 | || '_ \/ __| __/ _` | '_ \ / __/ _ \ | | | '_ \| |/ _ \ / __| |/ /
 | || | | \__ \ || (_| | | | | (_|  __/ |_| | | | | | (_) | (__|   <
|___|_| |_|___/\__\__,_|_| |_|\___\___|\___/|_| |_|_|\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to unlock
 *  \return success flag
 *
 * This method unlocks a named mutex immediatly. \n
 * The return value is
 * \li \b true if mutex has been unlocked
 * \li \b false if mutex was not locked or does not exist
 */
bool CSynch::InstanceUnlock(std::string name) {
    StaticInit();
    // check the existence of the mutex
    std::string mutex_name = std::string("Local\\") + name;
    _synchMapType::const_iterator it = _instanceSynch.find(mutex_name);
    if (it != _instanceSynch.end()) {
        // mutex exist, release it
        RELEASEMUTEX(it->second);
        return true;
        } else
            // mutex does not exist
            return false;
    };



// -------------------------------------------------------------------------------
// ------------------------- INTRA-PROCESS LOCK/UNLOCK
// -------------------------------------------------------------------------------

/*
 ___       _             ____                             _               _
|_ _|_ __ | |_ _ __ __ _|  _ \ _ __ ___   ___ ___ ___ ___| |    ___   ___| | __
 | || '_ \| __| '__/ _` | |_) | '__/ _ \ / __/ _ Y __/ __| |   / _ \ / __| |/ /
 | || | | | |_| | | (_| |  __/| | | (_) | (_|  __|__ \__ \ |__| (_) | (__|   <
|___|_| |_|\__|_|  \__,_|_|   |_|  \___/ \___\___|___/___/_____\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to lock
 *
 * This method waits for a mutex to be available and then locks it. If the mutex is already locked
 * by another thread then the thread will lock until the mutex is released by the owner thread and
 * the new lock is executed by this thread.
 */
void CSynch::IntraProcessLock(std::string name) {
    StaticInit();
    EnterCriticalSection(&_intraprocessLockCs);
    std::stringstream mutex_name;
    mutex_name << GetCurrentProcessId() << name;
    MUTEX_TYPE mx;
    // check the existence of the mutex
    _synchMapType::const_iterator it = _intraprocessSynch.find(mutex_name.str());
    if (it == _intraprocessSynch.end()) {
        // mutex does not exist yet, create it
        CREATE_MUTEX(mx, mutex_name.str().c_str());
        // add mutex to the map
        _intraprocessSynch[mutex_name.str()] = mx;
        }
    else
        mx = it->second;
    LeaveCriticalSection(&_intraprocessLockCs);
    // lock mutex
    DWORD res = WAITFORMUTEX(mx,MUTEX_INFINITE_TIMEOUT);
    };

/*
 ___       _             ____                             _               _
|_ _|_ __ | |_ _ __ __ _|  _ \ _ __ ___   ___ ___ ___ ___| |    ___   ___| | __
 | || '_ \| __| '__/ _` | |_) | '__/ _ \ / __/ _ Y __/ __| |   / _ \ / __| |/ /
 | || | | | |_| | | (_| |  __/| | | (_) | (_|  __|__ \__ \ |__| (_) | (__|   <
|___|_| |_|\__|_|  \__,_|_|   |_|  \___/ \___\___|___/___/_____\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to lock
 *  \param ms timeout in ms
 *  \return success flag
 *
 * This method try to execute a standard lock to the named mutex taking into account the specified 
 * timeout value. \n
 * The return value is:
 * \li \b true if the lock as success
 * \li \b false if timeout occurred
 */
bool CSynch::IntraProcessLock(std::string name, int ms) {
    StaticInit();
    EnterCriticalSection(&_intraprocessLockTimeoutCs);
    // check the existence of the mutex
    std::stringstream mutex_name;
    mutex_name << GetCurrentProcessId() << name;
    MUTEX_TYPE mx;
    // look for the mutex name
    _synchMapType::const_iterator it = _intraprocessSynch.find(mutex_name.str());
    
    if (it == _intraprocessSynch.end()) {
        // mutex does not exist yet, create it
        CREATE_MUTEX(mx, mutex_name.str().c_str());
        // add mutex to the map
        _intraprocessSynch[mutex_name.str()] = mx;
        } 
    else
        mx = it->second;
    LeaveCriticalSection(&_intraprocessLockTimeoutCs);
    // lock mutex
    THREADAPI_RESULT res = WAITFORMUTEX(mx,ms);
    // check the final result
    if (res != MUTEX_TIMEOUT_CODE)
        return true;
    else
        return false;
    };

/*
 ___       _             ____                             _   _       _            _
|_ _|_ __ | |_ _ __ __ _|  _ \ _ __ ___   ___ ___ ___ ___| | | |_ __ | | ___   ___| | __
 | || '_ \| __| '__/ _` | |_) | '__/ _ \ / __/ _ Y __/ __| | | | '_ \| |/ _ \ / __| |/ /
 | || | | | |_| | | (_| |  __/| | | (_) | (_|  __|__ \__ \ |_| | | | | | (_) | (__|   <
|___|_| |_|\__|_|  \__,_|_|   |_|  \___/ \___\___|___/___/\___/|_| |_|_|\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to unlock
 *  \return success flag
 *
 * This method unlocks a named mutex immediatly. \n
 * The return value is
 * \li \b true if mutex has been unlocked
 * \li \b false if mutex was not locked or does not exist
 */
bool CSynch::IntraProcessUnlock(std::string name) {
    StaticInit();
    // check the existence of the mutex
    std::stringstream mutex_name;
    mutex_name << GetCurrentProcessId() << name;
    _synchMapType::const_iterator it = _intraprocessSynch.find(mutex_name.str());
    if (it != _intraprocessSynch.end()) {
        // mutex exist, release it
        RELEASEMUTEX(it->second);
        return true;
        } else
            // mutex does not exist
            return false;
    };


// -------------------------------------------------------------------------------
// ------------------------- INTER-PROCESS LOCK/UNLOCK
// -------------------------------------------------------------------------------

/*
 ___       _            ____                             _               _
|_ _|_ __ | |_ ___ _ __|  _ \ _ __ ___   ___ ___ ___ ___| |    ___   ___| | __
 | || '_ \| __/ _ \ '__| |_) | '__/ _ \ / __/ _ Y __/ __| |   / _ \ / __| |/ /
 | || | | | ||  __/ |  |  __/| | | (_) | (_|  __|__ \__ \ |__| (_) | (__|   <
|___|_| |_|\__\___|_|  |_|   |_|  \___/ \___\___|___/___/_____\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to lock
 *
 * This method waits for a mutex to be available and then locks it. If the mutex is already locked
 * by another thread or process then the thread will lock until the mutex is released and
 * the new lock is executed by the caller thread.
 */
void CSynch::InterProcessLock(std::string name) {
    StaticInit();
    EnterCriticalSection(&_interprocessLockCs);
    MUTEX_TYPE mx;
    std::string mutex_name = std::string("Global\\") + name;
    // check the existence of the mutex
    _synchMapType::const_iterator it = _interprocessSynch.find(mutex_name);
    if (it == _interprocessSynch.end()) {
        // mutex does not exist yet, create it
        CREATE_MUTEX(mx,mutex_name.c_str());
        // add mutex to the map
        _interprocessSynch[mutex_name] = mx;
        }
    else
        mx = it->second;
    LeaveCriticalSection(&_interprocessLockCs);
    // lock mutex
    DWORD res = WAITFORMUTEX(mx,MUTEX_INFINITE_TIMEOUT);
    };

/*
 ___       _            ____                             _               _
|_ _|_ __ | |_ ___ _ __|  _ \ _ __ ___   ___ ___ ___ ___| |    ___   ___| | __
 | || '_ \| __/ _ \ '__| |_) | '__/ _ \ / __/ _ Y __/ __| |   / _ \ / __| |/ /
 | || | | | ||  __/ |  |  __/| | | (_) | (_|  __|__ \__ \ |__| (_) | (__|   <
|___|_| |_|\__\___|_|  |_|   |_|  \___/ \___\___|___/___/_____\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to lock
 *  \param ms timeout in ms
 *  \return success flag
 *
 * This method try to execute a standard lock to the named mutex taking into account the specified 
 * timeout value. \n
 * The return value is:
 * \li \b true if the lock as success
 * \li \b false if timeout occurred
 */
bool CSynch::InterProcessLock(std::string name, int ms) {
    StaticInit();
    EnterCriticalSection(&_interprocessLockTimeoutCs);
    // check the existence of the mutex
    MUTEX_TYPE mx;
    std::string mutex_name = std::string("Global\\") + name;
    // look for the mutex name
    _synchMapType::const_iterator it = _interprocessSynch.find(mutex_name);
    if (it == _interprocessSynch.end()) {
        // mutex does not exist yet, create it
        CREATE_MUTEX(mx,mutex_name.c_str());
        // add mutex to the map
        _interprocessSynch[mutex_name] = mx;
        }
    else
        mx = it->second;
    LeaveCriticalSection(&_interprocessLockTimeoutCs);
    // lock mutex
    THREADAPI_RESULT res = WAITFORMUTEX(mx,ms);
    // check the final result
    if (res != MUTEX_TIMEOUT_CODE)
        return true;
    else
        return false;
    };

/*
 ___       _            ____                             _   _       _            _
|_ _|_ __ | |_ ___ _ __|  _ \ _ __ ___   ___ ___ ___ ___| | | |_ __ | | ___   ___| | __
 | || '_ \| __/ _ \ '__| |_) | '__/ _ \ / __/ _ Y __/ __| | | | '_ \| |/ _ \ / __| |/ /
 | || | | | ||  __/ |  |  __/| | | (_) | (_|  __|__ \__ \ |_| | | | | | (_) | (__|   <
|___|_| |_|\__\___|_|  |_|   |_|  \___/ \___\___|___/___/\___/|_| |_|_|\___/ \___|_|\_\
*/
/**
 *  \param name name of the mutex to unlock
 *  \return success flag
 *
 * This method unlocks a named mutex immediatly. \n
 * The return value is
 * \li \b true if mutex has been unlocked
 * \li \b false if mutex was not locked or does not exist
 */
bool CSynch::InterProcessUnlock(std::string name) {
    StaticInit();
    // check the existence of the mutex
    std::string mutex_name = std::string("Global\\") + name;
    _synchMapType::const_iterator it = _interprocessSynch.find(mutex_name);
    if (it != _interprocessSynch.end()) {
        // mutex exist, release it
        RELEASEMUTEX(it->second);
        return true;
        } else
            // mutex does not exist
            return false;
    };


// -------------------------------------------------------------------------------
// ------------------------- EVENTS MANAGEMENT
// -------------------------------------------------------------------------------

/*
 _   _       _   _  __       _____                 _
| \ | | ___ | |_(_)/ _|_   _| ____|_   _____ _ __ | |_
|  \| |/ _ \| __| | |_| | | |  _| \ \ / / _ \ '_ \| __|
| |\  | (_) | |_| |  _| |_| | |___ \ V /  __/ | | | |_
|_| \_|\___/ \__|_|_|  \__, |_____| \_/ \___|_| |_|\__|
                       |___/
*/
/**
 *  \param name name of the event to notify
 *
 * This method wakes up all the threads waiting for this event.
 */
void CSynch::NotifyEvent(std::string name) {
    StaticInit();
    IntraProcessLock("CSynch_event");
    // check the existence of an event
    EVENT_TYPE event_tmp;
    _eventMapType::iterator it = _events.find(name);
    if (it == _events.end()) {
        // event does not exist, create it
        event_tmp = CreateEvent(NULL,  FALSE, FALSE, NULL);
        _events[name] = event_tmp;
        }
    PulseEvent(_events[name]);
    IntraProcessUnlock("CSynch_event");
    };

/*
__        __    _ _   _____                 _
\ \      / /_ _(_) |_| ____|_   _____ _ __ | |_
 \ \ /\ / / _` | | __|  _| \ \ / / _ \ '_ \| __|
  \ V  V / (_| | | |_| |___ \ V /  __/ | | | |_
   \_/\_/ \__,_|_|\__|_____| \_/ \___|_| |_|\__|
*/
/**
 *  \param name name of the event to wait for
 *
 * This method waits for an event to be notified witout any timeout. Some
 * thread has to ensure the notify of the event to avoid deadlock.
 */
void CSynch::WaitEvent(std::string name) {
    StaticInit();
    //check the existence of the event
    IntraProcessLock("CSynch_event");
    _eventMapType::iterator it = _events.find(name);
    if (it == _events.end()) {
        // event does not exist, create it
        EVENT_TYPE event_tmp;
        event_tmp = CreateEvent(NULL,  FALSE, FALSE, NULL);
        _events[name] = event_tmp;
        it = _events.find(name);
        }
    MUTEX_TYPE mx = it->second;
    IntraProcessUnlock("CSynch_event");
    // wait for the event to be notified
    WAITFORMUTEX(mx, MUTEX_INFINITE_TIMEOUT);
    };

/*
__        __    _ _   _____                 _
\ \      / /_ _(_) |_| ____|_   _____ _ __ | |_
 \ \ /\ / / _` | | __|  _| \ \ / / _ \ '_ \| __|
  \ V  V / (_| | | |_| |___ \ V /  __/ | | | |_
   \_/\_/ \__,_|_|\__|_____| \_/ \___|_| |_|\__|
*/
/**
 *  \param name name of the event to wait for
 *  \param ms timeout in ms
 *  \return notify flag
 *
 * This method waits for an event to be notified \n
 * The return value is
 * \li \b true if signal has been notified
 * \li \b false if timeout occurred
 */
bool CSynch::WaitEvent(std::string name, int ms) {
    StaticInit();
    // check the existence of the event
    IntraProcessLock("CSynch_event");
    _eventMapType::iterator it = _events.find(name);
    if (it == _events.end()) {
        // event does not exist, create it
        EVENT_TYPE event_tmp;
        event_tmp = CreateEvent(NULL,  FALSE, FALSE, NULL);
        _events[name] = event_tmp;
        it = _events.find(name);
        }
    MUTEX_TYPE mx = it->second;
    IntraProcessUnlock("CSynch_event");
    // wait for the event to be notified
    THREADAPI_RESULT res = WAITFORMUTEX(mx, ms);
    // check the final result
    if (res != MUTEX_TIMEOUT_CODE)
        return true;
    else
        return false;
    };

/*
 ____                   _  __        ___ _   _     _____
/ ___| _   _ _ __   ___| |_\ \      / (_) |_| |__ |  ___| __ ___  __ _ _   _  ___ _ __   ___ _   _
\___ \| | | | '_ \ / __| '_ \ \ /\ / /| | __| '_ \| |_ | '__/ _ \/ _` | | | |/ _ \ '_ \ / __| | | |
 ___) | |_| | | | | (__| | | \ V  V / | | |_| | | |  _|| | |  __/ (_| | |_| |  __/ | | | (__| |_| |
|____/ \__, |_| |_|\___|_| |_|\_/\_/  |_|\__|_| |_|_|  |_|  \___|\__, |\__,_|\___|_| |_|\___|\__, |
       |___/                                                        |_|                      |___/
*/
/**
 *  \param frequency requested frequency synchronization
 *
 * This method waits the necessary time from the previous call to ensure that a given period.
 */   
void CSynch::SynchWithFrequency(float frequency) {

    // actual time value
    DWORD actualTime;

    // frequency must be positive
    if ( frequency > 0.0 ) {    
        // init data in the first call
        if (_SWFFirstPass) {
            _SWFLastCALLTime = timeGetTime();
            _SWFFirstPass = false;
            }

        DWORD period_ms = DWORD(1000.0/frequency);
        // get the actual time
        actualTime = timeGetTime();
        // wait the necessary time in ms
        if ( actualTime - _SWFLastCALLTime < period_ms ) {
            // sleep the necessary time
            DWORD sleep_time = period_ms - (actualTime - _SWFLastCALLTime);
            Sleep( sleep_time );
            }
        // restart to count from now!
        _SWFLastCALLTime = timeGetTime();
        }
    else
        _SWFFirstPass = true;

    }