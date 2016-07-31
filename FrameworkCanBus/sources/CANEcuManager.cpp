#include "FrameworkCanBus/CANEcuManager.h"
#include "FrameworkCore/CSynch.h"
#include <sstream>

using namespace FrameworkCore;
using namespace FrameworkCanBus;

/*
  ____    _    _   _ _____           __  __
 / ___|  / \  | \ | | ____|___ _   _|  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| |  _| / __| | | | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  | |__| (__| |_| | |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_|_____\___|\__,_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                              |___/
*/
CANEcuManager::CANEcuManager() {
    SetErrorClassPrefix("CANEcuManager");
    _ecuMap.clear();
    };

/*
 /\/|___    _    _   _ _____           __  __
|/\/ ___|  / \  | \ | | ____|___ _   _|  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |     / _ \ |  \| |  _| / __| | | | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___ / ___ \| |\  | |__| (__| |_| | |  | | (_| | | | | (_| | (_| |  __/ |
   \____/_/   \_\_| \_|_____\___|\__,_|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                |___/
*/
CANEcuManager::~CANEcuManager() {
    // destroy ECU objects
    CSynch::IntraProcessLock("CANEcuManager");
    _ecuMapType::iterator it = _ecuMap.begin();
    while (it != _ecuMap.end()) {
        delete it->second;
        it++;
        };
    _ecuMap.clear();
    CSynch::IntraProcessUnlock("CANEcuManager");
    };

/*
            _                          _ _     _
  __ _  ___| |_     ___  ___ _   _    | (_)___| |_
 / _` |/ _ \ __|   / _ \/ __| | | |   | | / __| __|
| (_| |  __/ |_   |  __/ (__| |_| |   | | \__ \ |_
 \__, |\___|\__|___\___|\___|\__,_|___|_|_|___/\__|
 |___/        |_____|            |_____|
*/
CANEcuTypes::ecu_list_type CANEcuManager::get_ecu_list() {
    // create the ECU list
    CANEcuTypes::ecu_list_type list;
    CSynch::IntraProcessLock("CANEcuManager");
    _ecuMapType::iterator it = _ecuMap.begin();
    while (it != _ecuMap.end()) {
        list.insert(it->first);
        it++;
        }
    CSynch::IntraProcessUnlock("CANEcuManager");
    return list;
    };

/*
            _
  __ _  ___| |_     ___  ___ _   _
 / _` |/ _ \ __|   / _ \/ __| | | |
| (_| |  __/ |_   |  __/ (__| |_| |
 \__, |\___|\__|___\___|\___|\__,_|
 |___/        |_____|
*/
CANEcu *CANEcuManager::get_ecu(CANBasicTypes::type_string _ecuName) {
    CANEcu *retval = NULL;
    CSynch::IntraProcessLock("CANEcuManager");
    // ECU must be present
    _ecuMapType::iterator it = _ecuMap.find(_ecuName);
    if (it == _ecuMap.end()) {
        // ECU is not present
        std::stringstream s;
        s << "getting non-existent -" << _ecuName << "- ECU";
        PushError("get_ecu", s.str());
        }
    else
        retval = _ecuMap[_ecuName];
    CSynch::IntraProcessUnlock("CANEcuManager");
    return retval;
    };

/*
    _       _     _ _____ ____ _   _
   / \   __| | __| | ____/ ___| | | |
  / _ \ / _` |/ _` |  _|| |   | | | |
 / ___ \ (_| | (_| | |__| |___| |_| |
/_/   \_\__,_|\__,_|_____\____|\___/
*/
bool CANEcuManager::AddECU(CANBasicTypes::type_string _ecuName) {
    CSynch::IntraProcessLock("CANEcuManager");
    // ECU must not be already present
    _ecuMapType::iterator it = _ecuMap.find(_ecuName);
    if (it != _ecuMap.end()) {
        // ECU is already present
        std::stringstream s;
        s << "adding ECU -" << _ecuName << "- which is already present";
        PushError("AddECU", s.str());
        CSynch::IntraProcessUnlock("CANEcuManager");
        return false;
        }
    else
        {
        CANEcu *ECU = new CANEcu(_ecuName);
        _ecuMap[_ecuName] = ECU;
        CSynch::IntraProcessUnlock("CANEcuManager");
        return true;
        }
    };

/*
    _       _     _ _____ ____ _   _ _     _     _
   / \   __| | __| | ____/ ___| | | | |   (_)___| |_
  / _ \ / _` |/ _` |  _|| |   | | | | |   | / __| __|
 / ___ \ (_| | (_| | |__| |___| |_| | |___| \__ \ |_
/_/   \_\__,_|\__,_|_____\____|\___/|_____|_|___/\__|
*/
bool CANEcuManager::AddECUList(CANEcuTypes::ecu_list_type ecu_name_list) {
    //CSynch::IntraProcessLock("CANEcuManager");
    CANEcuTypes::ecu_list_type::iterator it = ecu_name_list.begin();
    while (it != ecu_name_list.end()) {
        AddECU(*it);
        if (HasError()) {
            PropagateError("AddECUList");
            CSynch::IntraProcessUnlock("CANEcuManager");
            return false;
            };
        it++;
        };
    //CSynch::IntraProcessUnlock("CANEcuManager");
    return true;
    };

/*
 _____      _     _   _____ ____ _   _
| ____|_  _(_)___| |_| ____/ ___| | | |
|  _| \ \/ / / __| __|  _|| |   | | | |
| |___ >  <| \__ \ |_| |__| |___| |_| |
|_____/_/\_\_|___/\__|_____\____|\___/
*/
bool CANEcuManager::ExistECU(CANBasicTypes::type_string _ecuName) {
    // check for ECU to be present
    CSynch::IntraProcessLock("CANEcuManager");
    _ecuMapType::iterator it = _ecuMap.find(_ecuName);
    if (it == _ecuMap.end()) {
        CSynch::IntraProcessUnlock("CANEcuManager");
        return false;
        } else {
            CSynch::IntraProcessUnlock("CANEcuManager");
            return true;
        }
    };

/*
 ____                               _____ ____ _   _
|  _ \ ___ _ __ ___   _____   _____| ____/ ___| | | |
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \  _|| |   | | | |
|  _ <  __/ | | | | | (_) \ V /  __/ |__| |___| |_| |
|_| \_\___|_| |_| |_|\___/ \_/ \___|_____\____|\___/
*/
bool CANEcuManager::RemoveECU(CANBasicTypes::type_string _ecuName) {
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
std::string CANEcuManager::GetDescription() {
#define EOL std::endl
    CSynch::IntraProcessLock("CANEcuManager");
    std::stringstream s;
    s << "ECU Manager" << EOL;
    _ecuMapType::iterator it = _ecuMap.begin();
    while (it != _ecuMap.end()) {
        CANEcu *ECU = it->second;
        s << ECU->GetDescription() << EOL;
        it++;
        }
    CSynch::IntraProcessUnlock("CANEcuManager");
    return s.str();
    };