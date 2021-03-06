#include "CPeakDevice.h"

using namespace FrameworkCanBus;
using namespace std;


/*
 /\/|___ ____            _    ____             _             _   _ ____  ____
|/\/ ___|  _ \ ___  __ _| | _|  _ \  _____   _(_) ___ ___   | | | / ___|| __ )
  | |   | |_) / _ \/ _` | |/ / | | |/ _ \ \ / / |/ __/ _ \  | | | \___ \|  _ \
  | |___|  __/  __/ (_| |   <| |_| |  __/\ V /| | (_|  __/  | |_| |___) | |_) |
   \____|_|   \___|\__,_|_|\_\____/ \___| \_/ |_|\___\___|___\___/|____/|____/
                                                        |_____|
*/
CPeakDevice_USB::~CPeakDevice_USB() {
    // if device is still open then close it
    if (_open)
        Close();
    }

/*
 ____  _             _           ___       _ _   _       _ _
|  _ \| |_   _  __ _(_)_ __     |_ _|_ __ (_) |_(_) __ _| (_)_______
| |_) | | | | |/ _` | | '_ \     | || '_ \| | __| |/ _` | | |_  / _ \
|  __/| | |_| | (_| | | | | |    | || | | | | |_| | (_| | | |/ /  __/
|_|   |_|\__,_|\__, |_|_| |_|___|___|_| |_|_|\__|_|\__,_|_|_/___\___|
               |___/       |_____|
*/
bool CPeakDevice_USB::Plugin_Initialize(std::string config_file) {
    return true;
    }

/*
 ____  _             _           ____      _   _   _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |_| \ | | __ _ _ __ ___   ___
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __|  \| |/ _` | '_ ` _ \ / _ \
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_| |\  | (_| | | | | | |  __/
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__|_| \_|\__,_|_| |_| |_|\___|
               |___/       |_____|
*/
void CPeakDevice_USB::Plugin_Shutdown() {
    // if device is still open then close it
    if (_open)
        Close();
    }

/*
 ____  _             _           ____      _   _   _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |_| \ | | __ _ _ __ ___   ___
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __|  \| |/ _` | '_ ` _ \ / _ \
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_| |\  | (_| | | | | | |  __/
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__|_| \_|\__,_|_| |_| |_|\___|
               |___/       |_____|
*/
std::string CPeakDevice_USB::Plugin_GetName() {
    return _hwInterfaceName;
    }

/*
 ____  _             _           ____      _ __     __            _
|  _ \| |_   _  __ _(_)_ __     / ___| ___| |\ \   / /__ _ __ ___(_) ___  _ __
| |_) | | | | |/ _` | | '_ \   | |  _ / _ \ __\ \ / / _ \ '__/ __| |/ _ \| '_ \
|  __/| | |_| | (_| | | | | |  | |_| |  __/ |_ \ V /  __/ |  \__ \ | (_) | | | |
|_|   |_|\__,_|\__, |_|_| |_|___\____|\___|\__| \_/ \___|_|  |___/_|\___/|_| |_|
               |___/       |_____|
*/
std::string CPeakDevice_USB::Plugin_GetVersion() {
    return "1.0";
    }


/*
  ____      _   ____                     _       _   _
 / ___| ___| |_|  _ \  ___ ___  ___ _ __(_)_ __ | |_(_) ___  _ __
| |  _ / _ \ __| | | |/ _ Y __|/ __| '__| | '_ \| __| |/ _ \| '_ \
| |_| |  __/ |_| |_| |  __|__ \ (__| |  | | |_) | |_| | (_) | | | |
 \____|\___|\__|____/ \___|___/\___|_|  |_| .__/ \__|_|\___/|_| |_|
                                          |_|
*/
std::string CPeakDevice_USB::GetDescription() {
    stringstream ss;
    ss << "Peak-System USB device on channel : " << _channel - PCAN_USBBUS1;
    return ss.str();
    }

// EXPORT THE PLUGIN CLASS
FRAMEWORK_EXPORT_PLUGIN(CPeakDevice_USB);
