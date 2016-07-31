#pragma once

#include "FrameworkPlugin/CPlugin.h"
#include "FrameworkCanBus/CANHardwareInterface.h"

namespace FrameworkCanBus {

    /** \brief Class name of any CAN BUS hardware interface plugin      */
#define CANBUS_HARDWARE_DEVICE_DRIVER_CLASS std::string("CANBUS_HARDWARE_DEVICE_DRIVER_CLASS")

    /** \brief CAN hardware interface plug-in class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class defines the basic class to develope a hardware device interface plugin for the      
     * CAN BUS module. User will be able to develope its own hwrdware interface module to let the      
     * hardware manager to use a specific device on the PC.
     */
    class framework_canbus_class CANHardwareInterfaceDriver :   public CANHardwareInterface ,
                                                                public FrameworkPlugin::CPlugin {
    private:
        /** \brief Device ID assigned by the manager.               */
        unsigned int _deviceID;

    public:
        CANHardwareInterfaceDriver() {_hwInterfaceName = std::string("CANHardwareInterfaceDriver");}
        // empty class, everything is inherited from CANHardwareInterface and CPlugin classes.
        /** \brief Plugin initialization.                           */
        virtual bool        Plugin_Initialize(std::string config_file) {return true;}
        /** \brief Plugin shutdown.                                 */
        virtual void        Plugin_Shutdown() {};
        /** \brief Get the plugin name.                             */
        virtual std::string Plugin_GetName() {return "CANHardwareInterfaceDriver class";};
        /** \brief Get the plugin version.                          */
        virtual std::string Plugin_GetVersion(){return "1.0";};
        /** \brief Get the plugin class.                            */
        std::string Plugin_GetClass() {return CANBUS_HARDWARE_DEVICE_DRIVER_CLASS;};

        // inherited from CANHardwareInterface
        /** \brief Test a CANBUS hardware.                          */
        virtual bool Test() {return true;};
        /** \brief Open a device.                                   */
        virtual bool Open(const CANHardwareConfiguration config) {return true;};
        /** \brief Closes the connection to the hardware.           */
        virtual bool Close() {return true;};
        /** \brief Transmit a message to the hardware.              */
        virtual bool Transmit(CANMessage *msg) {return true;};
        /** \brief Wait for a message from the hardware.            */
        virtual bool Receive(CANMessage *msg) {return true;};
        /** \brief Empty the receiving buffer.                      */
        virtual void EmptyReceivingBuffer() {};

        /** \brief Get device ID.                                   */
        __inline unsigned int getDeviceID() {return _deviceID;};
        /** \brief Setter for the deviceID member.                  */
        __inline void setDeviceID(unsigned int id) { _deviceID = id;}

        };

    }