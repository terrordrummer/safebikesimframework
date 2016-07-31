#pragma once

#include <map>
#include <set>
#include <string>
#include "FrameworkCore/common_defines.h"

namespace FrameworkCore {

    /** \brief This class manages multi-threading.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * <b>USAGE</b>\n
     * User that wants to run some code in a thread should:
     * \li create a new class that inherits the CThread class
     * \li specialize the ThreadedMethod() method which will contain the threaded code
     * \li launch the thread by calling the launching methods
     *
     * <b>RUN A THREAD</b>\n
     * To launch a thread the user can call one of the following methods:
     * \li ThreadRunOnce()
     * \li ThreadRun()
     *
     * The ThreadRunOnce() method runs the ThreadedMethod() just once.
     * The ThreadRun() method calls ThreadedMethod() ciclically until a ThreadTermination() or 
     * ThreadTerminationRequest() is called.
     *
     * Each time a thread is runned the thread name has to be specified and will appear as a
     * parameter of the threaded method.
     * \n
     * Multiple threads are runnable by calling ThreadRunOnce() or ThreadRun() with different
     * thread names.
     *
     * <b>TERMINATE A CICLIC THREAD</b>\n
     * To terminate a ciclic thread the user can call
     * \li ThreadTerminate()
     * \li ThreadTerminationRequest()
     *
     * the difference is that the first waits the thread to terminate, the second will request
     * the thread to terminate only but will not wait it to terminate.
     *
     * <b>EXAMPLE</b>\n
     *  \code
     *  void main() {
     *
     *      char mydata[1024];
     *
     *      MyClass Thread;
     *      // launch some threads
     *      Thread.ThreadRunOnce("Thread_1",(void *)mydata);    // run ONCE
     *      Thread.ThreadRun("Thread_2",(void *)mydata);        // run CICLICALLY
     *      Thread.ThreadRun("Thread_3",NULL);                  // run CICLICALLY
     *
     *      // do something....
     *
     *      // ciclic threads has to be terminated at the end
     *      Thread.ThreadTerminate("Thread_2");
     *      Thread.ThreadTerminate("Thread_3");
     *  }
     *  \endcode
     *
     * The ThreadedMethod should switch on the thread name to implement different threaded
     * functions:
     *
     *  \code
     *
     *  class MyClass : public CThread {
     *      virtual void ThreadedMethod(std::string thread_name, void *thread_params);
     *  }
     *
     *  void MyClass::ThreadedMethod(std::string thread_name, void *thread_params) {
     *      switch(thread_name) {
     *          case("Thread_1"):   // Thread_1 code accessing the mydata var
     *
     *              // do something relevant for thread 1
     *              printf("thread 1 param: %s\n",(char *)thread_params);
     *              
     *              break;
     *
     *          case("Thread_2"):   // Thread_2 code accessing the mydata var
     *              
     *              // do something relevant for thread 2
     *              printf("thread 2 param: %s\n",(char *)thread_params);
     *
     *              break;
     *
     *          case("Thread_3"):   // Thread_3 code with NULL params
     *              
     *              // do something relevant for thread 3
     *              printf("Thread 3\n");
     *
     *              break;
     *          }
     *  }
     *  \endcode
     * 
     * In this example, Thread_1 is executed just once while Thread_2 and Thread_3 are executed cyclically until
     * the ThreadTerminate() is called. \n
     * <b>Notice that ThreadRun() requires ThreadTerminate() or ThreadTerminationRequest() to be called to
     * stop the threaded cyclic call to ThreadedMethod().</b>
     */
    class framework_core_class CThread {
    private:
        /** \brief Map between thread names and termination request flags.
         *
         * When a thread is requested to terminate its termination flag is setted.
         */
        std::map<std::string,bool>  _terminationRequest;

        /** \brfief Map between thread name and thread ID.
         *
         * This map is used to check the thread status at OS level supposing that a thread can crash without 
         * reset its data correctly.
         */
        std::map<std::string, unsigned long long>    _threadIDs;

        /** \brief List of active thread names.
         *
         * When a thread starts its name is inserted in this list until it terminates.
         */
        std::set<std::string>       _runningThreads;  

        /** \brief This is a temporary variable containing the pointer to the thread's parameters.
         *
         * The thread launching sequence stores temporairly the thread's parameters pointer in this variable
         * until the thread is started.
         */
        void*                       _launchingThreadParams;        

        /** \brief Name identifier of the thread that is going to be launched.
         *
         * The thread launching sequence stores temporairly the thread's name poter in this variable
         * until the thread is started.
         */
        std::string                 _launchingThreadName;    

        /** \brief Thread launching flag.
         *
         * This flag is true when a thread is launched and it's set to false when thread is launched.
         */
        bool                        _threadLaunchFlag;
        
    protected:    
        /** \brief Thread cycle management method.                                                          */
        void RunManager();

        /** \brief Threaded method.                                                                         */
        virtual void ThreadedMethod(std::string thread_name, void *thread_params = NULL);

    public:        
        // ------------------------ constructor/destructor
        CThread();              /**< \brief Default constructor.                                            */
        virtual ~CThread();     /**< \brief Default desconstructor.                                         */
        
        /** \brief Run ThreadedMethod just once.                                                            */
        void ThreadRunOnce(std::string thread_name, void *params = NULL);

        /** \brief Run ThreadedMethod ciclically until termination is explicitly called.                    */
        void ThreadRun(std::string thread_name, void *params = NULL);

        /** \brief Set the thread's termination flag and exit immediatly.                                   */
        void ThreadTerminationRequest(std::string thread_name);

        /** \brief Set the thread's termination flag and wait it to terminate.                              */
        void ThreadTerminate(std::string thread_name);

        /** \brief Check if a thread is running.                                                            */
        bool ThreadIsRunning(std::string thread_name);      
        };
    }