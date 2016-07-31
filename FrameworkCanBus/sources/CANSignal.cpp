#include "FrameworkCanBus/CANSignal.h"
#include <sstream>

using namespace FrameworkCore;
using namespace FrameworkCanBus;

#define signal_mutex_name "SIGNAL SET"


/*
  ____    _    _   _ ____  _                   _
 / ___|  / \  | \ | / ___|(_) __ _ _ __   __ _| |
| |     / _ \ |  \| \___ \| |/ _` | '_ \ / _` | |
| |___ / ___ \| |\  |___) | | (_| | | | | (_| | |
 \____/_/   \_\_| \_|____/|_|\__, |_| |_|\__,_|_|
                             |___/
*/
CANSignal::CANSignal(CANSignalProperties *properties) : _value(0), _enabled(true) {
    SetErrorClassPrefix(std::string("CANSignal"));
    _properties = new CANSignalProperties();
    if (properties != NULL)
        *_properties = *properties;
    _messageSet.clear();
    _enabled = true;
    _disabledValue = _properties->get_minval();
    };

/*
 /\/|___    _    _   _ ____  _                   _
|/\/ ___|  / \  | \ | / ___|(_) __ _ _ __   __ _| |
  | |     / _ \ |  \| \___ \| |/ _` | '_ \ / _` | |
  | |___ / ___ \| |\  |___) | | (_| | | | | (_| | |
   \____/_/   \_\_| \_|____/|_|\__, |_| |_|\__,_|_|
                               |___/
*/
CANSignal::~CANSignal() {
    delete _properties;
    };

/*
          _                _
 ___  ___| |_  __   ____ _| |_   _  ___
/ __|/ _ \ __| \ \ / / _` | | | | |/ _ \
\__ \  __/ |_   \ V / (_| | | |_| |  __/
|___/\___|\__|___\_/ \__,_|_|\__,_|\___|
            |_____|
*/
bool CANSignal::set_value(CANBasicTypes::type_float value) {
    _synch.InstanceLock(signal_mutex_name);
    // saturate the value between minimum and maximum value
    if (value < _properties->get_minval()) {
        value = _properties->get_minval();
        };
    if (value > _properties->get_maxval()) {
        value = _properties->get_maxval();
        };
    _value = value;
    _synch.InstanceUnlock(signal_mutex_name);
    return true;
    };

/*
    _       _     _ __  __
   / \   __| | __| |  \/  | ___ ___ ___  __ _  __ _  ___
  / _ \ / _` |/ _` | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 / ___ \ (_| | (_| | |  | |  __|__ \__ \ (_| | (_| |  __/
/_/   \_\__,_|\__,_|_|  |_|\___|___/___/\__,_|\__, |\___|
                                              |___/
*/
bool CANSignal::AddMessage(CANBasicTypes::type_string name) {
    _synch.InstanceLock(signal_mutex_name);
    CANSignalTypes::message_set_type::iterator it = _messageSet.find(name);
    if (it != _messageSet.end()) {
        std::stringstream s;
        s << "trying to adding packing message <" << name << "> already present in signal " << _properties->get_name() << std::endl;
        PushError("AddMessage", s.str());
        _synch.InstanceUnlock(signal_mutex_name);
        return false;
        };
    _messageSet.insert(name);
    _synch.InstanceUnlock(signal_mutex_name);
    return true;
    };

/*
 ____                               __  __
|  _ \ ___ _ __ ___   _____   _____|  \/  | ___ ___ ___  __ _  __ _  ___
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
|  _ <  __/ | | | | | (_) \ V /  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
|_| \_\___|_| |_| |_|\___/ \_/ \___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                              |___/
*/
bool CANSignal::RemoveMessage(CANBasicTypes::type_string name) {
    _synch.InstanceLock(signal_mutex_name);
    CANSignalTypes::message_set_type::iterator it = _messageSet.find(name);
    if (it == _messageSet.end()) {
        std::stringstream s;
        s << ": trying to remove a non-packing message <" << name << "> from signal " << _properties->get_name() << std::endl;
        PushError("RemoveMessage", s.str());
        _synch.InstanceUnlock(signal_mutex_name);
        return false;
        }
    _messageSet.erase(name);
    _synch.InstanceUnlock(signal_mutex_name);
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
std::string CANSignal::GetDescription() {
    _synch.InstanceLock(signal_mutex_name);
#define EOL std::endl
    std::stringstream s;
    s << "[Signal]" << EOL;
    s << _properties->GetDescription() << EOL;
    if (_messageSet.size() == 0)
        s << "Packed in message  : <not assigned>";
    else if (_messageSet.size() == 1)
        s << "Packed in message  : ";
    else
        s << "Packed in messages : ";
    CANSignalTypes::message_set_type::iterator it = _messageSet.begin();
    while (it != _messageSet.end()) {
        s << *it << "; ";
        it++;
        }
    s << EOL;
    _synch.InstanceUnlock(signal_mutex_name);
    return s.str();
    };