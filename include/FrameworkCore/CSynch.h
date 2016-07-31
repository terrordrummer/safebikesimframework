#pragma once

#include <map>
#include <string>
#include "FrameworkCore/ThreadMultiplatformMacros.h"
#include "FrameworkCore/common_defines.h"

namespace FrameworkCore {

    /** \brief This class implements basic synchronization methods for multithread and multiprocess synchronization.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * Synchronization can be done at three levels: <b>instance</b>, <b>intra-process</b> and <b>inter-process</b>
     * level.
     *
     * \section _instanceSynch Instance level
     * At this level this class must be instantiated to a variable and this variable will be used to lock/unlock mutex.
     * Here is an example:
     *
     * \code
     * DWORD WINAPI threadedProcedure( __in params) {
     * CSynch synch;
     * ...
     * #pragma omp parallel private(tid) { // function is splitted in a set of threads sharing the same CSynch instance
     *   ...
     *   synch.InstanceLock("myVar");     // myVar name scope is associated to the instance of synch var
     *   myVar[3] = myVar[2] + myVar[1];  // splitted threads will have exclusive access to myVar
     *   synch.InstanceUnlock("myVar");   // unlock myVar mutex
     *   ...
     *   synch.InstanceLock("myVar");     // myVar name scope is associated to the instance of synch var
     *   myVar[1] = myVar[3];             // splitted threads will have exclusive access to myVar
     *   synch.InstanceUnlock("myVar");   // unlock myVar mutex
     *   ...
     * }
     * \endcode
     *
     * This example shows how instance level works. The threadedProcedure is called, the synch variable is created and shared between
     * several other threads thanks to the OpenMP directive <b>parallel</b>, these threads will have synchronous access to
     * the myVar variable because they share the same synch instance variable and the same mutex name.
     * If the procedure is called in a parallel thread then a different instance of the synch variable is created and a new set of threads
     * sharing this variable is created. They have again sinchronous access to myvar each other but they will not be synchronized with the
     * first set of threads because they're not referring to the same synch variable instance.
     *
     * \section intra-process_mutexes Intra-process level
     * IntraProcessLock and IntraProcessUnlock are class-methods and will share the mutex names between all
     * the threads within the same process. \n
     * An example for a safe use of a shared variable is the following:
     *
     * \code
     * CSynch::IntraProcessLock("myvar");   // guarantee that only one thread at the time will lock the "myvar" mutex
     * myvar[3] = myvar[2] + myvar[1];      // use myvar
     * CSynch::IntraProcessUnlock("myvar"); // release the "myvar" mutex allowing other threads to execute the next lock
     * \endcode
     *
     * This code can be shared between threads and it will be safe because it will access the myvar exclusively.
     *
     * \section iter-process_mutexes Inter-process level
     * InterProcessLock and InterProcessUnlock are class-level methods which shares mutex names betrweel all the 
     * threads of all the running processes. Use these functions if there is the need to synchronize threads
     * of different processes eachother. As an example see the following:
     *
     * \code
     * CSynch::InterProcessLock("myvar");           // guarantee that only one thread at the time will lock the "myvar" mutex
     * ofstream ofs("data.log",ios_base::append);   // append log data to file
     * ofs << "time: " << timeGetTime() << endl; 
     * ofs.close();
     * CSynch::InterProcessUnlock("myvar");         // release the "myvar" mutex allowing other threads to execute the next lock
     * \endcode
     *
     * Running several processes containing this code will produce the data.log file containing an ordinated 
     * sequence of time measurement inside the file.
     *
     * \section events_synch Events
     * The event management works with the standard wait/notify paradigm i.e. any thread waiting for an event
     * will be awaked by some other thread which notifies the same event:
     *
     * \code
     * // Thread 1
     * ...
     * CSynch::WaitEvent("event_1"); // wait the "event_1" to be notified by someone
     * cout << "event_1 notified!";
     * ...
     *
     * // Thread 2
     * ...
     * CSynch::NotifyEvent("event_1");  // notify the event "event_1" to awake Thread 1 if it is waiting for it
     * ...
     * \endcode
     *
     * A maximum timeout can be specified for the functions which will wait for a lock to occur or an event to be
     * notified. The scope of the event names is inter-process. No synchronization between different processes
     * is implemented.
     *
     */
    class framework_core_class CSynch {
    private:
        /** \brief This map access the mutex variable using a string name.   */
        typedef std::map<std::string, MUTEX_TYPE> _synchMapType;

        /** \brief This map access the event variable using a string name.   */
        typedef std::map<std::string, EVENT_TYPE> _eventMapType;

        /** \brief Map of _events by string name.                             */
        _eventMapType _events;

        /** \brief Class _instances counter.
         *
         * This variable counts the number of existing _instances of the class
         */
        static int _instances;
        
        // -------------------------------------------------------------------------------
        // ------------------------- INSTANCE MEMBERS
        // -------------------------------------------------------------------------------

        /** \brief Map of mutexes by string name.                    */
        _synchMapType _instanceSynch;

