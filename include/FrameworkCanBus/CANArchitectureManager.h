#pragma once

#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANMessageManager.h"
#include "FrameworkCanBus/CANSignalManager.h"
#include "FrameworkCanBus/CANEcuManager.h"
#include "FrameworkCanBus/CANTransmitterManager.h"
#include "FrameworkCanBus/CANReceiverManager.h"
#include "FrameworkCanBus/CANHardwareManager.h"
#include "FrameworkCanBus/CANCallbackManager.h"
#include <string>

namespace FrameworkCanBus {

    /** \brief CAN architecture manager. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class manages at the highest level the CANBUS architecture. \n
     *
     * The term "Architecture Management" is concerning all the following aspects of the CAN BUS network:
     *
     * <b>Signal management</b>
     * \li add a signal with some properties
     * \li remove a signal
     * \li pack the signal into a message
     * \li associate a signal to a transmitting ECU and to some receiving ECU
     *
     * <b>Message management</b>
     * \li add a message with some properties
     * \li remove a message
     * \li pack signals into the message
     * \li associate the message to a transmitting ECU and to several receiving ECU
     *
     * <b>ECU management</b>
     * \li add/remove ECUs
     * \li mark an ECU as transmitter of some messages and signals
     * \li mark an ECU as receiver of some signals and messages
     *
     * <b>Transmission management</b>
     * \li transmit a single message on the CANBUS
     * \li activate/deactivate a periodic message which will be transmitted with a fixed frequency
     * \li transmit a message for a given number of times with a fixed frequency
     * \li transmit a message for a given period with a fixed frequency
     * \li collect transmission statistics like the number of times the message was transmitted and the average
     *     transmission period
     * \li enable/disable the transmission of a signal
     *
     * <b>Reception management</b>
     * \li activate/deactivate the reception of a message
     * \li enable/disable the reception of some signals
     *
     * <b>Hardware Management</b>
     * \li get the list of available CANBUS hardware devices
     * \li configure devices
     * \li route messages through the available devices
     *
     * <b>Events management</b>
     * For an efficient asynchronous management of the CANBUS data, the architecture manager offers the
     * methods to install as many user-defined callbacks as needed which will be called in the occurence of the
     * following events:
     * \li a message has been transmitted
     * \li a message has been received (and unpacked)
     * \li a signal value has been transmitted
     * \li a signal value has been updated
     *
     *
     */
    class framework_canbus_class CANArchitectureManager : public CANBasicClass {
    private:
        CANHardwareManager      *_hardwareManager;      /**< \brief Hardware manager.       */
        CANMessageManager       *_messageManager;       /**< \brief Message manager.        */
        CANSignalManager        *_signalManager;        /**< \brief Signal manager.         */
        CANEcuManager           *_ecuManager;           /**< \brief ECU manager.            */
        CANTransmitterManager   *_transmitterManager;   /**< \brief Transmitter manager.    */
        CANReceiverManager      *_receiverManager;      /**< \brief Receiver manager.       */
        CANCallbackManager      *_callbackManager;      /**< \brief Callback manager.       */

        /** \brief Architecture's default CANBUS device.  */
        CANBasicTypes::type_string _defaultDevice;

        /** \brief Signal's collision checking.
         *
         * This method verifies if some signals are colliding eachother inside the message with 
         * the given id. An empty string is returned if no collision occurs, otherwise a descriptipon of
         * the first collision found is returned.
         */
        CANBasicTypes::type_string CheckSignalCollisions(CANBasicTypes::type_uinteger message_id);


    public:
        // ----------------------------------------------------------------------------------
        // ------------------------------- constructors/destructors
        // ----------------------------------------------------------------------------------
        /** \brief Class constructor.       */
        CANArchitectureManager();

        /** \brief Class destructor.        */
        ~CANArchitectureManager();

        // ----------------------------------------------------------------------------------
        // ------------------------------- basic getters
        // ----------------------------------------------------------------------------------
        /** \brief Get the hardware manager.
         *  \return hardware manager reference
         */
        CANHardwareManager       *get_hardware_manager() {return _hardwareManager;};

        /** \brief Get the message manager.
         *  \return message manager reference
         */
        CANMessageManager        *get_message_manager() {return _messageManager;};

