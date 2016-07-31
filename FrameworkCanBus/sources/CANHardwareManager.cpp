#include "FrameworkCanBus/CANHardwareManager.h"
#include "FrameworkCanBus/CVirtualCANBUSDevice.h"
#include <sstream>
#include <vector>

using namespace FrameworkCanBus;

// -------------------------------------------------------------------------------
//                          STATIC VARS DECLARATION

CANHardwareManager::_hwDevicesType CANHardwareManager::_hwDevices;
unsigned int CANHardwareManager::_instanceCount = 0;
// -------------------------------------------------------------------------------

// -------------------------------------------------------
// alias to the Windows system GetLastError API functions
 DWORD (WINAPI *CANHardwareManager_winGetLastError)(void) = GetLastError;
// -------------------------------------------------------


/*
     _        _   _      ___       _ _   _       _ _
 ___| |_ __ _| |_(_) ___|_ _|_ __ (_) |_(_) __ _| (_)_______ _ __
/ __| __/ _` | __| |/ __|| || '_ \| | __| |/ _` | | |_  / _ \ '__|
\__ \ || (_| | |_| | (__ | || | | | | |_| | (_| | | |/ /  __/ |
|___/\__\__,_|\__|_|\___|___|_| |_|_|\__|_|\__,_|_|_/___\___|_|
*/
 void CANHardwareManager::staticInitializer()  {
    CANHardwareManager::_instanceCount = 0;
    _hwDevices.clear();
     }

/*
  ____    _    _   _ _   _               _                        __  __
 / ___|  / \  | \ | | | | | __ _ _ __ __| |_      ____ _ _ __ ___|  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| | |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  |  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                           |___/
*/
 CANHardwareManager::CANHardwareManager(std::string config_file) {
    SetErrorClassPrefix("CANHardwareManager");
    // at the  first initialization loads the hardware plugins
    if (_instanceCount == 0) {

        // clear the device list
        _hwDevices.clear();

        // set the hardware devices plugins search path
        ClearSearchPath();
        InsertSearchPath("./_hwDevices/");

        // set the virtual device as default
        _virtualDevice = new CVirtualCANBUSDevice();
        std::string devname = _virtualDevice->getName();
        std::transform(devname.begin(), devname.end(), devname.begin(), ::toupper);
        _hwDevices[devname] = _virtualDevice;

        // set the plugin manager class
        SetPluginClass(CANBUS_HARDWARE_DEVICE_DRIVER_CLASS);

        // load all available plugins
        LoadAvailablePlugins();

        // Initialize the plugins with the config file
        InitializeAllPlugins(config_file);

        // get the loaded plugin name list
        std::vector<std::string> list = GetLoadedPluginsName();

        // _Initialize, test and store only the available hardware devices
        for (unsigned int i=0; i<list.size(); i++) {
            CANHardwareInterfaceDriver *device = (CANHardwareInterfaceDriver *)GetPlugin(list[i]);
            if (device->Test()) {
                // device available, put it into the device list
                _hwDevices[list[i]] = device;
            } else {
                // device unavailable, unload it
                UnloadPlugin(list[i]);
            }
        }
    }
    _instanceCount++;
    };

/*
 /\/|___    _    _   _ _   _               _                        __  __
|/\/ ___|  / \  | \ | | | | | __ _ _ __ __| |_      ____ _ _ __ ___|  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |     / _ \ |  \| | |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___ / ___ \| |\  |  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |  | | (_| | | | | (_| | (_| |  __/ |
   \____/_/   \_\_| \_|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                             |___/
*/
CANHardwareManager::~CANHardwareManager() {
    _instanceCount--;
    if (!_instanceCount) {
        // stops all hardware interfaces
        _hwDevicesType::iterator it = _hwDevices.begin();
        while (it != _hwDevices.end()) {
            // if the hardware device is already opened then close it
            if (it->second->isOpen())
                it->second->Close();
            it++;
            }
        // shutdown all hardware device plugins
        ShutdownAllPlugins();
        // empty hardware interfaces list
        _hwDevices.clear();
        }
    };

