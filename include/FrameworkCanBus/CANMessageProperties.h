#pragma once

#include "CANBasicClass.h"
#include "CANMessageTypes.h"

namespace FrameworkCanBus {

    /** \brief CAN message properties descriptor. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     * 
     * This class defines the list of properties of a CAN message which are:
     * \li its identifier
     * \li its lexical name
     * \li type of message (standard/extended)
     * \li number of bytes (0..8)
     * \li by which ECU is transmitted
     * \li an optional arthitecture developer's comment
     *
     */
    class framework_canbus_class CANMessageProperties : public CANBasicClass {
    private:
        CANBasicTypes::type_uinteger   _id;            /**< \brief Message's ID.                                 */
        CANBasicTypes::type_string     _name;          /**< \brief Message's name.                               */
        CANMessageTypes::MessageType   _messageType;   /**< \brief Message type.                                 */
        CANBasicTypes::type_uinteger   _dlc;           /**< \brief Message's data length.                        */
        CANBasicTypes::type_string     _transmitter;   /**< \brief Name of the ECU that transmits the message.   */
        CANBasicTypes::type_float      _cycleTime;     /**< \brief Message's cycle time (in seconds).            */
        CANBasicTypes::type_string     _comment;       /**< \brief Message's comment.                            */

    public:

        /** \brief Class constructor.           */
        CANMessageProperties(  CANBasicTypes::type_uinteger   _id            = 0, 
                               CANBasicTypes::type_string     _name          = "", 
                               CANMessageTypes::MessageType   _messageType  = CANMessageTypes::MSG_STANDARD,
                               CANBasicTypes::type_uinteger   _dlc           = 0,
                               CANBasicTypes::type_string     _transmitter   = "",
                               CANBasicTypes::type_float      _cycleTime    = 0      );

        /** \brief Returns message's id.                    */
        CANBasicTypes::type_uinteger get_id() {return _id;};                       
        /** \brief Returns message's name.                  */
        CANBasicTypes::type_string   get_name() {return _name;};                    
        /** \brief Returns message's type.                  */
        CANMessageTypes::MessageType get_message_type() {return _messageType;};   
        /** \brief Returns message's dlc.                   */
        CANBasicTypes::type_uinteger get_dlc() {return _dlc;};                     
        /** \brief Returns message's transmitter.           */
        CANBasicTypes::type_string   get_transmitter() {return _transmitter;};     
        /** \brief Returns message's cycle_time.            */
        CANBasicTypes::type_float    get_cycle_time() {return _cycleTime;};  
        /** \brief Returns message's comment.               */
        CANBasicTypes::type_string   get_comment() {return _comment;};

        bool set_id(CANBasicTypes::type_uinteger id);                       /**< \brief Sets message's id.          */
        bool set_name(CANBasicTypes::type_string name);                     /**< \brief Sets message's name.         */
        bool set_message_type(CANMessageTypes::MessageType message_type);   /**< \brief Sets message's type.         */
        bool set_dlc(CANBasicTypes::type_uinteger dlc);                     /**< \brief Sets message's dlc.          */
        bool set_transmitter(CANBasicTypes::type_string transmitter);       /**< \brief Sets message's transmitter.  */
        bool set_cycle_time(CANBasicTypes::type_float cycle_time);          /**< \brief Sets message's cycle_time.   */
        bool set_comment(CANBasicTypes::type_string);                       /**< \brief Sets message's comment.      */

        /** \brief Sets all the properties of the message.       */
        bool set_properties(CANMessageProperties *prop);
        
        /** \brief Class description.
         *  \return message's properties description
         */
        std::string GetDescription();

        };

    }