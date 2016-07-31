#include "CPeakDevice.h"
#include "FrameworkCanBus/CANHardwareConfiguration.h"
#include "FrameworkCore/CTime.h"

using namespace FrameworkCanBus;
using namespace FrameworkCore;

#define DEFAULT_TIMEOUT 3600000 // 1 hour wait before timeout

/*
  ____ ____            _    ____             _
 / ___|  _ \ ___  __ _| | _|  _ \  _____   _(_) ___ ___
| |   | |_) / _ \/ _` | |/ / | | |/ _ \ \ / / |/ __/ _ \
| |___|  __/  __/ (_| |   <| |_| |  __/\ V /| | (_|  __/
 \____|_|   \___|\__,_|_|\_\____/ \___| \_/ |_|\___\___|
*/
CPeakDevice::CPeakDevice() {
    _hwInterfaceName = std::string("CPeakDevice");
    }

/*
 /\/|___ ____            _    ____             _
|/\/ ___|  _ \ ___  __ _| | _|  _ \  _____   _(_) ___ ___
  | |   | |_) / _ \/ _` | |/ / | | |/ _ \ \ / / |/ __/ _ \
  | |___|  __/  __/ (_| |   <| |_| |  __/\ V /| | (_|  __/
   \____|_|   \___|\__,_|_|\_\____/ \___| \_/ |_|\___\___|
*/
CPeakDevice::~CPeakDevice() {
    
    }

/*
 ____       _    ____ _                            _
/ ___|  ___| |_ / ___| |__   __ _ _ __  _ __   ___| |
\___ \ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ |
 ___) |  __/ |_| |___| | | | (_| | | | | | | |  __/ |
|____/ \___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/
void CPeakDevice::SetChannel(unsigned int p_channel) {
    if (p_channel < 0 || p_channel > 7) p_channel = 0;
    _channel = _channelOffset + p_channel;
    };

/*
  ____      _   ____                     _       _   _
 / ___| ___| |_|  _ \  ___ ___  ___ _ __(_)_ __ | |_(_) ___  _ __
| |  _ / _ \ __| | | |/ _ Y __|/ __| '__| | '_ \| __| |/ _ \| '_ \
| |_| |  __/ |_| |_| |  __|__ \ (__| |  | | |_) | |_| | (_) | | | |
 \____|\___|\__|____/ \___|___/\___|_|  |_| .__/ \__|_|\___/|_| |_|
                                          |_|
*/
std::string CPeakDevice::GetDescription() {
    return std::string("Peak-System device");
    };

/*
 _____        _
|_   _|__ ___| |_
  | |/ _ Y __| __|
  | |  __|__ \ |_
  |_|\___|___/\__|
*/
bool CPeakDevice::Test() {
    _tested = true;
    TPCANStatus CAN_res = CAN_Initialize( _channel,  PCAN_BAUD_500K,  0, 0, 0);
    if (CAN_res == PCAN_ERROR_OK) {
        // get device ID
        long nr;
        CAN_res = CAN_GetValue(_channel, PCAN_DEVICE_NUMBER, (void *)&nr, sizeof(long));
        if (CAN_res == PCAN_ERROR_OK) {
            setDeviceID(nr);
            }
        // uninitialize hardware
        CAN_res = CAN_Uninitialize(_channel);
        if (CAN_res == PCAN_ERROR_OK)
            return true;
        else
            return false;
        }
    else {
        CAN_res = CAN_Uninitialize(_channel);
        return false;
        }
    };

