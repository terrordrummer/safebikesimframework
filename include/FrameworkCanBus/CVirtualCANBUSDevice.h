#pragma once

#include "FrameworkCanBus/CANHardwareInterfaceDriver.h"

namespace FrameworkCanBus {
    
    /** \brief CANBUS virtual device.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements a virtual CAN device. This device does not esist physically on the machine but
     * if the loopback is enabled it can be used to simulate an architecture network.
     */
    class CVirtualCANBUSDevice : public CANHardwareInterfaceDriver {
    public:

        /** \brief Default constructor.                         */
        CVirtualCANBUSDevice();
        
        /** \brief Test a CANBUS hardware.
         *
         * This method tests if a hardware is present or available.
         */
        virtual bool Test();

        /** \brief Open a CANBUS hardware.
         *
         * This method connects to the CANBUS hardware device and initialize it for comunication.
         */
        virtual bool Open(const CANHardwareConfiguration config);

        /** \brief Closes the connection to the hardware.       */
        virtual bool Close();

        /** \brief Transmit a message to the hardware.          */
        virtual bool Transmit(CANMessage *msg);

        /** \brief Wait for a message from the hardware.        */
        virtual bool Receive(CANMessage *msg);
        
        /** \brief Wait for a message from the hardware.        */
        virtual bool Receive(CANMessage *msg, unsigned int timeout_ms);

        /** \brief Empty the receiving buffer.                  */
        virtual void EmptyReceivingBuffer();

        /** \brief Get hardware name identifier.                    */
        virtual CANBasicTypes::type_string getName() {return "CAN_VirtualDevice";};

        /** \brief Hardware data of the device.
         *  \return virtual device description
         */
        virtual std::string GetDescription();

        };
    }