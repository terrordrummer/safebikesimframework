#pragma once

#include "FrameworkIPC/CP2P.h"
#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkPlugin/CPluginManager.h"
#include "FrameworkCanBus/CANHardwareInterface.h"
#include "FrameworkCanBus/CANHardwareInterfaceDriver.h"
#include <set>

//#define REMOTE_MANAGEMENT

namespace FrameworkCanBus {
    
    /** \brief Hardware manager class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class manages the available hardware devices. It allows to get the list of available devices
     * and get the hardware interface to them.\n
     * There must be a single instance of this class which will perform a scan of the available devices
     * and will make hardware interfaces linked to them.
     *
     */
    class framework_canbus_class CANHardwareManager :   public CANBasicClass ,
                                                        public FrameworkPlugin::CPluginManager {
    private:    
        /** \brief Type definition of the map hardware_name -> hardware object. */
        typedef std::map<CANBasicTypes::type_string, CANHardwareInterface *> _hwDevicesType;
    
        /** \brief Container of the hardware devices vailable.                  */
        static _hwDevicesType _hwDevices;

        /** \brief Instances counter.                                           */
        static unsigned int _instanceCount;

        /** \brief Synch object.                                                */
        CSynch _synch;

        /** \brief Vitual CANBUS device interface.                              */
        CANHardwareInterfaceDriver *_virtualDevice;

    public:
        /** \brief Definition type of the container of the names of the 
                   available hardware deveices.                                 */
        typedef std::vector<CANBasicTypes::type_string> CANHardwareNameList_type;

        /** \brief Definition type of the container of the interfaces of the 
                   available hardware deveices.                                 */
        typedef std::vector<CANHardwareInterface *> CANHardwareInterfaceList_type;

        ///** \brief Class constructor.                                           */
        //CANHardwareManager() : this("") {};

        /** \brief Class constructor with config file.
         *
         * This constructor loads the available plugins and initialize them
         * with the given config file.
         */
        CANHardwareManager(std::string config_file = "");

        /** \brief Class destructor.                                            */
        ~CANHardwareManager();

        /** \brief Get the number of available devices.
         *  \return number of available devices
         */
        CANBasicTypes::type_uinteger GetNumberOfAvailableDevices()
            {return (CANBasicTypes::type_uinteger)_hwDevices.size();};

        /** \brief Get the name list of the available hardware devices.
         *  \param name_list list containing the name of the available devices
         *  \return TRUE if success
         */
        bool GetHardwareNameList(CANHardwareNameList_type &name_list);

        /** \brief Get the interface list of the available hardware devices.
         *  \param interface_list list containing the hardware interfaces of the 
         *         available devices
         *  \return TRUE if success
         */
        bool GetHardwareInterfaceList(CANHardwareInterfaceList_type &interface_list);

        /** \brief Get the virtual device.
         *  \return hardware interface of the virtual device
         */
        CANHardwareInterface *GetVirtualDevice();

        /** \brief Get a hardware interface.
         *  \param name name of the hardware device
         *  \return hardware interface of the device
         */
        CANHardwareInterface *GetHardwareInterface(CANBasicTypes::type_string name);

        /** \brief Get the hardware interface given the DeviceID.
         *  \param deviceID ID of the device
         *  \return hardware interface of the device
         */
        CANHardwareInterface *GetHardwareInterface(unsigned int deviceID);

        /** \brief Static data initializer  */
        static void staticInitializer();
        };

    
    }