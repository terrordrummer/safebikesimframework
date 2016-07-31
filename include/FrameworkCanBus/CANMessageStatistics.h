#pragma once

#include "FrameworkCore/CTime.h"
#include "FrameworkCanBus/CANBasicTypes.h"

namespace FrameworkCanBus {
    
    /** \brief This class defines the statistics of a transmitted message.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * Message's statistics container. Available statistics are:
     *
     * \li the times the message was transmitted on the CANBUS
     * \li the average transmission time
     */
    class framework_canbus_class CANMessageStatistics {
        
    public:

        /** \brief Number of times the message was transmitted                           */
        unsigned long                   n_transmissions;

        /** \brief Mean time between two messages (windowed).                            */
        FrameworkCore::CTime::time_type t_mean;

        /** \brief Class constructor.                                                    */
        CANMessageStatistics() : n_transmissions(0), t_mean(0) {};
        };
    
    }