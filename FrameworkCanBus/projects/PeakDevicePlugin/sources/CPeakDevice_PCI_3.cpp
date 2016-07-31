#include "CPeakDevice.h"

using namespace FrameworkCanBus;
using namespace std;

/*
  ____ ____            _    ____             _              ____   ____ ___
 / ___|  _ \ ___  __ _| | _|  _ \  _____   _(_) ___ ___    |  _ \ / ___|_ _|
| |   | |_) / _ \/ _` | |/ / | | |/ _ \ \ / / |/ __/ _ \   | |_) | |    | |
| |___|  __/  __/ (_| |   <| |_| |  __/\ V /| | (_|  __/   |  __/| |___ | |
 \____|_|   \___|\__,_|_|\_\____/ \___| \_/ |_|\___\___|___|_|    \____|___|
                                                      |_____|
*/
CPeakDevice_PCI::CPeakDevice_PCI() {
    _channel     = PCAN_PCIBUS1 + 3;
    _hwInterfaceName.clear();
    _hwInterfaceName = string("PeakDevice_PCI_3");
    _open       = false;
    _loopback   = false;
    _tested     = false;
    setDeviceID(0);
    _channelOffset = PCAN_PCIBUS1;
    };

