#pragma once;

#include "CANBasicTypes.h"
#include <set>

namespace FrameworkCanBus {

    /** \brief ECU class specific types. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class contains some specific types used by CANEcu class.
     */
    class framework_canbus_class CANEcuTypes {
    public:
        /** \brief Type of the list which contains the list of transmitted and received message by an ECU.   */
        typedef std::set<CANBasicTypes::type_uinteger> msg_id_set_type;

        /** \brief Type of the list which contains the list of transmitted and received signals by an ECU.   */
        typedef std::set<CANBasicTypes::type_string> signal_set_type;

        /** \brief Type of the list of ecu name.                                                        */
        typedef std::set<CANBasicTypes::type_string> ecu_list_type;
        
        };

    }