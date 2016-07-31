#include "FrameworkCanBus/CANMessage.h"
#include "FrameworkCore/CSynch.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace std;


/*
  ____    _    _   _ __  __
 / ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___
| |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/
 \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|
                                               |___/
*/
CANMessage::CANMessage(CANMessageProperties *properties) {
    // inintialize values
    SetErrorClassPrefix(std::string("CANMessage"));
    _data.clear();
    _signalsSet.clear();
    _signalStartingBit.clear();
    _properties = new CANMessageProperties();
    if (properties != NULL) {
        _properties->set_properties(properties);
        _data.resize(properties->get_dlc());
        }
    };

/*
 /\/|___    _    _   _ __  __
|/\/ ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___
  | |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
  | |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/
   \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                 |___/
*/
CANMessage::~CANMessage() {
    delete _properties;
    };

/*
            _           _                   _         _             _   _                 _     _ _
  __ _  ___| |_     ___(_) __ _ _ __   __ _| |    ___| |_ __ _ _ __| |_(_)_ __   __ _    | |__ (_) |_
 / _` |/ _ \ __|   / __| |/ _` | '_ \ / _` | |   / __| __/ _` | '__| __| | '_ \ / _` |   | '_ \| | __|
| (_| |  __/ |_    \__ \ | (_| | | | | (_| | |   \__ \ || (_| | |  | |_| | | | | (_| |   | |_) | | |_
 \__, |\___|\__|___|___/_|\__, |_| |_|\__,_|_|___|___/\__\__,_|_|   \__|_|_| |_|\__, |___|_.__/|_|\__|
 |___/        |_____|     |___/             |_____|                             |___/_____|
*/
CANBasicTypes::type_uinteger CANMessage::get_signal_starting_bit(CANBasicTypes::type_string signal_name) {
    CANMessageTypes::signal_starting_bit_type::iterator it = _signalStartingBit.find(signal_name);
    if (it != _signalStartingBit.end())
        return it->second;
    else {
        PushError("get_signal_starting_bit", string("signal -") + signal_name + string("- not found in message"));
        return 0;
        }
    };

/*
          _                                        _   _
 ___  ___| |_      _ __  _ __ ___  _ __   ___ _ __| |_(_) ___ ___
/ __|/ _ \ __|    | '_ \| '__/ _ \| '_ \ / _ \ '__| __| |/ _ Y __|
\__ \  __/ |_     | |_) | | | (_) | |_) |  __/ |  | |_| |  __|__ \
|___/\___|\__|____| .__/|_|  \___/| .__/ \___|_|   \__|_|\___|___/
            |_____|_|             |_|
*/
bool CANMessage::set_properties(CANMessageProperties *prop) {
    delete _properties;
    _properties = new CANMessageProperties();
    _properties->set_properties(prop);
    _data.resize(prop->get_dlc());
    // check for errors
    if (_properties->HasError()) {
        PropagateError("set_properties", _properties);
        return false;
        }
    return true;
    };

/*
          _          _       _
 ___  ___| |_     __| | __ _| |_ __ _
/ __|/ _ \ __|   / _` |/ _` | __/ _` |
\__ \  __/ |_   | (_| | (_| | || (_| |
|___/\___|\__|___\__,_|\__,_|\__\__,_|
            |_____|
*/
bool CANMessage::set_data(std::vector<char> &data) {
    _data = data;
    return true;
    };

/*
          _          _       _
 ___  ___| |_     __| | __ _| |_ __ _
/ __|/ _ \ __|   / _` |/ _` | __/ _` |
\__ \  __/ |_   | (_| | (_| | || (_| |
|___/\___|\__|___\__,_|\__,_|\__\__,_|
            |_____|
*/
bool CANMessage::set_data(void * data) {
    unsigned char msg_len = get_properties()->get_dlc();
    // check for data size
    if (msg_len != _data.size())
        _data.resize(msg_len);
    // store data
    memcpy(&(_data[0]), data, msg_len);
    return true;
    };

/*
 ____               _   ____        _
|  _ \ ___ ___  ___| |_|  _ \  __ _| |_ __ _
| |_) / _ Y __|/ _ \ __| | | |/ _` | __/ _` |
|  _ <  __|__ \  __/ |_| |_| | (_| | || (_| |
|_| \_\___|___/\___|\__|____/ \__,_|\__\__,_|
*/
void CANMessage::ResetData() {
    _data.assign(_data.size(),0);
    };

