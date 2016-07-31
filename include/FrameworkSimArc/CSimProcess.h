#pragma once

#include "FrameworkSimArc/common_defines.h"
#include "FrameworkSimArc/SimArcDefines.h"

// Framework
#include "FrameworkCore/CSynch.h"
#include "FrameworkCore/CError.h"
#include "FrameworkCore/CThread.h"
#include "FrameworkCore/CGenericType.h"
#include "FrameworkCore/CTime.h"
#include "FrameworkCore/CTrace.h"
#include "FrameworkIPC/CIPC.h"

/** \brief Simulation architecture namespace.                                                               */
namespace FrameworkSimArc {

    /** \brief Process delegate class.
     *
     *  This class defines the virtual methods a class must implement to be the delegate of the CSimProcess
     * class.
     */
    class framework_simarc_class CSimProcessDelegate {
    protected:
        /** \brief Standard process trace stream.                                       */
        FrameworkCore::CTrace          *_trace;

        /** \brief Process state data.                                          */
        FrameworkCore::CGenericType     _processData;

    public:

        // -------------------------  Process Implementation -------------------
        virtual bool Initialize()   {return true;}  /**< \brief Process initialization. */
        virtual bool Run()          {return true;}  /**< \brief Process run.            */
        virtual bool Pause()        {return true;}  /**< \brief Process pause.          */
        virtual bool Stop()         {return true;}  /**< \brief Process stop.           */
        virtual bool Shutdown()     {return true;}  /**< \brief Process shutdown.       */

        virtual bool IsGoingToStart()   {return true;}
        virtual bool IsGoingToPause()   {return true;}
        virtual bool IsGoingToResume()  {return true;}
        virtual bool IsGoingToStop()    {return true;}

        void AssignStream(FrameworkCore::CTrace *trace) {_trace = trace;}
        void SetProcessData(FrameworkCore::CGenericType &data) {_processData = data;}
    };

    /** \brief Simulation Process class.
     *
     * Each process must use these class methods to fit correctly to the the simulation framework. Using 
     * this class a process signals its state, exchanges data between processes, receives the simulation
     * control commands and let the other processes to know its own status.
     */
    class framework_simarc_class CSimProcess :    public    FrameworkCore::CError
                                                , private   FrameworkCore::CThread {

    private:
        
        // --------------------------       Members        --------------------

        /** \brief The process state.                                           */
        e_ProcessState                  _processState;

        /** \brief Synchronization object.                                      */
        FrameworkCore::CSynch           _synch;

        /** \brief Time mnagement for automatic frequency computation.          */
        FrameworkCore::CTime            _time;

        /** \brief Trace object.                                                */
        FrameworkCore::CTrace          *_trace;

        /** \brief WatchDog thread synchronization object.                      */
        FrameworkCore::CSynch           _watchdogSynch;

        /** \brief Process frequency of pulses.                                 */
        double _pulseFrequency;

        /** \brief Process required control state.                              */
        FrameworkCore::CGenericType     _processControlState;

        /** \brief Process delegate class.                                      */
        CSimProcessDelegate            *_processDelegate;

        // --------------------------       Methods         --------------------

        /** \brief Send a pulse for the frequency computation.                  */
        void Pulse();

    protected:

        /** \brief Process state data.                                          */
        FrameworkCore::CGenericType _processData;

        /** \brief Inter-process comunication object.                           */
        FrameworkIPC::CIPC          _ipc;

        // -------------------------- State management --------------------------
        /** \brief Set the process state.                                       */
        void SetState(e_ProcessState state);

    protected:

        /** \brief Threaded method.                                             */
        virtual void ThreadedMethod(std::string thread_name, void *thread_params = NULL);

    public:

        /** \brief Default constructor.                                         */
        CSimProcess();
        /** \brief Constructor with given process name.                         */
        explicit CSimProcess(std::string processName);
        /** \brief Default destructor.                                          */
        ~CSimProcess();
        
        /** \brief Assign the delegate class.                                   */
        void AssignDelegate(CSimProcessDelegate *delegateProcess);

        /** \brief Get the actual process state.                                */
        e_ProcessState GetState();

        /** \brief Get a description of the state.                              */
        static std::string GetStateDesc(e_ProcessState state);

        /** \brief Execute the process task.                                    */
        bool Execute();

        /** \brief Execute the process in a separate thread.                    */
        void ExecuteInAThread();

    };

}