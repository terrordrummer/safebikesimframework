#pragma once

#include "CANBasicClass.h"
#include "CANEcuTypes.h"
#include <string>

namespace FrameworkCanBus {

    /** \brief ECU descriptor.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements an ECU descriptor. An ECU is considered a device which is:
     * \li receiving some CAN messages
     * \li transmitting some CAN messages
     *
     * An ECU is also indirectly linked to the set of signals which are packed into the messages the ECU is transmitting and
     * listening to. For this reason an ECU can be marked not only as transmitter and receiver of messages but also
     * as transmitter and receiver of signals.
     *
     * As a consequence, an ECU is assumed to be defined by its name and by four sets:
     * \li the set of the transmitted messages
     * \li the set of the received messages
     * \li the set of the transmitted signals
     * \li the set of the received signals
     *
     * This class exist only to store these information, any control about the correctness of these values have
     * to be done externally.
     *
     */
    
    class framework_canbus_class CANEcu : public CANBasicClass {
    private:
        /** \brief ECU name.                        */
        CANBasicTypes::type_string _ecuName;

        /** \brief Set of transmitted messages.     */
        CANEcuTypes::msg_id_set_type _transmittedMsgIdList;

        /** \brief Set of received messages.        */
        CANEcuTypes::msg_id_set_type _receivedMsgIdList;

        /** \brief Set of transmitted signals.      */
        CANEcuTypes::signal_set_type _transmittedSignalList;

        /** \brief Set of received signals.         */
        CANEcuTypes::signal_set_type _receivedSignalList;

    public:
        /** \brief Class constructor.               */
        CANEcu();

        /** \brief Class constructor.               */
        CANEcu(CANBasicTypes::type_string _ecuName);

        /** \brief ECU name getter.                              */
        CANBasicTypes::type_string get_name() {return _ecuName;};

        /** \brief Get the full list of transmitted messages.    */
        CANEcuTypes::msg_id_set_type get_transmitted_msg_id_list() { return _transmittedMsgIdList;}; 

        /** \brief Get the full list of received messages.       */
        CANEcuTypes::msg_id_set_type get_received_msg_id_list() { return _receivedMsgIdList;};

        /** \brief Get the full list of transmitted signals.     */
        CANEcuTypes::signal_set_type get_transmitted_signal_list() {return _transmittedSignalList;};

        /** \brief Get the full list of received signals.        */
        CANEcuTypes::signal_set_type get_received_signal_list() {return _receivedSignalList;};

        /** \brief ECU name setter.  
         *  \param name name to be assigned to the ECU
         *  \return TRUE if success
         */
        bool set_name(CANBasicTypes::type_string name);

        /** \brief Mark the ECU as receiver of this message.
         *  \param msg_id identifier of the message
         *  \return TRUE if success
         *
         * Add a message in the set of received messages by the ECU. If the message is 
         * already received by the ECU then a warning is traced.  
         */
        bool AddReceivedMesssage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Unmark the ECU as receiver of the message.
         *  \param msg_id identifier of the message
         *  \return TRUE if success
         *
         * Remove a message from the set of received messages by the ECU. If the message is 
         * not in the set of the received messages then a warning is traced.
         */
        bool RemoveReceivedMesssage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Mark the ECU as transmitter of the message.
         *  \param msg_id identifier of the message
         *  \return TRUE if success
         *
         * Add a message in the set of transmitted messages by the ECU. If the message is 
         * already transmitted by the ECU then a warning is traced. 
         */
        bool AddTransmittedMessage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Unmark the ECU as transmitter of the message
         *  \param msg_id identifier of the message
         *  \return TRUE if success
         *
         * Remove a message from the set of transmitted messages by the ECU. If the message is 
         * not in the set of the transmitted messages then a warning is traced.
         */
        bool RemoveTransmittedMessage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Mark the ECU as transmitter of the signal.    
         *  \param signal_name name of the signal
         *  \return TRUE if success
         *
         * Add a signal in the set of received signals by the ECU. If the signal is 
         * already received by the ECU then a warning is traced.  
         */
        bool AddTransmittedSignal(CANBasicTypes::type_string signal_name);

        /** \brief Unmark the ECU as transmitter of the signal.
         *  \param signal_name name of the signal
         *  \return TRUE if success
         *
         * Remove a signal from the set of transmitted signals by the ECU. If the signal is 
         * not in the set of the transmitted signals then a warning is traced.
         */
        bool RemoveTransmittedSignal(CANBasicTypes::type_string signal_name);

        /** \brief Mark the ECU as receiver of the signal.
         *  \param signal_name name of the signal
         *  \return TRUE if success
         *
         * Add a signal in the set of received signals by the ECU. If the signal is 
         * already received by the ECU then a warning is traced.  
         */
        bool AddReceivedSignal(CANBasicTypes::type_string signal_name);

        /** \brief Unmark the ECU as receiver of the signal.
         *  \param signal_name name of the signal
         *  \return TRUE if success
         *
         * Remove a signal from the set of received signals by the ECU. If the signal is 
         * not in the set of the received signals then a warning is traced.
         */
        bool RemoveReceivedSignal(CANBasicTypes::type_string signal_name);

        /** \brief Class description.  
         *  \return ECU description string
         */
        virtual std::string GetDescription();
    };

}