/*
          _           _                   _         _             _   _                 _     _ _
 ___  ___| |_     ___(_) __ _ _ __   __ _| |    ___| |_ __ _ _ __| |_(_)_ __   __ _    | |__ (_) |_
/ __|/ _ \ __|   / __| |/ _` | '_ \ / _` | |   / __| __/ _` | '__| __| | '_ \ / _` |   | '_ \| | __|
\__ \  __/ |_    \__ \ | (_| | | | | (_| | |   \__ \ || (_| | |  | |_| | | | | (_| |   | |_) | | |_
|___/\___|\__|___|___/_|\__, |_| |_|\__,_|_|___|___/\__\__,_|_|   \__|_|_| |_|\__, |___|_.__/|_|\__|
            |_____|     |___/             |_____|                             |___/_____|
*/
bool CANMessage::set_signal_starting_bit(CANBasicTypes::type_string signal_name, CANBasicTypes::type_integer p_starting_bit){
    CANMessageTypes::signal_starting_bit_type::iterator it = _signalStartingBit.find(signal_name);
    if (it == _signalStartingBit.end()) {
        // signal not packed in the message
        std::stringstream s;
        s << "signal -" << signal_name << "- not packed in message 0x" << hex << get_properties()->get_id();
        PushError("set_signal_starting_bit", s.str());
        return false;
        }
    // signal founded
    it->second = p_starting_bit;
    return true;
    };

/*
    _       _     _ ____  _                   _
   / \   __| | __| / ___|(_) __ _ _ __   __ _| |
  / _ \ / _` |/ _` \___ \| |/ _` | '_ \ / _` | |
 / ___ \ (_| | (_| |___) | | (_| | | | | (_| | |
/_/   \_\__,_|\__,_|____/|_|\__, |_| |_|\__,_|_|
                            |___/
*/
bool CANMessage::AddSignal(CANBasicTypes::type_string signal_name, CANBasicTypes::type_integer p_starting_bit) {
    // collision has to be chjecked BEFORE adding the signal
    // a signal is not allowed to compare twice in the message so first its existence is first tested
    if (_signalsSet.find(signal_name) != _signalsSet.end()) {
        std::stringstream s;
        s << "signal -" << signal_name << "- already added in message -" << get_properties()->get_name() << "-";
        PushError("AddSignal", s.str());
        return false;
        }
    // signal is not present, add it to the message
    _signalsSet.insert(signal_name);
    _signalStartingBit[signal_name] = p_starting_bit;
    return true;
    };

/*
 ____                               ____  _                   _
|  _ \ ___ _ __ ___   _____   _____/ ___|(_) __ _ _ __   __ _| |
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \___ \| |/ _` | '_ \ / _` | |
|  _ <  __/ | | | | | (_) \ V /  __/___) | | (_| | | | | (_| | |
|_| \_\___|_| |_| |_|\___/ \_/ \___|____/|_|\__, |_| |_|\__,_|_|
                                            |___/
*/
bool CANMessage::RemoveSignal(CANBasicTypes::type_string signal_name) {
    CANMessageTypes::signal_set_type::iterator it = _signalsSet.find(signal_name);
    if (it == _signalsSet.end()) {
        // signal is not packed in the message
        std::stringstream s;
        s << "signal -" << signal_name << "- is not in message 0x" << hex << get_properties()->get_id() ;
        PushError("RemoveSignal", s.str());
        return false;
        };
    // signal exist, remove it from the message
    _signalsSet.erase(signal_name);
    _signalStartingBit.erase(signal_name);
    return true;
    };

/*
  ____      _   ____                     _       _   _
 / ___| ___| |_|  _ \  ___ ___  ___ _ __(_)_ __ | |_(_) ___  _ __
| |  _ / _ \ __| | | |/ _ Y __|/ __| '__| | '_ \| __| |/ _ \| '_ \
| |_| |  __/ |_| |_| |  __|__ \ (__| |  | | |_) | |_| | (_) | | | |
 \____|\___|\__|____/ \___|___/\___|_|  |_| .__/ \__|_|\___/|_| |_|
                                          |_|
*/
std::string CANMessage::GetDescription() {
#define EOL std::endl
    std::stringstream s;
    // write message properties
    s << "[Message] " << EOL;
    s << _properties->GetDescription() << EOL;
    s << "List of packed signals " << EOL;
    CANMessageTypes::signal_set_type::iterator it = _signalsSet.begin();
    for (;it != _signalsSet.end(); it++) {
        s << "  - " << *it << EOL;
        }
    return s.str();
    };