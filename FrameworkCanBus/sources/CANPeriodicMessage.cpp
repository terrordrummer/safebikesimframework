#include "FrameworkCanBus/CANPeriodicMessage.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace FrameworkCore;

/*
  ____    _    _   _    _        _   _           __  __
 / ___|  / \  | \ | |  / \   ___| |_(_)_   _____|  \/  | ___ ___ ___  __ _  __ _  ___
| |     / _ \ |  \| | / _ \ / __| __| \ \ / / _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |___ / ___ \| |\  |/ ___ \ (__| |_| |\ V /  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
 \____/_/   \_\_| \_/_/   \_\___|\__|_| \_/ \___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                           |___/
*/
CANPeriodicMessage::CANPeriodicMessage() {
    // standard initialization
    SetErrorClassPrefix("CANPeriodicMessage");
    max_lifetime        = -1.0;
    max_n_transmissions = 0;
    HW_device.clear();
    HW_device="default";
    };

/*
  ____    _    _   _    _        _   _           __  __
 / ___|  / \  | \ | |  / \   ___| |_(_)_   _____|  \/  | ___ ___ ___  __ _  __ _  ___
| |     / _ \ |  \| | / _ \ / __| __| \ \ / / _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |___ / ___ \| |\  |/ ___ \ (__| |_| |\ V /  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
 \____/_/   \_\_| \_/_/   \_\___|\__|_| \_/ \___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                           |___/
*/
CANPeriodicMessage::CANPeriodicMessage(CANMessage *msg, CANBasicTypes::type_string p_HW_name) {
    // standard initialization
    SetErrorClassPrefix("CANPeriodicMessage");
    max_lifetime        = -1.0;
    max_n_transmissions = 0;
    HW_device.clear();
    HW_device = p_HW_name;

    // copy message properties
    get_properties()->set_id(msg->get_properties()->get_id());
    get_properties()->set_name(msg->get_properties()->get_name());
    get_properties()->set_message_type(msg->get_properties()->get_message_type());
    get_properties()->set_dlc(msg->get_properties()->get_dlc());
    get_properties()->set_transmitter(msg->get_properties()->get_transmitter());
    get_properties()->set_cycle_time(msg->get_properties()->get_cycle_time());
    get_properties()->set_comment(msg->get_properties()->get_comment());
    
    // duplicate signal list
    _signalsSet.clear();
    _signalStartingBit.clear();
    CANMessageTypes::signal_set_type sset = msg->get_signals();
    CANMessageTypes::signal_set_type::iterator it = sset.begin();
    while (it != sset.end()) {
        AddSignal(*it, msg->get_signal_starting_bit(*it));
        it++;
        };

    };

/*
  ____    _    _   _    _        _   _           __  __
 / ___|  / \  | \ | |  / \   ___| |_(_)_   _____|  \/  | ___ ___ ___  __ _  __ _  ___
| |     / _ \ |  \| | / _ \ / __| __| \ \ / / _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |___ / ___ \| |\  |/ ___ \ (__| |_| |\ V /  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
 \____/_/   \_\_| \_/_/   \_\___|\__|_| \_/ \___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                           |___/
*/
CANPeriodicMessage::CANPeriodicMessage(CANMessage *msg, CANBasicTypes::type_uinteger p_max_n_transmissions, CTime::time_type p_max_lifetime, CANBasicTypes::type_string p_HW_name) {
    // standard initialization
    SetErrorClassPrefix("CANPeriodicMessage");
    max_lifetime        = p_max_lifetime;
    max_n_transmissions = p_max_n_transmissions;
    HW_device = p_HW_name;

    // duplicate message properties
    CANMessageProperties *prop = new CANMessageProperties();
    prop->set_id(msg->get_properties()->get_id());
    prop->set_name(msg->get_properties()->get_name());
    prop->set_message_type(msg->get_properties()->get_message_type());
    prop->set_dlc(msg->get_properties()->get_dlc());
    prop->set_transmitter(msg->get_properties()->get_transmitter());
    prop->set_cycle_time(msg->get_properties()->get_cycle_time());
    prop->set_comment(msg->get_properties()->get_comment());
    set_properties(prop);
    
    // duplicate signal list
    _signalsSet.clear();
    _signalStartingBit.clear();
    CANMessageTypes::signal_set_type sset = msg->get_signals();
    CANMessageTypes::signal_set_type::iterator it = sset.begin();
    while (it != sset.end()) {
        AddSignal(*it, msg->get_signal_starting_bit(*it));
        it++;
        };
    delete prop;
    };



/*
 /\/|___    _    _   _    _        _   _           __  __
|/\/ ___|  / \  | \ | |  / \   ___| |_(_)_   _____|  \/  | ___ ___ ___  __ _  __ _  ___
  | |     / _ \ |  \| | / _ \ / __| __| \ \ / / _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
  | |___ / ___ \| |\  |/ ___ \ (__| |_| |\ V /  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
   \____/_/   \_\_| \_/_/   \_\___|\__|_| \_/ \___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                             |___/
*/
CANPeriodicMessage::~CANPeriodicMessage() {
    };

/*
  ____      _   ____                     _       _   _
 / ___| ___| |_|  _ \  ___ ___  ___ _ __(_)_ __ | |_(_) ___  _ __
| |  _ / _ \ __| | | |/ _ Y __|/ __| '__| | '_ \| __| |/ _ \| '_ \
| |_| |  __/ |_| |_| |  __|__ \ (__| |  | | |_) | |_| | (_) | | | |
 \____|\___|\__|____/ \___|___/\___|_|  |_| .__/ \__|_|\___/|_| |_|
                                          |_|
*/
std::string CANPeriodicMessage::GetDescription() {
#define EOL std::endl
    // write message properties
    std::stringstream s;
        s << "[Active Message] " << EOL;
    s << _properties->GetDescription() << EOL;
    s << "List of packed signals " << EOL;
    CANMessageTypes::signal_set_type::iterator it = _signalsSet.begin();
    for (;it != _signalsSet.end(); it++) {
        s << "  - " << *it << EOL;
        }
    s << "max lifetime [sec] : " << get_max_lifetime() << EOL;
    s << "max transmissions  : " << get_max_n_transmissions() << EOL;
    return std::string("");
    };