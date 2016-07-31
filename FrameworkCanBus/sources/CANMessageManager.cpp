#include "FrameworkCanBus/CANMessageManager.h"
#include "FrameworkCore/CSynch.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace std;

/*
  ____    _    _   _ __  __                               __  __
 / ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___|  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \ |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/ |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                               |___/                               |___/
*/
CANMessageManager::CANMessageManager() {
    _messageById.clear();
    _messageByName.clear();
    SetErrorClassPrefix(std::string("CANMessageManager"));
    //synch = CSynch();
    };

/*
 /\/|___    _    _   _ __  __                               __  __
|/\/ ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___|  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \ |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/ |  | | (_| | | | | (_| | (_| |  __/ |
   \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                 |___/                               |___/
*/
CANMessageManager::~CANMessageManager() {
    _synch.InstanceLock("CANMessageManager_access");
    // delete all allocated messages
    _messageByIdType::iterator it = _messageById.begin();
    while (it != _messageById.end()) {
        // delete message
        delete it->second;
        it++;
        }
    // clear arrays and exit
    _messageById.clear();
    _messageByName.clear();
    _synch.InstanceUnlock("CANMessageManager_access");
    };

/*
            _
  __ _  ___| |_     _ __ ___   ___ ___ ___  __ _  __ _  ___
 / _` |/ _ \ __|   | '_ ` _ \ / _ Y __/ __|/ _` |/ _` |/ _ \
| (_| |  __/ |_    | | | | | |  __|__ \__ \ (_| | (_| |  __/
 \__, |\___|\__|___|_| |_| |_|\___|___/___/\__,_|\__, |\___|
 |___/        |_____|                            |___/
*/
CANMessage *CANMessageManager::get_message(CANBasicTypes::type_uinteger id) {
    _synch.InstanceLock("CANMessageManager_access");
    _messageByIdType::iterator it = _messageById.find(id);
    if (it == _messageById.end()) {
        std::stringstream s;
        s << "failed to get message with id <" << id << ">";
        PushError("get_message", s.str());
        _synch.InstanceUnlock("CANMessageManager_access");
        return NULL;
        }
    _synch.InstanceUnlock("CANMessageManager_access");
    return it->second;
    };

/*
            _
  __ _  ___| |_     _ __ ___   ___ ___ ___  __ _  __ _  ___
 / _` |/ _ \ __|   | '_ ` _ \ / _ Y __/ __|/ _` |/ _` |/ _ \
| (_| |  __/ |_    | | | | | |  __|__ \__ \ (_| | (_| |  __/
 \__, |\___|\__|___|_| |_| |_|\___|___/___/\__,_|\__, |\___|
 |___/        |_____|                            |___/
*/
CANMessage *CANMessageManager::get_message(CANBasicTypes::type_string name) {
    _synch.InstanceLock("CANMessageManager_access");
    _messageByNameType::iterator it = _messageByName.find(name);
    if (it == _messageByName.end()) {
        std::stringstream s;
        s << "failed to get message with name <" << name << ">";
        PushError("get_message", s.str());
        _synch.InstanceUnlock("CANMessageManager_access");
        return NULL;
        }
    _synch.InstanceUnlock("CANMessageManager_access");
    return it->second;
    };

/*
 __  __                               _____      _     _
|  \/  | ___ ___ ___  __ _  __ _  ___| ____|_  _(_)___| |_
| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \  _| \ \/ / / __| __|
| |  | |  __|__ \__ \ (_| | (_| |  __/ |___ >  <| \__ \ |_
|_|  |_|\___|___/___/\__,_|\__, |\___|_____/_/\_\_|___/\__|
                           |___/
*/
bool CANMessageManager::MessageExist(CANBasicTypes::type_string name) {
    _synch.InstanceLock("CANMessageManager_access");
    if (_messageByName.find(name) != _messageByName.end()) {
        _synch.InstanceUnlock("CANMessageManager_access");
        return true;
        } else {
            _synch.InstanceUnlock("CANMessageManager_access");
            return false;
        };
    };