        /** \brief Get the signal manager.
         *  \return hardware signal reference
         */
        CANSignalManager         *get_signal_manager() {return _signalManager;};

        /** \brief Get the ECU manager.
         *  \return ECU manager reference
         */
        CANEcuManager            *get_ecu_manager() {return _ecuManager;};

        /** \brief Get the message transmitter manager.
         *  \return message transmitter manager reference
         */
        CANTransmitterManager    *get_transmitter_manager() {return _transmitterManager;};

        /** \brief Get the message receiver manager.
         *  \return message receiver reference
         */
        CANReceiverManager       *get_receiver_manager() {return _receiverManager;};

        /** \brief Get the callback manager.
         *  \return callback manager reference
         */
        CANCallbackManager       *get_callback_manager() {return _callbackManager;};


        // ----------------------------------------------------------------------------------
        // ------------------------------- loading architecture
        // ----------------------------------------------------------------------------------
        /** \brief Loads the architecture from a dbc file.
         *  \param dbc_fname file name containing the dbc description of the architecture
         *  \return TRUE if architecture has been succesfully loaded
         */
        bool LoadDBC(std::string dbc_fname);

        // ----------------------------------------------------------------------------------
        // ------------------------------- managing architecture
        // ----------------------------------------------------------------------------------
        /** \brief This method adds a signal into a message.
         *  \param signal_name name of the signal
         *  \param message_id identifier of the message
         *  \param starting_bit starting bit for the signal
         *  \result TRUE if signal has been succesfully packed into the message
         *
         * This method is used to pack a signal into a message by giving the starting bit. The signal 
         * and the message must already exist in the architecture.
         */
        bool AddSignalToMessage(    CANBasicTypes::type_string      signal_name, 
                                    CANBasicTypes::type_uinteger    message_id,
                                    CANBasicTypes::type_uinteger    starting_bit);

        /** \brief This method removes a signal from a message.
         *  \param signal_name name of the signal to be removed
         *  \param message_id identifier of the message the signal has to be removed from
         *  \return TRUE if success
         *
         * This method is used to remove a signal from a message. The signal 
         * and the message must already exist in the architecture.
         */
        bool RemoveSignalFromMessage ( CANBasicTypes::type_string    signal_name, 
                                       CANBasicTypes::type_uinteger  message_id);

        // ----------------------------------------------------------------------------------
        // ------------------------------- hardware management
        // ----------------------------------------------------------------------------------
        /** \brief Sets the default hardware by name.
         *  \param name hardware device name
         *  \return TRUE if success
         */
        bool SetDefaultHardwareDevice(CANBasicTypes::type_string name);

        /** \brief Sets the default hardware with the given deviceID.
         *  \param deviceID identifier of the hardware device
         *  \return TRUE if success
         */
        bool SetDefaultHardwareDevice(unsigned int deviceID);

        /** \brief Returns the current default hardware interface
         *  \return default hardware interface
         */
        CANHardwareInterface * GetDefaultHardwareInterface() {
                        return _hardwareManager->GetHardwareInterface(_defaultDevice);
                        }

        // ----------------------------------------------------------------------------------
        // ------------------------------- start/stop transmission and reception
        // ----------------------------------------------------------------------------------
        /** \brief Start receiving messages from default hardware.
         *  \return TRUE on success
         */
        bool StartReceivingFromDefaultHardwareDevice();

        /** \brief Stop receiving messages from default hardware.
         *  \return TRUE on success
         */
        bool StopReceivingFromDefaultHardwareDevice();

        /** \brief Activate an ECU which will start to transmit its messages 
          *        on the default hardware.
          * \param ECU_name name of the ECU to be started
          */
        void StartECU(CANBasicTypes::type_string ECU_name);

        /** \brief Activate an ECU which will start to transmit its messages 
          *        on the assigned hardware .
          * \param ECU_name name of the ECU to be started
          * \param HW_name name of the hardware device on which the ECU has to be started
          */
        void StartECUOnDevice(CANBasicTypes::type_string ECU_name, 
                                CANBasicTypes::type_string HW_name);

        /** \brief Deactivate an ECU which will stop to transmit its messages.
         *  \param ECU_name name of the ECU to be stopped
         */
        void StopECU(CANBasicTypes::type_string ECU_name);