/*
  ____      _   _   _               _                        _   _                      _     _     _
 / ___| ___| |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___| \ | | __ _ _ __ ___   ___| |   (_)___| |_
| |  _ / _ \ __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \  \| |/ _` | '_ ` _ \ / _ \ |   | / __| __|
| |_| |  __/ |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |\  | (_| | | | | | |  __/ |___| \__ \ |_
 \____|\___|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|_| \_|\__,_|_| |_| |_|\___|_____|_|___/\__|
*/
bool CANHardwareManager::GetHardwareNameList(CANHardwareNameList_type &name_list) {
    
    // clear the list
    name_list.clear();
    name_list.resize(_hwDevices.size());
    // create the device's name array
    _hwDevicesType::iterator it = _hwDevices.begin();
    int i=0;
    while (it != _hwDevices.end()) {
        name_list[i++] = it->first;
        it++;
        }
    return true;
    };

/*
  ____      _   _   _               _                        ___       _             __                _     _     _
 / ___| ___| |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___| |   (_)___| |_
| |  _ / _ \ __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \| || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \ |   | / __| __|
| |_| |  __/ |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/| || | | | ||  __/ |  |  _| (_| | (_|  __/ |___| \__ \ |_
 \____|\___|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|___|_| |_|\__\___|_|  |_|  \__,_|\___\___|_____|_|___/\__|
*/
bool CANHardwareManager::GetHardwareInterfaceList(CANHardwareInterfaceList_type &interface_list) {
    
    // clear the  list
    interface_list.clear();
    interface_list.resize(_hwDevices.size());
    // create the interfaces array
    _hwDevicesType::iterator it = _hwDevices.begin();
    int i=0;
    while (it != _hwDevices.end()) {
        interface_list[i++] = it->second;
        it++;
        }
    return true;
    };

/*
  ____      _ __     ___      _               _ ____             _
 / ___| ___| |\ \   / (_)_ __| |_ _   _  __ _| |  _ \  _____   _(_) ___ ___
| |  _ / _ \ __\ \ / /| | '__| __| | | |/ _` | | | | |/ _ \ \ / / |/ __/ _ \
| |_| |  __/ |_ \ V / | | |  | |_| |_| | (_| | | |_| |  __/\ V /| | (_|  __/
 \____|\___|\__| \_/  |_|_|   \__|\__,_|\__,_|_|____/ \___| \_/ |_|\___\___|
*/
CANHardwareInterface *CANHardwareManager::GetVirtualDevice() {
    return _virtualDevice;
    };

/*
  ____      _   _   _               _                        ___       _             __
 / ___| ___| |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___
| |  _ / _ \ __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \| || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
| |_| |  __/ |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/| || | | | ||  __/ |  |  _| (_| | (_|  __/
 \____|\___|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|___|_| |_|\__\___|_|  |_|  \__,_|\___\___|

*/
CANHardwareInterface *CANHardwareManager::GetHardwareInterface(CANBasicTypes::type_string name) {
    // search the device
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    _hwDevicesType::iterator it = _hwDevices.find(name);
    if (it!=_hwDevices.end())
        // device found
        return it->second;
    else
        // device not founded
        return NULL;
    };

/*
  ____      _   _   _               _                        ___       _             __
 / ___| ___| |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___
| |  _ / _ \ __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \| || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
| |_| |  __/ |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/| || | | | ||  __/ |  |  _| (_| | (_|  __/
 \____|\___|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|___|_| |_|\__\___|_|  |_|  \__,_|\___\___|

*/
CANHardwareInterface *CANHardwareManager::GetHardwareInterface(unsigned int deviceID) {
    // cycle throug the hardware devices and search for the requested device ID
    _hwDevicesType::iterator it = _hwDevices.begin();
    while (it != _hwDevices.end()) {
        if (it->second->getDeviceID() == deviceID)
            return it->second;
        it++;
        }
    return NULL;
    };