        /** \brief IntraProcessLock critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        CRITICAL_SECTION _instanceLockCs;

        /** \brief Lock_timeout critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        CRITICAL_SECTION _instanceLockTimeoutCs;

        /** \brief IntraProcessUnlock critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        CRITICAL_SECTION _instanceUnlockCs;

        // -------------------------------------------------------------------------------
        // ------------------------- INTRA-PROCESS MEMBERS
        // -------------------------------------------------------------------------------
        
        /** \brief Map of mutexes by string name.                            */
        static _synchMapType _intraprocessSynch;

        /** \brief IntraProcessLock critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        static CRITICAL_SECTION _intraprocessLockCs;

        /** \brief Lock_timeout critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        static CRITICAL_SECTION _intraprocessLockTimeoutCs;

        /** \brief IntraProcessUnlock critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        static CRITICAL_SECTION _intraprocessUnlockCs;

        // -------------------------------------------------------------------------------
        // ------------------------- INTER-PROCESS MEMBERS
        // -------------------------------------------------------------------------------

        /** \brief Map of mutexes by string name.                    */
        static _synchMapType _interprocessSynch;

        /** \brief IntraProcessLock critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        static CRITICAL_SECTION _interprocessLockCs;

        /** \brief Lock_timeout critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        static CRITICAL_SECTION _interprocessLockTimeoutCs;

        /** \brief IntraProcessUnlock critical section var.
         *
         * For multithreading issues, some methods have some lines of code are defined in a critical sction
         */
        static CRITICAL_SECTION _interprocessUnlockCs;

        // -------------------------------------------------------------------------------
        // -------------------- SYNCHRONIZATION WITH FREQUENCY VARS
        // -------------------------------------------------------------------------------
        /** \brief Store the last time the SynchWithFrequency method was called.                */
        DWORD _SWFLastCALLTime;

        /** \brief Store if the SynchWithFrequency method has to be initialized.                */
        bool _SWFFirstPass;

    private:
        /** \brief Initialization function managing the static instance.
         *
         * This method is used to manage the singleton behaviour of the class. A static counter stores the number
         * of existing _instances: the class is initialized only the first time and next a reference pointer will
         * be returned as usual.
         */
        static void StaticInit();


    public:        
        /** \brief Default constructor.  */
        explicit CSynch();

        /** \brief Default destructor.   */
        virtual ~CSynch();

        
        // -------------------------------------------------------------------------------
        // ------------------------- INSTANCE LOCK/UNLOCK
        // -------------------------------------------------------------------------------

        /** \brief IntraProcessLock a mutex scoped by the class instance.                                       */
        void InstanceLock(std::string mutex_name);

        /** \brief IntraProcessLock a mutex scoped by the class instance waiting for a maximum timeout.         */
        bool InstanceLock(std::string mutex_name, int timeout_ms);

        /** \brief IntraProcessUnlock a mutex scoped by the class instance.                                     */
        bool InstanceUnlock(std::string mutex_name);

        // -------------------------------------------------------------------------------
        // ------------------------- INTRA-PROCESS LOCK/UNLOCK
        // -------------------------------------------------------------------------------

        /** \brief IntraProcessLock a mutex scoped inside the process.                                          */
        static void IntraProcessLock(std::string mutex_name);

        /** \brief IntraProcessLock a mutex scoped inside the process waiting for a maximum timeout.            */
        static bool IntraProcessLock(std::string mutex_name, int timeout_ms);

        /** \brief IntraProcessUnlock a mutex scoped inside the process.                                        */
        static bool IntraProcessUnlock(std::string mutex_name);

        // -------------------------------------------------------------------------------
        // ------------------------- INTER-PROCESS LOCK/UNLOCK
        // -------------------------------------------------------------------------------

        /** \brief IntraProcessLock a mutex with inter-process scope.                                           */
        static void InterProcessLock(std::string mutex_name);

        /** \brief IntraProcessLock a mutex with inter-process scope waiting for a maximum timeout.             */
        static bool InterProcessLock(std::string mutex_name, int timeout_ms);

        /** \brief IntraProcessUnlock a mutex with inter-process scope.                                         */
        static bool InterProcessUnlock(std::string mutex_name);
       
        // -------------------------------------------------------------------------------
        // ------------------------- EVENTS MANAGEMENT
        // -------------------------------------------------------------------------------

        /** \brief Notify an event.                                                                             */
        void NotifyEvent(std::string event_name);

        /** \brief Wait for an event to be raised.                                                              */
        void WaitEvent(std::string event_name);

        /** \brief Wait for an event to be raised with a maximum timeout.                                       */
        bool WaitEvent(std::string event_name, int timeout_ms); 

        /** \brief Static data initializer.                                                                     */
        static void staticInitializer();

        // -------------------------------------------------------------------------------
        // ------------------------- FREQUENCY SYNCHRONIZATION
        // -------------------------------------------------------------------------------
        /** \brief Wait the necessary time from the previous call to fit the given frequency.                   */        
        void SynchWithFrequency(float frequency);

        };

    }