/*
  ___
 / _ \ _ __   ___ _ __
| | | | '_ \ / _ \ '_ \
| |_| | |_) |  __/ | | |
 \___/| .__/ \___|_| |_|
      |_|
*/
bool CPeakDevice::Open(const CANHardwareConfiguration config) {
    //cout << "Peak-system Open" << std::endl;
    WORD bitrate;
    switch(config.baudrate) {
        case CANHardwareConfiguration::BAUDRATE_1M:
                bitrate = PCAN_BAUD_1M;
            break;
        case CANHardwareConfiguration::BAUDRATE_500K:
                bitrate = PCAN_BAUD_500K;
            break;
        case CANHardwareConfiguration::BAUDRATE_250K:
                bitrate = PCAN_BAUD_250K;
            break;
        case CANHardwareConfiguration::BAUDRATE_125K:
                bitrate = PCAN_BAUD_125K;
            break;
        case CANHardwareConfiguration::BAUDRATE_100K:
                bitrate = PCAN_BAUD_100K;
            break;
        case CANHardwareConfiguration::BAUDRATE_50K:
                bitrate = PCAN_BAUD_50K;
            break;
        case CANHardwareConfiguration::BAUDRATE_20K:
                bitrate = PCAN_BAUD_20K;
            break;
        case CANHardwareConfiguration::BAUDRATE_10K:
                bitrate = PCAN_BAUD_10K;
            break;
        case CANHardwareConfiguration::BAUDRATE_5K:
                bitrate = PCAN_BAUD_5K;
                break;
        default:
                bitrate = PCAN_BAUD_500K;
            break;
        }
    // set loopback flag (parent class)
    _loopback = config.loopback;
    
    TPCANStatus CAN_res = CAN_Initialize( _channel,  bitrate,  0, 0, 0);
    if (CAN_res == PCAN_ERROR_OK) {
        _open = true;
        return true;
    } else {
        _open = false;
        return false;
        }
    };

/*
 _____                              _ _
|_   _| __ __ _ _ __  ___ _ __ ___ (_) |_
  | || '__/ _` | '_ \/ __| '_ ` _ \| | __|
  | || | | (_| | | | \__ \ | | | | | | |_
  |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|
*/
bool CPeakDevice::Transmit(CANMessage *msg) {
    // device should be open
    if (!_open) return false;

    // fill message structure
    TPCANMsg message;
    message.ID = msg->get_properties()->get_id();
    message.LEN = msg->get_properties()->get_dlc();
    if (msg->get_properties()->get_message_type() == CANMessageTypes::MSG_STANDARD)    
        message.MSGTYPE = PCAN_MESSAGE_STANDARD;
    else
        message.MSGTYPE = PCAN_MESSAGE_EXTENDED;
    // copy message content
    if (message.LEN>0)
        memcpy((void *)message.DATA, msg->GetDataPointer(), message.LEN);
    // write message
    if (CAN_Write(_channel, &message) == PCAN_ERROR_OK)
        return true;
    else
        return false;
    };

/*
 ____               _
|  _ \ ___  ___ ___(_)_   _____
| |_) / _ \/ __/ _ \ \ \ / / _ \
|  _ <  __/ (_|  __/ |\ V /  __/
|_| \_\___|\___\___|_| \_/ \___|
*/
bool CPeakDevice::Receive(CANMessage *msg) {
    return Receive(msg, DEFAULT_TIMEOUT);
    };

