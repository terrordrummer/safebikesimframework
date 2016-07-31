#pragma once

#include "FrameworkCore/CSynch.h"
#include "FrameworkCanBus/CANMessageProperties.h"
#include "FrameworkCanBus/CANMessageTypes.h"
#include "FrameworkCanBus/CANBasicClass.h"
#include <vector>

namespace FrameworkCanBus {

    /** \brief CAN message descriptor.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class defines a CAN message. A message is identified by:
     * \li its properties
     * \li its binary data content
     * \li the list of signal's names packed inside the message
     *
     */
    class framework_canbus_class CANMessage : public CANBasicClass {
    protected:
        CANMessageProperties                       *_properties;             /**< \brief Message's properties object.                             */
        std::vector<char>                           _data;                   /**< \brief Message data, assumed to be at maximum 8 bytes length.   */
        CANMessageTypes::signal_set_type            _signalsSet;            /**< \brief Set of signals transported by the message.               */
        CANMessageTypes::signal_starting_bit_type   _signalStartingBit;    /**< \brief Signal's starting bit.                                   */
        
    public:

        /** \brief Class constructor.                                           */
        CANMessage(CANMessageProperties *properties = NULL);

        /** \brief Class desctuctor.                                            */
        ~CANMessage();

        /** \brief Get message properties.                                      */
        CANMessageProperties *get_properties() {return _properties;};

        /** \brief Get message's ata content.                                   */
        std::vector<char> get_data() {return _data;};

        /** \brief Get data pointer.                                            */
        void *GetDataPointer() { return (void *)&(_data[0]);}

        /** \brief [] Operator: this operator is used to access message's data. */
        unsigned char operator[](unsigned int i) {return _data[i];};

        /** \brief Get signal's names transported by the message.               */
        CANMessageTypes::signal_set_type get_signals() {return _signalsSet;};

        /** \brief Get starting bit of a signal.
         *  \param signal_name name of the signal
         *  \return starting bit of the signal
         */
        CANBasicTypes::type_uinteger get_signal_starting_bit(CANBasicTypes::type_string signal_name);

        /** \brief Set message's properties.
         *  \param prop message's properties
         *  \return TRUE on success
         */
        bool set_properties(CANMessageProperties *prop);

        /** \brief Set message's data.
         *  \param data new message's data container
         *  \return TRUE on success
         */
        bool set_data(std::vector<char> &data);

        /** \brief Set message's data.
         *  \param data pointer to new message's data
         *  \return TRUE on success
         */
        bool set_data(void * data);

        /** \brief Resets data content of the message.
         *
         * This method resets message's data to zero.
         */
        void ResetData();

        /** \brief Signal's starting bit setter.
         *  \param signal name of the signal
         *  \param starting_bit starting bit for the signal
         *  \return TRUE on success, FALSE if signal is colliding with other signals or does not fit the message size
         */
        bool set_signal_starting_bit(CANBasicTypes::type_string signal, CANBasicTypes::type_integer starting_bit);

        /** \brief Add a signal to the message with the assigned starting bit.
         *  \param signal name of the signal
         *  \param starting_bit starting bit of the signal
         *  \return TRUE on success, FALSE if signal is colliding with other signals or does not fit the message size
         */
        bool AddSignal(CANBasicTypes::type_string signal, CANBasicTypes::type_integer starting_bit);

        /** \brief Remove signal from message.
         *  \param signal name of the signal
         *
         */
        bool RemoveSignal(CANBasicTypes::type_string signal);
        
        /** \brief Class description.
         *  \return message description
         */
        virtual std::string GetDescription();
        };

    }