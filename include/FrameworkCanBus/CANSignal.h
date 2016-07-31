#pragma once

#include "FrameworkCore/CSynch.h"
#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANSignalProperties.h"
#include <string>

namespace FrameworkCanBus {

    /** \brief Can signal descriptor. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class defines a signal. A signal is identified by
     * \li its properties
     * \li its current value
     * \li the list of messages in which it is packed
     *
     */
    class framework_canbus_class CANSignal: public CANBasicClass {
    private :
        /** \brief signal's properties                                          */
        CANSignalProperties                *_properties;

        /** \brief list of messages which are packing the signal                */
        CANSignalTypes::message_set_type    _messageSet;

        /** \brief signal's value                                               */
        CANBasicTypes::type_float           _value;

        /** \brief signal's disabled value                                      */
        CANBasicTypes::type_float           _disabledValue;

        /** \brief Signal's enable flag.                                        */
        bool _enabled;

        /** \brief Instance synchronization object                              */
        FrameworkCore::CSynch _synch;

    public :

        /** \brief specialized constructor                                      */
        CANSignal(CANSignalProperties *properties = NULL);
        /** \brief default destructor                                           */
        ~CANSignal();
        
        /** \brief properties getter                                            */
        CANSignalProperties *get_properties() {return _properties;};

        /** \brief signed value getter                                          */
        CANBasicTypes::type_float get_value() {if (_enabled) return _value; else return _disabledValue; } ;

        /** \brief returns the set of messages in which the signal is packed    */
        CANSignalTypes::message_set_type get_message_set() {return _messageSet;};

        /** \brief value setter                                                 */
        bool set_value(CANBasicTypes::type_float);

        /** \brief add a message name in the list                               */
        bool AddMessage(CANBasicTypes::type_string name);

        /** \brief remove a message from the list                               */
        bool RemoveMessage(CANBasicTypes::type_string name);

        /** \brief Enable the signal.
         *
         * If a signal is enabled the get_value method returns the last
         * value assigned to the signal using set_value, if a signal is disabled
         * then the method get_value returns the maximum value between zero
         * and the minimum range value.
         */
        void Enable() {_enabled=true;};

        /** \brief Disable the signal.
         *
         * When a signal is disabled, its value is always zero.
         */
        void Disable() {_enabled=false;};

        /** \brief signal descfription string
         *  \return signal description
         */
        virtual std::string GetDescription();
        };
    }