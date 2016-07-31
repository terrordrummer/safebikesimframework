#pragma once

#include "FrameworkPlugin/CPlugin.h"
#include "FrameworkCanBus/CANHardwareInterface.h"
#include "FrameworkCanBus/CANHardwareInterfaceDriver.h"
#include <atlstr.h> // Advanced Template Library containing CString used by PCANLight.h
#include <sstream>
#include "PCANBasic.h"

namespace FrameworkCanBus {
    
    /** \brief Peak CANBUS device generic hardware driver interface class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class manages Peak-System CAN devices. In this class some common interface methods 
     * are implemented and shared between all inherited classes which are device-specific.
     * http://www.peak-system.com/
     */
    class CPeakDevice : public CANHardwareInterfaceDriver {
    protected:

        bool            _tested;        /**< \brief TRUE if hardware was tested.    */
        TPCANHandle     _channel;       /**< \brief Hardware channel.               */
        TPCANHandle     _channelOffset; /**< \brief Channel enumeration offset      */
    public:

        /** \brief Default constructor  */
        CPeakDevice();

        /** \brief Default destructor.  */
        ~CPeakDevice();

        /** \brief Set SUB channel.                                 */
        void SetChannel(unsigned int channel);
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
        /** \brief Get the device ID.                           */
        
        virtual std::string GetDescription();
        };

    /** \brief Peak-USB device specific class.
     *
     * This class implements the hardware interface to the Peak-USB device. The constructor allows to
     * specify the hardwdare channel to be used, this allows to select which device the hardware
     * interface will be linked to if many USB device would be plugged. \n
     * Channels are associated to the devices in the order the devices are plugged to the PC.
     */
    class CPeakDevice_USB : public CPeakDevice {
    public:
        /** \brief Initialize the hardware on the default channel.  */
        CPeakDevice_USB();
        /** \brief Destructor.                                      */
        ~CPeakDevice_USB();

        // PLUGIN VIRTUAL METHODS
        /** \brief Plugin initialization.                           */
        virtual bool        Plugin_Initialize(std::string config_file);
        /** \brief Plugin shutdown.                                 */
        virtual void        Plugin_Shutdown();
        /** \brief Get the plugin name.                             */
        virtual std::string Plugin_GetName();
        /** \brief Get the plugin version.                          */
        virtual std::string Plugin_GetVersion();

        /** \brief Hardware data of the device.
         *  \return Peak USB device description
         */
        virtual std::string GetDescription();
        };

    /** \brief Peak-PCI device specific class.
     *
     * This class implements the hardware interface to the Peak-PCI device. Depending on the hardware
     * there could be up to 8 channels available and the constructor allows to
     * specify the hardwdare channel to be used in order to define which device the hardware
     * interface will be linked to.
     */
    class CPeakDevice_PCI : public CPeakDevice {
    public:
        /** \brief Initialize the hardware on the default channel.  */
        CPeakDevice_PCI();
        /** \brief Destructor.                                      */
        ~CPeakDevice_PCI();
        
        // PLUGIN VIRTUAL METHODS
        /** \brief Plugin initialization.                           */
        virtual bool        Plugin_Initialize(std::string config_file);
        /** \brief Plugin shutdown.                                 */
        virtual void        Plugin_Shutdown();
        /** \brief Get the plugin name.                             */
        virtual std::string Plugin_GetName();
        /** \brief Get the plugin version.                          */
        virtual std::string Plugin_GetVersion();

        /** \brief Hardware data of the device.
         *  \return Peak USB device description
         */
        virtual std::string GetDescription();
        };

    }