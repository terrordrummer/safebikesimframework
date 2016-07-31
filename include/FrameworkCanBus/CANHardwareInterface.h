#pragma once

// Framework
#include "FrameworkCore/CSynch.h"
#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANMessage.h"
#include "FrameworkCanBus/CANHardwareConfiguration.h"

// STL
#include <queue>
#include <string>

namespace FrameworkCanBus {

    using namespace FrameworkCore;

    /** \brief CAN hardware interface class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class defines the abstract methods to access a CANBUS hardware device in order to transmit 
     * and receive messages. This interface is generic and will be specialized by the device-dependent 
     * classes.
     */
    class framework_canbus_class CANHardwareInterface : public CANBasicClass {
    protected:

        /** \brief Hardware interface name.                     */
        std::string _hwInterfaceName;

        /** \brief Open status flag.                            */
        bool _open;

        /** \brief Loopback messages queue.                     */
        std::queue<CANMessage *> _loopbackQueue;

        /** \brief Loopback status flag.                        */
        bool _loopback;

        /** \brief Loopback queue maximum size.                 */
        CANBasicTypes::type_uinteger _loopbackMaxSize;

        /** \brief Synchronization object.                      */
        CSynch _synch;

    public:
        /** \brief Class constructor.                           */
        CANHardwareInterface();

        /** \brief Class destructor.                            */
        virtual ~CANHardwareInterface();
        
        /** \brief Test a CANBUS hardware.
         *  \return TRUE if device is ready
         *
         * This method tests if a hardware is installed and available.
         */
        virtual bool Test();

        /** \brief Test if the hardware is open.
         *  \return TRUE if the interface is opened.
         *
         * This method is used to test if the hardware device is currently opened. A device results to be
         * opened if the Open method has been called and the comunication speed, the loopback flag and the
         * transmission channel have been assigned. To be reconfigured the device has to be closed and  
         * opened again with the new set of configuration parameters.
         */
        bool isOpen();

        /** \brief Open a device.
         *  \param config configuration parameter class
         *  \result TRUE if device is opened succesfully
         *
         * This method initializes the CANBUS hardware device with the given parameters for comunication.
         */
        virtual bool Open(const CANHardwareConfiguration config);

        /** \brief Closes the connection to the hardware.
         *  \return TRUE if device has been succesfully closed
         * 
         * A device needs to be closed to terminate the comunication. After a device has been closed it can be opened
         * agains with new configuration parameter set.
         */
        virtual bool Close();

        /** \brief Transmit a message to the hardware.
         *  \param msg message to be transmitted
         *  \result TRUE if success
         *
         * This method sneds a message on the CANBUS network immediatly without any extra operation. This allows to
         * control the transmission at low-level setting the desired byte-content of the message's data to be 
         * transmitted.
         */
        virtual bool Transmit(CANMessage *msg);

        /** \brief Wait for a message from the hardware.
         *  \param msg message reference which will contain the next received message's data
         *  \return TRUE if success
         *
         * This method tries to read a message received from the device. The result can be OK if a message was received
         * or there can be errors like buffer-overrun, bus-heavy and other which will result in a negative result.
         * In absence of bus errors this function will wait for the next message to be received.
         */
        virtual bool Receive(CANMessage *msg);

        /** \brief Wait for a message from the canbus with a timeout.
         *  \param msg message reference which will contain the next received message's data
         *  \param timeout_ms timeout in milliseconds
         *  \return TRUE if success
         *
         * This method tries to read a message received from the device. The result can be TRUE if a message was received,
         * FALSE in case of error or in case of timeout. Method HasError() can be called to check If an error occurred, 
         * if no error occurred then the timeout elapsed.
         */
        virtual bool Receive(CANMessage *msg, unsigned int timeout_ms);

        /** \brief Empty the receiving buffer.
         *
         * This method empties the receiving queue.
         */
        virtual void EmptyReceivingBuffer();

        /** \brief Get hardware name identifier.                */
        CANBasicTypes::type_string getName() {return _hwInterfaceName;};

        /** \brief Get device ID.
         *  \return device id
         *
         * This method returns the DeviceID factory value which is different for any hardware device. This allows
         * to identify one specific hardware device not depending on the channel it is assigned to.
         */
        virtual unsigned int getDeviceID();

        // -------------------------------------------------------
        //                     Loopback management
        // -------------------------------------------------------
        /** \brief Enable loopback for this device.             */
        void EnableLoopback();

        /** \brief disable loopback for this device.            */
        void DisableLoopback();

        /** \brief Loopback message transmission.
         *  \param msg message to be transmitted
         *  \return TRUE if success
         *
         * This method pushes a message into the loopback buffer.
         */
        bool LoopbackTransmit(CANMessage *msg);

        /** \brief Loopback message receiving.
         *  \param msg message to be received
         *  \result TRUE if message was received
         *
         * This method reads the next available message from the loopback queue. If no messages are available then
         * it returns FALSE otherwise the message content is returned.
         */
        bool LoopbackReceive(CANMessage *msg);

        /** \brief Loopback buffer size.
         *  \param size buffer size
         *
         * This method defines the number of messages that the loopback will be able to contain.
         */
        void SetLoopbackBufferSize(CANBasicTypes::type_uinteger size);
        
        };

    }