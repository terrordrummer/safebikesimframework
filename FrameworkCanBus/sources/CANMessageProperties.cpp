#include "FrameworkCanBus/CANMessageProperties.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace std;


/*
  ____    _    _   _ __  __                               ____                            _   _
 / ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___|  _ \ _ __ ___  _ __   ___ _ __| |_(_) ___ ___
| |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \ |_) | '__/ _ \| '_ \ / _ \ '__| __| |/ _ Y __|
| |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/  __/| | | (_) | |_) |  __/ |  | |_| |  __|__ \
 \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|_|   |_|  \___/| .__/ \___|_|   \__|_|\___|___/
                                               |___/                     |_|
*/
CANMessageProperties::CANMessageProperties(CANBasicTypes::type_uinteger   p_id, 
                                           CANBasicTypes::type_string     p_name, 
                                           CANMessageTypes::MessageType   p_message_type,
                                           CANBasicTypes::type_uinteger   p_dlc,
                                           CANBasicTypes::type_string     p_transmitter,
                                           CANBasicTypes::type_float      p_cycle_time
                                           ) {
    /* assign values    */
    SetErrorClassPrefix(std::string("CANMessageProperties"));
    _name.clear();
    _transmitter.clear();
    set_message_type(p_message_type);
    set_id(p_id);
    set_name(p_name);
    set_dlc(p_dlc);
    set_transmitter(p_transmitter);
    set_cycle_time(p_cycle_time);
    };

/*
          _       _     _
 ___  ___| |_    (_) __| |
/ __|/ _ \ __|   | |/ _` |
\__ \  __/ |_    | | (_| |
|___/\___|\__|___|_|\__,_|
            |_____|
*/
bool CANMessageProperties::set_id(CANBasicTypes::type_uinteger p_id)
    {
    if (get_message_type() == CANMessageTypes::MSG_STANDARD) {
        /* standard ID, 11 bit used for identifier number    */
        if (p_id >= 2048) {
            std::stringstream s;
            s << "assigning wrong Id value for a STANDARD message: <" << p_id << ">";
            PushError("set_id", s.str());
            return false;
            }
        } else {
            /* extended ID, 29 bit used for identifier number    */
            if (p_id >= 536870912) {
                std::stringstream s;
                s << "assigning wrong Id value for an EXTENDED message: <" << p_id << ">";
                PushError("set_id", s.str());
                return false;                
                }
        }
    /* message id value correct */
    _id = p_id;
    return true;
    };

/*
          _
 ___  ___| |_     _ __   __ _ _ __ ___   ___
/ __|/ _ \ __|   | '_ \ / _` | '_ ` _ \ / _ \
\__ \  __/ |_    | | | | (_| | | | | | |  __/
|___/\___|\__|___|_| |_|\__,_|_| |_| |_|\___|
            |_____|
*/
bool CANMessageProperties::set_name(CANBasicTypes::type_string p_name) {
    /* name is not checked  */
    _name = p_name;
    return true;
    };

/*
          _                                                  _
 ___  ___| |_     _ __ ___   ___ ___ ___  __ _  __ _  ___   | |_ _   _ _ __   ___
/ __|/ _ \ __|   | '_ ` _ \ / _ Y __/ __|/ _` |/ _` |/ _ \  | __| | | | '_ \ / _ \
\__ \  __/ |_    | | | | | |  __|__ \__ \ (_| | (_| |  __/  | |_| |_| | |_) |  __/
|___/\___|\__|___|_| |_| |_|\___|___/___/\__,_|\__, |\___|___\__|\__, | .__/ \___|
            |_____|                            |___/    |_____|  |___/|_|
*/
bool CANMessageProperties::set_message_type(CANMessageTypes::MessageType p_message_type) {
    /* check enumerative value to be correct    */
    if ( p_message_type == CANMessageTypes::MSG_STANDARD || 
        p_message_type == CANMessageTypes::MSG_EXTENDED    ) {
            _messageType = p_message_type;
            return true;
        } else {
            PushError("set_message_type", "assigning wrong message_type value");
            return false;
        }
    };

