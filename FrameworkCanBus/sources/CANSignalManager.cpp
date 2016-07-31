#include "FrameworkCanBus/CANSignalManager.h"
#include <sstream>

using namespace FrameworkCanBus;

/*
  ____    _    _   _ ____  _                   _ __  __
 / ___|  / \  | \ | / ___|(_) __ _ _ __   __ _| |  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| \___ \| |/ _` | '_ \ / _` | | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  |___) | | (_| | | | | (_| | | |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_|____/|_|\__, |_| |_|\__,_|_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                             |___/                                        |___/
*/
CANSignalManager::CANSignalManager() {
    SetErrorClassPrefix("CANSignalManager");
    _signalByName.clear();
    //_synch = CSynch();
    };

/*
 /\/|___    _    _   _ ____  _                   _ __  __
|/\/ ___|  / \  | \ | / ___|(_) __ _ _ __   __ _| |  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |     / _ \ |  \| \___ \| |/ _` | '_ \ / _` | | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___ / ___ \| |\  |___) | | (_| | | | | (_| | | |  | | (_| | | | | (_| | (_| |  __/ |
   \____/_/   \_\_| \_|____/|_|\__, |_| |_|\__,_|_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                               |___/                                        |___/
*/
CANSignalManager::~CANSignalManager() {
    _synch.InstanceLock("CANSignalManager_access");
    _signalByNameType::iterator it = _signalByName.begin();
    while (it != _signalByName.end()) {
        // delete existing signals
        delete it->second;
        it++;
        };
    _signalByName.clear();
    _synch.InstanceUnlock("CANSignalManager_access");
    };

/*
 ____  _                   _ _____      _     _
/ ___|(_) __ _ _ __   __ _| | ____|_  _(_)___| |_
\___ \| |/ _` | '_ \ / _` | |  _| \ \/ / / __| __|
 ___) | | (_| | | | | (_| | | |___ >  <| \__ \ |_
|____/|_|\__, |_| |_|\__,_|_|_____/_/\_\_|___/\__|
         |___/
*/
bool CANSignalManager::SignalExist(CANBasicTypes::type_string signal_name) {
    _synch.InstanceLock("CANSignalManager_access");
    if (_signalByName.find(signal_name) != _signalByName.end()){
        _synch.InstanceUnlock("CANSignalManager_access");
        return true;
        } else {
            _synch.InstanceUnlock("CANSignalManager_access");
            return false;
        }
    };

/*
            _           _                   _
  __ _  ___| |_     ___(_) __ _ _ __   __ _| |
 / _` |/ _ \ __|   / __| |/ _` | '_ \ / _` | |
| (_| |  __/ |_    \__ \ | (_| | | | | (_| | |
 \__, |\___|\__|___|___/_|\__, |_| |_|\__,_|_|
 |___/        |_____|     |___/
*/
CANSignal *CANSignalManager::get_signal(CANBasicTypes::type_string signal_name) {
    _synch.InstanceLock("CANSignalManager_access");
    _signalByNameType::iterator it = _signalByName.find(signal_name);
    if (it == _signalByName.end()) {
        // no signals wxist with the given name
        std::stringstream s;
        s << "no signals with name <" << signal_name << "> found.";
        PushError("get_signal", s.str());
        _synch.InstanceUnlock("CANSignalManager_access");
        return NULL;
        }
    else {
        _synch.InstanceUnlock("CANSignalManager_access");
        return it->second;
        }
    };

/*
    _       _     _ ____  _                   _
   / \   __| | __| / ___|(_) __ _ _ __   __ _| |
  / _ \ / _` |/ _` \___ \| |/ _` | '_ \ / _` | |
 / ___ \ (_| | (_| |___) | | (_| | | | | (_| | |
/_/   \_\__,_|\__,_|____/|_|\__, |_| |_|\__,_|_|
                            |___/
*/
bool CANSignalManager::AddSignal(CANSignalProperties *signal_properties) {
    _synch.InstanceLock("CANSignalManager_access");
    // verify that a signal with the same name does not exist yet
    if (SignalExist(signal_properties->get_name())) {
        // signal already exist, not an error. exit
        std::stringstream s;
        s << "signal <" << signal_properties->get_name() << "> already exists" << std::endl;
        PushError("AddSignal", s.str());
        _synch.InstanceUnlock("CANSignalManager_access");
        return true;
        }
    // signal does not exist yet
    CANSignal *s = new CANSignal();
    s->get_properties()->set_properties(signal_properties);
    if (s->HasError()) {
        // error in property values
        PropagateError("AddSignal", s);
        delete s;
        _synch.InstanceUnlock("CANSignalManager_access");
        return false;
        }
    // ok, no error occurred, add the signal to the architecture
    _signalByName[s->get_properties()->get_name()] = s;
    _synch.InstanceUnlock("CANSignalManager_access");
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
bool CANSignalManager::RemoveMessage(CANBasicTypes::type_string signal_name) {
    _synch.InstanceLock("CANSignalManager_access");
    // verify that a signal with the same nale does not exist yet
    if (!SignalExist(signal_name)) {
        // signal does not exist
        std::stringstream s;
        s << "signal -" << signal_name << "- not found" << std::endl;
        PushError("RemoveMessage", s.str());
        _synch.InstanceUnlock("CANSignalManager_access");
        return false;
        }
    delete _signalByName[signal_name];
    _signalByName.erase(signal_name);
    _synch.InstanceUnlock("CANSignalManager_access");
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
std::string CANSignalManager::GetDescription() {
    _synch.InstanceLock("CANSignalManager_access");
#define EOL std::endl
    std::stringstream s;
    s << "Signal Manager" << EOL;
    _signalByNameType::iterator it = _signalByName.begin();
    while (it != _signalByName.end()) {
        s << it->second->GetDescription() << EOL;
        it++;
        };
    _synch.InstanceUnlock("CANSignalManager_access");
    return s.str();
    };

