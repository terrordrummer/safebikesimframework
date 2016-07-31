#pragma once

#include "FrameworkSimArc/common_defines.h"
#include "FrameworkSimArc/SimArcDefines.h"
#include <map>

// Framework
#include "FrameworkCore/CError.h"
#include "FrameworkCore/CThread.h"
#include "FrameworkCore/CSynch.h"
#include "FrameworkCore/CTime.h"
#include "FrameworkCore/CGenericType.h"
#include "FrameworkIPC/CIPC.h"

namespace FrameworkSimArc {

    /** \brief Simulation Control class.
     *
     * This class allows to control the simulation i.e. start, pause and stop it. Moreover, using this class
     * it's possible to retrive information about the running processes and their actual state.
     */
    class framework_simarc_class CSimControl :    public    FrameworkCore::CError
                                                , protected FrameworkCore::CThread {

    private:

        /** \brief Container of the active processes in the simulation.             */
        FrameworkCore::CGenericType                     _activeProcesses;

        /** \brief Timeout counter of the active processes in the simulation.       */
        std::map<std::string, FrameworkCore::CTime>     _activeProcessesTimeouts;

        /** \brief Inter-Process comunication object.                               */
        FrameworkIPC::CIPC                              _ipc;

        /** \brief Synchronization objects.                                         */
        FrameworkCore::CSynch                           _dataSynch;
        FrameworkCore::CSynch                           _processCleanerSynch;

        /** \brief Received multicast data.                                         */
        FrameworkCore::CGenericType                     _multicastData;

        /** \brief Simulation state required.                                       */
        FrameworkCore::CGenericType                     _simStateControl;

        /** \brief Control state FLAG. It's TRUE if the required state has been set.*/
        bool                                            _controlStateSet;


    protected:

        /** \brief Threaded method.                                                 */
        virtual void ThreadedMethod(std::string thread_name, void *thread_params = NULL);

    public:

        /** \brief Default constructor.                                             */
        CSimControl();
        /** \brief Default destructor.                                              */
        ~CSimControl();

        // ----------------------- SIMULATION MANAGEMENT -----------------------
        /** \brief Enable the simulation control.                                   */
        void EnableSimControl();
        /** \brief Disable the simulation control.                                  */
        void DisableSimControl();
        /** \brief Start the simulation.                                            */
        void Start();
        /** \brief Stop the simulation.                                             */
        void Stop();
        /** \brief Pause the simulation.                                            */
        void Pause();
        /** \brief Terminate the simulation and wait all processes to terminate.    */
        void Shutdown();
        /** \brief Resets all processes running in the simulation.                  */
        void Reset();
        
        // -----------------------  PROCESS MANAGEMENT  -----------------------
        /** \brief Start the given process.                                         */
        void StartProcess(std::string name);
        /** \brief Stop the given process.                                          */
        void StopProcess(std::string name);
        /** \brief Pause the given process.                                         */
        void PauseProcess(std::string name);
        /** \brief Shutdown the given process.                                      */
        void ShutdownProcess(std::string name);
        /** \brief Reset the given process.                                         */
        void ResetProcess(std::string name);

        // ----------------------- SIMULATION INSPECTION -----------------------
        /** \brief Get the list of active processes                                 */
        void GetProcessList(FrameworkCore::CGenericType &data);
        /** \brief Get the data of the given process.                               */
        void GetProcessData(FrameworkCore::CGenericType &data);

    };
}