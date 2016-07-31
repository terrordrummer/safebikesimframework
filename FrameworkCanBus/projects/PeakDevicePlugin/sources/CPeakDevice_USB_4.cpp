#include "CPeakDevice.h"

using namespace FrameworkCanBus;
using namespace std;

/*
  ____ ____            _    ____             _             _   _ ____  ____
 / ___|  _ \ ___  __ _| | _|  _ \  _____   _(_) ___ ___   | | | / ___|| __ )
| |   | |_) / _ \/ _` | |/ / | | |/ _ \ \ / / |/ __/ _ \  | | | \___ \|  _ \
| |___|  __/  __/ (_| |   <| |_| |  __/\ V /| | (_|  __/  | |_| |___) | |_) |
 \____|_|   \___|\__,_|_|\_\____/ \___| \_/ |_|\___\___|___\___/|____/|____/
                                                      |_____|
*/
CPeakDevice_USB::CPeakDevice_USB() {
    _channel                    = PCAN_USBBUS1 + 4;
    _hwInterfaceName.clear();
    _hwInterfaceName           = string("Peak_USB_CH4");
    _open                        = false;
    _loopback                    = false;
    _tested                      = false;
    setDeviceID(0);
    _channelOffset              = PCAN_USBBUS1;
    };