bool CPeakDevice::Receive(CANMessage *msg, unsigned int timeout_ms) {
    // device should be open
    if (!_open) return false;

    TPCANMsg        message;
    TPCANTimestamp  timestamp;

    message.LEN = 0;
    // read message
    TPCANStatus res = PCAN_ERROR_QRCVEMPTY;
    CTime t;
    t.Tic();
    while (res == PCAN_ERROR_QRCVEMPTY)  {
        res = CAN_Read(_channel, &message, &timestamp);
        if (res != PCAN_ERROR_OK) {
            switch (res) {
            case PCAN_ERROR_XMTFULL:
                PushError("Receive", "PCAN_ERROR_XMTFULL");
                return false;
            case PCAN_ERROR_OVERRUN:
                PushError("Receive", "PCAN_ERROR_OVERRUN");
                return false;
            case PCAN_ERROR_BUSLIGHT:
                PushError("Receive", "PCAN_ERROR_BUSLIGHT");
                return false;
            case PCAN_ERROR_BUSHEAVY:
                PushError("Receive", "PCAN_ERROR_BUSHEAVY");
                return false;
            case PCAN_ERROR_BUSOFF:
                PushError("Receive", "PCAN_ERROR_BUSOFF");
                return false;
            case PCAN_ERROR_ANYBUSERR:
                PushError("Receive", "PCAN_ERROR_ANYBUSERR");
                return false;
            case PCAN_ERROR_QOVERRUN:
                PushError("Receive", "PCAN_ERROR_QOVERRUN");
                return false;
            case PCAN_ERROR_QXMTFULL:
                PushError("Receive", "PCAN_ERROR_QXMTFULL");
                return false;
            case PCAN_ERROR_REGTEST:
                PushError("Receive", "PCAN_ERROR_REGTEST");
                return false;
            case PCAN_ERROR_NODRIVER:
                PushError("Receive", "PCAN_ERROR_NODRIVER");
                return false;
            case PCAN_ERROR_HWINUSE:
                PushError("Receive", "PCAN_ERROR_HWINUSE");
                return false;
            case PCAN_ERROR_NETINUSE:
                PushError("Receive", "PCAN_ERROR_NETINUSE");
                return false;
            case PCAN_ERROR_ILLHW:
                PushError("Receive", "PCAN_ERROR_ILLHW");
                return false;
            case PCAN_ERROR_ILLNET:
                PushError("Receive", "PCAN_ERROR_ILLNET");
                return false;
            case PCAN_ERROR_ILLCLIENT:
                PushError("Receive", "PCAN_ERROR_ILLCLIENT");
                return false;
            case PCAN_ERROR_RESOURCE:
                PushError("Receive", "PCAN_ERROR_RESOURCE");
                return false;
            case PCAN_ERROR_ILLPARAMTYPE:
                PushError("Receive", "PCAN_ERROR_ILLPARAMTYPE");
                return false;
            case PCAN_ERROR_ILLPARAMVAL:
                PushError("Receive", "PCAN_ERROR_ILLPARAMVAL");
                return false;
            case PCAN_ERROR_UNKNOWN:
                PushError("Receive", "PCAN_ERROR_UNKNOWN");
                return false;
            case PCAN_ERROR_INITIALIZE:
                PushError("Receive", "PCAN_ERROR_INITIALIZE");
                return false;
            case PCAN_ERROR_QRCVEMPTY:
                if (t.Toc()*1000 < timeout_ms)
                    Sleep(1);
                else {
                    // timed out
                    return false;
                }
                break;
            }
        }
    }

    // fill received message's properties
    CANMessageProperties prop;
    prop.set_id(message.ID);
    prop.set_dlc(message.LEN);
    if (message.MSGTYPE == PCAN_MESSAGE_STANDARD)
        prop.set_message_type(CANMessageTypes::MSG_STANDARD);
    else
        prop.set_message_type(CANMessageTypes::MSG_EXTENDED);
    // fill received message's data
    std::vector<char> data;
    data.resize(message.LEN);
    for( int i=0; i<message.LEN; i++)
        data[i] = message.DATA[i];
    // store received data
    msg->set_properties(&prop);
    msg->set_data(data);
    
    return true;
}

/*
  ____ _
 / ___| | ___  ___  ___
| |   | |/ _ \/ __|/ _ \
| |___| | (_) \__ \  __/
 \____|_|\___/|___/\___|
*/
bool CPeakDevice::Close() {
    CAN_Uninitialize(_channel);
    _open = false;
    return true;
    };

/*
 _____                 _         ____               _       _             ____         __  __
| ____|_ __ ___  _ __ | |_ _   _|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _| __ ) _   _ / _|/ _| ___ _ __
|  _| | '_ ` _ \| '_ \| __| | | | |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` |  _ \| | | | |_| |_ / _ \ '__|
| |___| | | | | | |_) | |_| |_| |  _ <  __/ (_|  __/ |\ V /| | | | | (_| | |_) | |_| |  _|  _|  __/ |
|_____|_| |_| |_| .__/ \__|\__, |_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |____/ \__,_|_| |_|  \___|_|
                |_|        |___/                                    |___/
*/
void CPeakDevice::EmptyReceivingBuffer() {};