        /** \brief Starts all ECUs on the default hardware.                     */
        void StartAllECU();

        /** \brief Starts all ECUs on the assigned hardware.
         *  \param HW_name hardware device name
         */
        void StartAllECUOnDevice(CANBasicTypes::type_string HW_name);

        /** \brief Stops all ECUs.                                              */
        void StopAllECU();

        /** \brief Start transmitting all the messages packing the given signal.
         *  \param signal_name name of the signal that has to be started
         *  \result TRUE if success
         *
         * This method mark the signal as \<transmitted\> and starts all the messages that are packing it */
        bool StartSignal(CANBasicTypes::type_string signal_name);

        /** \brief Stop transmitting all the messages packing the given signal.   
         *  \param signal_name name of the signal that has to be stopped
         *  \result TRUE on success
         *
         * This method mark the signal as \<non-transmitted\> so message will set to zero the bits containing 
         * its value. If no more transmitted signal are present in the message then the message itself is stopped.
         */
        bool StopSignal(CANBasicTypes::type_string signal_name);

        /** \brief Start transmitting the given message using the given hardware device.
         *  \param msg_name name of the message that has to be started
         *  \param HW_device name of the device on which transmit the message
         *  \result TRUE if success
         *
         * This method start transmitting a message using the given hardware device. */
        bool StartMessage(CANBasicTypes::type_string msg_name, CANBasicTypes::type_string HW_device);

        /** \brief Start transmitting the given message using the default hardware device.
         *  \param msg_name name of the message that has to be started
         *  \result TRUE if success
         *
         * This method start transmitting a message using the default hardware device. */
        bool StartMessage(CANBasicTypes::type_string msg_name);

        /** \brief Start transmitting the given message using the given hardware device.
         *  \param msg_id identifier of the message that has to be started
         *  \param HW_device name of the device on which the message has to be transmitted
         *  \result TRUE if success
         *
         * This method start transmitting a message with all the signals packed into it using the given hardware device. */
        bool StartMessage(CANBasicTypes::type_uinteger msg_id, CANBasicTypes::type_string HW_device);

        /** \brief Start transmitting the given message using the default hardware device.
         *  \param msg_id identifier of the message that has to be started
         *  \return TRUE if success
         *
         * This method start transmitting a message with all the signals packed into it using the default hardware device.*/
        bool StartMessage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Stop transmitting the given message.
         *  \param msg_name name of the message that has to be stopped
         *  \return TRUE if success
         *
         * This method stops the transmission of a message. All the signals are notify to be non-transmnitted
         * by this message.
         */
        bool StopMessage(CANBasicTypes::type_string msg_name);

        /** \brief Stop transmitting the given message.   
         *  \param msg_id identifier of the message that has to be stopped
         *  \result TRUE if success
         *
         * This method stops the transmission of a message. All the signals are notify to be non-transmnitted
         * by this message.
         */
        bool StopMessage(CANBasicTypes::type_uinteger msg_id);
        
        // ----------------------------------------------------------------------------------
        // ------------------------------- FILTERING
        // ----------------------------------------------------------------------------------
        /** \brief Enable the given signal.
         *  \param signal_name name of the signal that has to be enabled
         *  \return TRUE on success
         *
         * When a signal's value is going to be packed into a message it is forced to be the minimum value 
         * admitted if the signal is disabled. Disabling a signal is tipically used for testing, the default 
         * status of a signal is enabled.
         */
        bool EnableSignal(CANBasicTypes::type_string signal_name);

        /** \brief Disable the given signal.
         *  \param signal_name name of the signal that has to be disabled
         *  \return TRUE if success
         *
         * When a signal's value is going to be packed into a message it is forced to be the minimum value 
         * admitted if the signal is disabled. Disabling a signal is tipically used for testing, the default 
         * status of a signal is enabled.
         */
        bool DisableSignal(CANBasicTypes::type_string signal_name);

        // ----------------------------------------------------------------------------------
        // ------------------------------- aux
        // ----------------------------------------------------------------------------------
        /** \brief Class description.
         *  \result architecture description
         *
         * This method returns a full description of the architecture instance. This description shows
         * the list of the existing ECUs, messages and signals.
         */
        virtual std::string GetDescription();

        };


    };