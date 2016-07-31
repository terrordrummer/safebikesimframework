#pragma once

namespace FrameworkSimArc {

    /** \brief Process state enumeration list.                                                                  */
    typedef enum {
        PROCESS_STATE_LOADED        = 0,  /**< \brief The process is loaded and not yet initialized.            */
        PROCESS_STATE_ININTIALIZING = 1,  /**< \brief The process is initializing.                              */
        PROCESS_STATE_STOPPED       = 2,  /**< \brief The process is stopped.                                   */
        PROCESS_STATE_GOINGTOSTART  = 3,  /**< \brief The process is going to start.                            */
        PROCESS_STATE_RUNNING       = 4,  /**< \brief The process is running.                                   */
        PROCESS_STATE_GOINGTOPAUSE  = 5,  /**< \brief The process is going to be paused.                        */
        PROCESS_STATE_PAUSED        = 6,  /**< \brief The process is in pause.                                  */
        PROCESS_STATE_GOINGTORESUME = 7,  /**< \brief The process is going to be resumed.                       */
        PROCESS_STATE_GOINGTOSTOP   = 8,  /**< \brief The process is going to be stopped.                       */
        PROCESS_STATE_SHUTTINGDOWN  = 9,  /**< \brief The process is going to be paused.                        */
        PROCESS_STATE_ERROR         = 10, /**< \brief The process is locked in an error state.                  */
        PROCESS_STATE_KILLED        = 11, /**< \brief The process has been killed.                              */
        PROCESS_STATE_NOTRESPONDING = 12  /**< \brief The process it is not responding.                         */
    } e_ProcessState;

    /** \brief Simulation state enumeration list.                                                               */
    typedef enum {
        PROCESS_CONTROL_STOP        = 0,    /**< \brief The process is requested to be stopped.                 */
        PROCESS_CONTROL_START       = 1,    /**< \brief The process is requested to be started.                 */
        PROCESS_CONTROL_PAUSE       = 2,    /**< \brief The process is requested to be paused.                  */
        PROCESS_CONTROL_SHUTDOWN    = 3,    /**< \brief The process is requested to be killed.                  */
        PROCESS_CONTROL_RESET       = 4     /**< \brief The process is requested to be resetted.                */
    } e_ProcessControl;


    // IPC channel names
#define PROCESS_STATE_CHANNEL_NAME          "PROCESS_STATE_CHANNEL_NAME"
#define PROCESS_CONTROL_CHANNEL_NAME        "PROCESS_CONTROL_CHANNEL_NAME"

    // Conversion between CGenericType and e_SimulationState macros
#define CGENERICTYPE_TO_PROCESS_STATE(x) (e_ProcessState)(int)floor((x).toNumber())
#define PROCESS_STATE_TO_CGENERICTYPE(x) int(x)

#define CGENERICTYPE_TO_PROCESS_CONTROL(x) (e_ProcessControl)(int)floor((x).toNumber())
#define PROCESS_CONTROL_TO_CGENERICTYPE(x) int(x)

#define PROCESS_TIMEOUT     1.0 // process heartbeat timeout (in seconds)
}