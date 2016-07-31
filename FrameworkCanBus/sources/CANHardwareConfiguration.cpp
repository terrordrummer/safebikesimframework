#include "FrameworkCanBus/CANHArdwareConfiguration.h"

using namespace FrameworkCanBus;

/*
  ____    _    _   _ _   _               _                         ____             __ _                       _   _
 / ___|  / \  | \ | | | | | __ _ _ __ __| |_      ____ _ _ __ ___ / ___|___  _ __  / _(_) __ _ _   _ _ __ __ _| |_(_) ___  _ __
| |     / _ \ |  \| | |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ |   / _ \| '_ \| |_| |/ _` | | | | '__/ _` | __| |/ _ \| '_ \
| |___ / ___ \| |\  |  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |__| (_) | | | |  _| | (_| | |_| | | | (_| | |_| | (_) | | | |
 \____/_/   \_\_| \_|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|\____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__|_|\___/|_| |_|
                                                                                         |___/
*/
CANHardwareConfiguration::CANHardwareConfiguration() {
    baudrate = BAUDRATE_500K;
    loopback = false;
    }

/*
  ____    _    _   _ _   _               _                         ____             __ _                       _   _
 / ___|  / \  | \ | | | | | __ _ _ __ __| |_      ____ _ _ __ ___ / ___|___  _ __  / _(_) __ _ _   _ _ __ __ _| |_(_) ___  _ __
| |     / _ \ |  \| | |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ |   / _ \| '_ \| |_| |/ _` | | | | '__/ _` | __| |/ _ \| '_ \
| |___ / ___ \| |\  |  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |__| (_) | | | |  _| | (_| | |_| | | | (_| | |_| | (_) | | | |
 \____/_/   \_\_| \_|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|\____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__|_|\___/|_| |_|
                                                                                         |___/
*/
CANHardwareConfiguration::CANHardwareConfiguration(CAN_BAUDRATE p_baudrate, bool p_loopback) {
    baudrate = p_baudrate;
    loopback = p_loopback;
    }

/*
  ____    _    _   _ _   _               _                         ____             __ _                       _   _
 / ___|  / \  | \ | | | | | __ _ _ __ __| |_      ____ _ _ __ ___ / ___|___  _ __  / _(_) __ _ _   _ _ __ __ _| |_(_) ___  _ __
| |     / _ \ |  \| | |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ |   / _ \| '_ \| |_| |/ _` | | | | '__/ _` | __| |/ _ \| '_ \
| |___ / ___ \| |\  |  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |__| (_) | | | |  _| | (_| | |_| | | | (_| | |_| | (_) | | | |
 \____/_/   \_\_| \_|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|\____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__|_|\___/|_| |_|
                                                                                         |___/
*/
CANHardwareConfiguration::CANHardwareConfiguration(CANBasicTypes::type_string p_baudrate, bool p_loopback) {
    baudrate = CANHardwareConfiguration::baudrate_from_string(p_baudrate);
    loopback = p_loopback;
    }

/*
 _                     _           _            __                             _        _
| |__   __ _ _   _  __| |_ __ __ _| |_ ___     / _|_ __ ___  _ __ ___      ___| |_ _ __(_)_ __   __ _
| '_ \ / _` | | | |/ _` | '__/ _` | __/ _ \   | |_| '__/ _ \| '_ ` _ \    / __| __| '__| | '_ \ / _` |
| |_) | (_| | |_| | (_| | | | (_| | ||  __/   |  _| | | (_) | | | | | |   \__ \ |_| |  | | | | | (_| |
|_.__/ \__,_|\__,_|\__,_|_|  \__,_|\__\___|___|_| |_|  \___/|_| |_| |_|___|___/\__|_|  |_|_| |_|\__, |
                                         |_____|                     |_____|                    |___/
*/
CANHardwareConfiguration::CAN_BAUDRATE CANHardwareConfiguration::baudrate_from_string(CANBasicTypes::type_string description) {
    if (description.compare("BAUDRATE_1M") == 0) {
        return BAUDRATE_1M;
        } else if (description.compare("BAUDRATE_500K") == 0) {
        return BAUDRATE_500K;
        } else if (description.compare("BAUDRATE_250K") == 0) {
            return BAUDRATE_250K;
            } else if (description.compare("BAUDRATE_125K") == 0) {
            return BAUDRATE_125K;
            } else if (description.compare("BAUDRATE_100K") == 0) {
                return BAUDRATE_100K;
                } else if (description.compare("BAUDRATE_50K") == 0) {
                return BAUDRATE_50K;
                    } else if (description.compare("BAUDRATE_20K") == 0) {
                    return BAUDRATE_20K;
                    } else if (description.compare("BAUDRATE_10K") == 0) {
                        return BAUDRATE_10K;
                        } else {
                        return BAUDRATE_5K;
                        }
    };

/*
 _                     _           _          _               _        _
| |__   __ _ _   _  __| |_ __ __ _| |_ ___   | |_ ___     ___| |_ _ __(_)_ __   __ _
| '_ \ / _` | | | |/ _` | '__/ _` | __/ _ \  | __/ _ \   / __| __| '__| | '_ \ / _` |
| |_) | (_| | |_| | (_| | | | (_| | ||  __/  | || (_) |  \__ \ |_| |  | | | | | (_| |
|_.__/ \__,_|\__,_|\__,_|_|  \__,_|\__\___|___\__\___/___|___/\__|_|  |_|_| |_|\__, |
                                         |_____|    |_____|                    |___/
*/
CANBasicTypes::type_string CANHardwareConfiguration::baudrate_to_string(CAN_BAUDRATE baurdate) {
    switch(baurdate) {
        case BAUDRATE_1M:
            return std::string("BAUDRATE_1M");
        case BAUDRATE_500K:
            return std::string("BAUDRATE_500K");
        case BAUDRATE_250K:
            return std::string("BAUDRATE_250K");
        case BAUDRATE_125K:
            return std::string("BAUDRATE_125K");
        case BAUDRATE_100K:
            return std::string("BAUDRATE_100K");
        case BAUDRATE_50K:
            return std::string("BAUDRATE_50K");
        case BAUDRATE_20K:
            return std::string("BAUDRATE_20K");
        case BAUDRATE_10K:
            return std::string("BAUDRATE_10K");
        case BAUDRATE_5K:
            return std::string("BAUDRATE_5K");
        }
    return std::string("");
    };