/*
 __  __                               _____      _     _
|  \/  | ___ ___ ___  __ _  __ _  ___| ____|_  _(_)___| |_
| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \  _| \ \/ / / __| __|
| |  | |  __|__ \__ \ (_| | (_| |  __/ |___ >  <| \__ \ |_
|_|  |_|\___|___/___/\__,_|\__, |\___|_____/_/\_\_|___/\__|
                           |___/
*/
bool CANMessageManager::MessageExist(CANBasicTypes::type_uinteger id) {
    _synch.InstanceLock("CANMessageManager_access");
    if (_messageById.find(id) != _messageById.end()) {
        _synch.InstanceUnlock("CANMessageManager_access");
        return true;
        } else {
            _synch.InstanceUnlock("CANMessageManager_access");
            return false;
        };
    };

/*
    _       _     _ __  __
   / \   __| | __| |  \/  | ___ ___ ___  __ _  __ _  ___
  / _ \ / _` |/ _` | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 / ___ \ (_| | (_| | |  | |  __|__ \__ \ (_| | (_| |  __/
/_/   \_\__,_|\__,_|_|  |_|\___|___/___/\__,_|\__, |\___|
                                              |___/
*/
bool CANMessageManager::AddMessage(CANMessageProperties *properties) {
    _synch.InstanceLock("CANMessageManager_access");
    CANMessage *msg = NULL;
    // check existence of message
    _messageByIdType::iterator it = _messageById.find(properties->get_id());
    if (it != _messageById.end())  {
        // message already exist, point to the existing message
        std::stringstream s;
        s << "trying to add a message already present with id 0x" <<properties->get_id();
        PushError("AddMessage", s.str());
        _synch.InstanceUnlock("CANMessageManager_access");
        return false;
    }
    // message does not exist yet, a new message is created
    msg = new CANMessage(properties);
    // set message's properties
    msg->set_properties(properties);

    if (msg->HasError()) {
        // some error occurred, push the error message and reset msg error state
        PropagateError("set_message", msg);
        // delete the temporary mesasge
        delete msg;
        _synch.InstanceUnlock("CANMessageManager_access");
        return false;
    }
    // no error occurred, add the message to the architecture
    _messageById[properties->get_id()] = msg;
    _messageByName[properties->get_name()] = msg;
    _synch.InstanceUnlock("CANMessageManager_access");
    return true;
    }

/*
 ____                               __  __
|  _ \ ___ _ __ ___   _____   _____|  \/  | ___ ___ ___  __ _  __ _  ___
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
|  _ <  __/ | | | | | (_) \ V /  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
|_| \_\___|_| |_| |_|\___/ \_/ \___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                              |___/
*/
bool CANMessageManager::RemoveMessage(CANBasicTypes::type_uinteger message_id) {
    _synch.InstanceLock("CANMessageManager_access");
    // check the existence of the message
    _messageByIdType::iterator it = _messageById.find(message_id);
    if (it == _messageById.end())  {
        // message is not present
        std::stringstream s;
        s << "failed to remove the message with id = 0x" << hex << message_id << " [not present]";
        PushError("RemoveMessage", s.str());
        _synch.InstanceUnlock("CANMessageManager_access");
        return false;
        }
    CANMessage *msg = _messageById[message_id];
    // the message is removed by the architecture
    _messageByName.erase(msg->get_properties()->get_name());
    _messageById.erase(message_id);
    delete msg;
    _synch.InstanceUnlock("CANMessageManager_access");
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
bool CANMessageManager::RemoveMessage(CANBasicTypes::type_string message_name) {
    _synch.InstanceLock("CANMessageManager_access");
    // check the existence of the message
    _messageByNameType::iterator it = _messageByName.find(message_name);
    if (it == _messageByName.end())  {
        // message is not present
        std::stringstream s;
        s << "failed to remove the message <" << message_name << "> [not present]";
        PushError("RemoveMessage", s.str());
        _synch.InstanceUnlock("CANMessageManager_access");
        return false;
        }
    CANMessage *msg = _messageByName[message_name];
    // the message is removed by the architecture
    _messageByName.erase(message_name);
    _messageById.erase(msg->get_properties()->get_id());
    delete msg;
    _synch.InstanceUnlock("CANMessageManager_access");
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
std::string CANMessageManager::GetDescription() {
    _synch.InstanceLock("CANMessageManager_access");
#define EOL std::endl
    std::stringstream s;

    s << "CAN Message Manager" << EOL;
    _messageByIdType::iterator it = _messageById.begin();
    for (; it != _messageById.end(); it++) {
        s << " - - - - - - " << EOL << it->second->GetDescription() << EOL;
        }
    _synch.InstanceUnlock("CANMessageManager_access");
    return s.str();
    };