/*
          _          _ _
 ___  ___| |_     __| | | ___
/ __|/ _ \ __|   / _` | |/ __|
\__ \  __/ |_   | (_| | | (__
|___/\___|\__|___\__,_|_|\___|
            |_____|
*/
bool CANMessageProperties::set_dlc(CANBasicTypes::type_uinteger p_dlc) {
    /* dlc value has to be from 0 to 8  */
    if (p_dlc > 8) {
        std::stringstream s;
        s << "wrong dlc value of " << p_dlc << ", dlc must be in [0..8]";
        PushError("set_dlc", s.str());
        _dlc = 0;
        return false;
        }
    else {
        _dlc = p_dlc;
        return true;
        }
    };

/*
          _      _                                 _ _   _
 ___  ___| |_   | |_ _ __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___ _ __
/ __|/ _ \ __|  | __| '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \ '__|
\__ \  __/ |_   | |_| | | (_| | | | \__ \ | | | | | | |_| ||  __/ |
|___/\___|\__|___\__|_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|_|
            |_____|
*/
bool CANMessageProperties::set_transmitter(CANBasicTypes::type_string p_transmitter) {
    /* transmitter is not checked   */
    _transmitter = p_transmitter;
    return true;
    };

/*
          _                      _         _   _
 ___  ___| |_     ___ _   _  ___| | ___   | |_(_)_ __ ___   ___
/ __|/ _ \ __|   / __| | | |/ __| |/ _ \  | __| | '_ ` _ \ / _ \
\__ \  __/ |_   | (__| |_| | (__| |  __/  | |_| | | | | | |  __/
|___/\___|\__|___\___|\__, |\___|_|\___|___\__|_|_| |_| |_|\___|
            |_____|   |___/           |_____|
*/
bool CANMessageProperties::set_cycle_time(CANBasicTypes::type_float p_cycle_time) {
    /* cycle time is not checked    */
    _cycleTime = p_cycle_time;
    return true;
    };

/*
          _                                               _
 ___  ___| |_     ___ ___  _ __ ___  _ __ ___   ___ _ __ | |_
/ __|/ _ \ __|   / __/ _ \| '_ ` _ \| '_ ` _ \ / _ \ '_ \| __|
\__ \  __/ |_   | (_| (_) | | | | | | | | | | |  __/ | | | |_
|___/\___|\__|___\___\___/|_| |_| |_|_| |_| |_|\___|_| |_|\__|
            |_____|
*/
bool CANMessageProperties::set_comment(CANBasicTypes::type_string p_comment) {
    _comment = p_comment;
    return true;
    };

/*
          _                                        _   _
 ___  ___| |_      _ __  _ __ ___  _ __   ___ _ __| |_(_) ___ ___
/ __|/ _ \ __|    | '_ \| '__/ _ \| '_ \ / _ \ '__| __| |/ _ Y __|
\__ \  __/ |_     | |_) | | | (_) | |_) |  __/ |  | |_| |  __|__ \
|___/\___|\__|____| .__/|_|  \___/| .__/ \___|_|   \__|_|\___|___/
            |_____|_|             |_|
*/
bool CANMessageProperties::set_properties(CANMessageProperties *prop) {
    set_message_type(prop->get_message_type());
    set_id(prop->get_id());
    set_name(prop->get_name());
    set_dlc(prop->get_dlc());
    set_transmitter(prop->get_transmitter());
    set_cycle_time(prop->get_cycle_time());
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
std::string CANMessageProperties::GetDescription() {
    std::stringstream s;
    s << "Name            : " << get_name() << std::endl;
    s << "Id              : 0x" << hex << get_id() << std::endl ;
    if (get_message_type() == CANMessageTypes::MSG_STANDARD) {
        s << "Message type    : STANDARD" << std::endl;
    } else {
        s << "Message type    : EXTENDED" << std::endl;
    };
    s << "Dlc             : " << dec << get_dlc();
    if (get_dlc()==1)
        s << " byte" << std::endl;
    else
        s << " bytes" << std::endl;
    s << "Cycle time      : " << dec << get_cycle_time() << " ms" << std::endl;
    s << "Transmitter ECU : " << get_transmitter() << std::endl;
    s << "Comment         : \"" << get_comment() << "\"";
    return s.str();
    };