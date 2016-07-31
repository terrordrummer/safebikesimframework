#pragma once


#include "FrameworkCanBus/CANBasicTypes.h"
#include <map>
#include <set>

namespace FrameworkCanBus {

    /** \brief CAN message specific types. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     * 
     * This class contains all the non-basic types of the message properties.
     */
    class framework_canbus_class CANMessageTypes {
    public:
        /** \brief Message type enumeration.   */
        typedef enum {
            MSG_STANDARD,   /**< \brief Standard message format */
            MSG_EXTENDED    /**< \brief Extended message format */
            } MessageType;

        /** \brief Map type from a signal's name to its starting bit inside the message. */
        typedef std::map<CANBasicTypes::type_string, CANBasicTypes::type_integer> signal_starting_bit_type;

        /** \brief Set type containing the list of signals packed in the message.        */
        typedef std::set<CANBasicTypes::type_string> signal_set_type;
        
        };
    }