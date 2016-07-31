#include "FrameworkCanBus/CANEcu.h"
#include "FrameworkCore/CSynch.h"
#include <sstream>

using namespace FrameworkCore;
using namespace FrameworkCanBus;
using namespace std;

/*
  ____    _    _   _ _____
 / ___|  / \  | \ | | ____|___ _   _
| |     / _ \ |  \| |  _| / __| | | |
| |___ / ___ \| |\  | |__| (__| |_| |
 \____/_/   \_\_| \_|_____\___|\__,_|
*/
CANEcu::CANEcu() {
    SetErrorClassPrefix("CANEcu");
    _ecuName.clear();
    _ecuName = "unassigned";
    _transmittedMsgIdList.clear();
    _receivedMsgIdList.clear();
    _transmittedSignalList.clear();
    _receivedSignalList.clear();
    };

/*
  ____    _    _   _ _____
 / ___|  / \  | \ | | ____|___ _   _
| |     / _ \ |  \| |  _| / __| | | |
| |___ / ___ \| |\  | |__| (__| |_| |
 \____/_/   \_\_| \_|_____\___|\__,_|
*/
CANEcu::CANEcu(CANBasicTypes::type_string p_ecu_name) {
    SetErrorClassPrefix("CANEcu");
    _ecuName.clear();
    _ecuName = p_ecu_name;
    _transmittedMsgIdList.clear();
    _receivedMsgIdList.clear();
    _transmittedSignalList.clear();
    _receivedSignalList.clear();
    };

/*
          _
 ___  ___| |_     _ __   __ _ _ __ ___   ___
/ __|/ _ \ __|   | '_ \ / _` | '_ ` _ \ / _ \
\__ \  __/ |_    | | | | (_| | | | | | |  __/
|___/\___|\__|___|_| |_|\__,_|_| |_| |_|\___|
            |_____|
*/
bool CANEcu::set_name(CANBasicTypes::type_string p_ecu_name) {
    _ecuName = p_ecu_name;
    return true;
    };

/*
    _       _     _ ____               _               _ __  __
   / \   __| | __| |  _ \ ___  ___ ___(_)_   _____  __| |  \/  | ___ ___ ___ ___  __ _  __ _  ___
  / _ \ / _` |/ _` | |_) / _ \/ __/ _ \ \ \ / / _ \/ _` | |\/| |/ _ Y __/ __/ __|/ _` |/ _` |/ _ \
 / ___ \ (_| | (_| |  _ <  __/ (_|  __/ |\ V /  __/ (_| | |  | |  __|__ \__ \__ \ (_| | (_| |  __/
/_/   \_\__,_|\__,_|_| \_\___|\___\___|_| \_/ \___|\__,_|_|  |_|\___|___/___/___/\__,_|\__, |\___|
                                                                                       |___/
*/
bool CANEcu::AddReceivedMesssage(CANBasicTypes::type_uinteger msg_id) {
    _receivedMsgIdList.insert(msg_id);
    return true;
    };

/*
 ____                               ____               _               _ __  __
|  _ \ ___ _ __ ___   _____   _____|  _ \ ___  ___ ___(_)_   _____  __| |  \/  | ___ ___ ___ ___  __ _  __ _  ___
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \ |_) / _ \/ __/ _ \ \ \ / / _ \/ _` | |\/| |/ _ Y __/ __/ __|/ _` |/ _` |/ _ \
|  _ <  __/ | | | | | (_) \ V /  __/  _ <  __/ (_|  __/ |\ V /  __/ (_| | |  | |  __|__ \__ \__ \ (_| | (_| |  __/
|_| \_\___|_| |_| |_|\___/ \_/ \___|_| \_\___|\___\___|_| \_/ \___|\__,_|_|  |_|\___|___/___/___/\__,_|\__, |\___|
                                                                                                       |___/
*/
bool CANEcu::RemoveReceivedMesssage(CANBasicTypes::type_uinteger msg_id) {
    _receivedMsgIdList.erase(msg_id);
    return true;
    };

/*
    _       _     _ _____                              _ _   _           _ __  __
   / \   __| | __| |_   _| __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___  __| |  \/  | ___ ___ ___  __ _  __ _  ___
  / _ \ / _` |/ _` | | || '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \/ _` | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 / ___ \ (_| | (_| | | || | | (_| | | | \__ \ | | | | | | |_| ||  __/ (_| | |  | |  __|__ \__ \ (_| | (_| |  __/
/_/   \_\__,_|\__,_| |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|\__,_|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                                                     |___/
*/
bool CANEcu::AddTransmittedMessage(CANBasicTypes::type_uinteger msg_id) {
    _transmittedMsgIdList.insert(msg_id);
    return true;
    };

/*
 ____                              _____                              _ _   _           _ __  __
|  _ \ ___ _ __ ___   _____   ____|_   _| __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___  __| |  \/  | ___ ___ ___  __ _  __ _  ___
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \| || '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \/ _` | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
|  _ <  __/ | | | | | (_) \ V /  __/| || | | (_| | | | \__ \ | | | | | | |_| ||  __/ (_| | |  | |  __|__ \__ \ (_| | (_| |  __/
|_| \_\___|_| |_| |_|\___/ \_/ \___||_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|\__,_|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                                                                    |___/
*/
bool CANEcu::RemoveTransmittedMessage(CANBasicTypes::type_uinteger msg_id) {
    _transmittedMsgIdList.erase(msg_id);
    return true;
    };

/*
    _       _     _ _____                              _ _   _           _ ____  _                   _
   / \   __| | __| |_   _| __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___  __| / ___|(_) __ _ _ __   __ _| |
  / _ \ / _` |/ _` | | || '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \/ _` \___ \| |/ _` | '_ \ / _` | |
 / ___ \ (_| | (_| | | || | | (_| | | | \__ \ | | | | | | |_| ||  __/ (_| |___) | | (_| | | | | (_| | |
/_/   \_\__,_|\__,_| |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|\__,_|____/|_|\__, |_| |_|\__,_|_|
                                                                                   |___/
*/
bool CANEcu::AddTransmittedSignal(CANBasicTypes::type_string signal_name) {
    _transmittedSignalList.insert(signal_name);
    return true;
    };

/*
 ____                              _____                              _ _   _           _ ____  _                   _
|  _ \ ___ _ __ ___   _____   ____|_   _| __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___  __| / ___|(_) __ _ _ __   __ _| |
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \| || '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \/ _` \___ \| |/ _` | '_ \ / _` | |
|  _ <  __/ | | | | | (_) \ V /  __/| || | | (_| | | | \__ \ | | | | | | |_| ||  __/ (_| |___) | | (_| | | | | (_| | |
|_| \_\___|_| |_| |_|\___/ \_/ \___||_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|\__,_|____/|_|\__, |_| |_|\__,_|_|
                                                                                                  |___/
*/
bool CANEcu::RemoveTransmittedSignal(CANBasicTypes::type_string signal_name) {
    _transmittedSignalList.erase(signal_name);
    return true;
    };

/*
    _       _     _ ____               _               _ ____  _                   _
   / \   __| | __| |  _ \ ___  ___ ___(_)_   _____  __| / ___|(_) __ _ _ __   __ _| |
  / _ \ / _` |/ _` | |_) / _ \/ __/ _ \ \ \ / / _ \/ _` \___ \| |/ _` | '_ \ / _` | |
 / ___ \ (_| | (_| |  _ <  __/ (_|  __/ |\ V /  __/ (_| |___) | | (_| | | | | (_| | |
/_/   \_\__,_|\__,_|_| \_\___|\___\___|_| \_/ \___|\__,_|____/|_|\__, |_| |_|\__,_|_|
                                                                 |___/
*/
bool CANEcu::AddReceivedSignal(CANBasicTypes::type_string signal_name) {
    _receivedSignalList.insert(signal_name);
    return true;
    };

/*
 ____                               ____               _               _ ____  _                   _
|  _ \ ___ _ __ ___   _____   _____|  _ \ ___  ___ ___(_)_   _____  __| / ___|(_) __ _ _ __   __ _| |
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \ |_) / _ \/ __/ _ \ \ \ / / _ \/ _` \___ \| |/ _` | '_ \ / _` | |
|  _ <  __/ | | | | | (_) \ V /  __/  _ <  __/ (_|  __/ |\ V /  __/ (_| |___) | | (_| | | | | (_| | |
|_| \_\___|_| |_| |_|\___/ \_/ \___|_| \_\___|\___\___|_| \_/ \___|\__,_|____/|_|\__, |_| |_|\__,_|_|
                                                                                 |___/
*/
bool CANEcu::RemoveReceivedSignal(CANBasicTypes::type_string signal_name) {
    _receivedSignalList.erase(signal_name);
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
std::string CANEcu::GetDescription() {
#define EOL std::endl
    CSynch::IntraProcessLock("CANEcu");
    std::stringstream s;
    s << "[ECU]" << EOL << "Name : " << get_name() << EOL;
    // list of received messages
    s << "--- received messages" << EOL;
    CANEcuTypes::msg_id_set_type::iterator it_msg;
    it_msg = _receivedMsgIdList.begin();
    while (it_msg != _receivedMsgIdList.end()) {
        s << " - 0x" << hex << *it_msg << EOL;
        it_msg++;
        }
    // list of transmitted messages
    s << "--- transmitted messages" << EOL;
    it_msg = _transmittedMsgIdList.begin();
    while (it_msg != _transmittedMsgIdList.end()) {
        s << " - 0x" << hex << *it_msg << EOL;
        it_msg++;
        }
    // list of received signals
    s << "--- received signals" << EOL;
    CANEcuTypes::signal_set_type::iterator it_sig;
    it_sig = _receivedSignalList.begin();
    while (it_sig != _receivedSignalList.end()) {
        s << " - " << *it_sig << EOL;
        it_sig++;
        }
    // list of transmitted signals
    s << "--- transmitted signals" << EOL;
    it_sig = _transmittedSignalList.begin();
    while (it_sig != _transmittedSignalList.end()) {
        s << " - " << *it_sig << EOL;
        it_sig++;
        }
    // return description
    CSynch::IntraProcessUnlock("CANEcu");
    return s.str();
    };