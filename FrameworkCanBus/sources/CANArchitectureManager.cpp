#include "FrameworkCanBus/CANArchitectureManager.h"
#include "FrameworkCanBus/CDbcWrapper.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace std;

// ----------------------------------------------------------------------------------
// ------------------------------- constructors/destructors
// ----------------------------------------------------------------------------------
/*
  ____    _    _   _    _             _     _ _            _                  __  __
 / ___|  / \  | \ | |  / \   _ __ ___| |__ (_) |_ ___  ___| |_ _   _ _ __ ___|  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| | / _ \ | '__/ __| '_ \| | __/ _ \/ __| __| | | | '__/ _ \ |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  |/ ___ \| | | (__| | | | | ||  __/ (__| |_| |_| | | |  __/ |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_/_/   \_\_|  \___|_| |_|_|\__\___|\___|\__|\__,_|_|  \___|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                                       |___/
*/
CANArchitectureManager::CANArchitectureManager() :    _hardwareManager(NULL)
                                                    , _messageManager(NULL)
                                                    , _signalManager(NULL)
                                                    , _ecuManager(NULL)
                                                    , _transmitterManager(NULL)
                                                    , _receiverManager(NULL)
                                                    , _callbackManager(NULL) {
    
    SetErrorClassPrefix("CANArchitectureManager");
    // hardware manager initialization
    _hardwareManager = new CANHardwareManager();
    // signal manager initialization
    _signalManager  = new CANSignalManager();
    // message manager initialization
    _messageManager = new CANMessageManager();
    // ECU manager initialization
    _ecuManager     = new CANEcuManager();
    // message transmitter manager
    _transmitterManager  = new CANTransmitterManager();
    // message receiver manager
    _receiverManager = new CANReceiverManager();
    // callback manager
    _callbackManager = new CANCallbackManager();

    // assegna i gestori
    _transmitterManager->SetManagers(_messageManager, _signalManager, _hardwareManager, _callbackManager);
    _receiverManager->SetManagers(_ecuManager, _messageManager, _signalManager, _hardwareManager, _callbackManager);
    // sets the virtual device as default hardware
    _defaultDevice.clear();
    SetDefaultHardwareDevice(_hardwareManager->GetVirtualDevice()->getName());

    }

/*
 /\/|___    _    _   _    _             _     _ _            _                  __  __
|/\/ ___|  / \  | \ | |  / \   _ __ ___| |__ (_) |_ ___  ___| |_ _   _ _ __ ___|  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |     / _ \ |  \| | / _ \ | '__/ __| '_ \| | __/ _ \/ __| __| | | | '__/ _ \ |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___ / ___ \| |\  |/ ___ \| | | (__| | | | | ||  __/ (__| |_| |_| | | |  __/ |  | | (_| | | | | (_| | (_| |  __/ |
   \____/_/   \_\_| \_/_/   \_\_|  \___|_| |_|_|\__\___|\___|\__|\__,_|_|  \___|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                                         |___/
*/
CANArchitectureManager::~CANArchitectureManager() {
    StopAllECU();
    delete _transmitterManager; // stop transmitting messages
    delete _receiverManager;    // stop receiving messages
    delete _signalManager;      // deallocate all signals
    delete _messageManager;     // deallocate all messages
    delete _ecuManager;         // deallocate all ecus
    delete _hardwareManager;    // release all hardware devices
    }



/*
  ____ _               _    ____  _                   _  ____      _ _ _     _
 / ___| |__   ___  ___| | _/ ___|(_) __ _ _ __   __ _| |/ ___|___ | | (_)___(_) ___  _ __  ___
| |   | '_ \ / _ \/ __| |/ |___ \| |/ _` | '_ \ / _` | | |   / _ \| | | / __| |/ _ \| '_ \/ __|
| |___| | | |  __/ (__|   < ___) | | (_| | | | | (_| | | |__| (_) | | | \__ \ | (_) | | | \__ \
 \____|_| |_|\___|\___|_|\_\____/|_|\__, |_| |_|\__,_|_|\____\___/|_|_|_|___/_|\___/|_| |_|___/
                                    |___/
*/
CANBasicTypes::type_string CANArchitectureManager::CheckSignalCollisions(CANBasicTypes::type_uinteger message_id) {
    
    // check message existence
    if (!_messageManager->MessageExist(message_id)) {
        std::stringstream s;
        s << "checking collisions in a non-existent message with id 0x" << hex << message_id;
        PushError("check_collisions", s.str());
        std::string("");
    }
    
    // get the mesasge
    CANMessage *msg = _messageManager->get_message(message_id);

    // if one bit is going to be assigned to more than one signal then collision occurred and the new
    // message is removed from the message.
    CANBasicTypes::type_string message_bits[8][8];
    // bit are initialized to be free
    for (int i=0;i<8;i++)
        for(int j=0;j<8;j++)
            message_bits[i][j] = "";
    
    // msg structure: message_bits[a_indx][b_indx] : a_indx id byte index, b_indx is bit index
    // for every signal, the name is associated to the starting bit, then:
    // - b_indx is always increasing 
    // - a_indx increases in INTEL byte order, decreases in MOTOROLA byte order
    //
    //             (INTEL)   ---- >           (INTEL)   ---- >      (INTEL)   ---- >  
    // |          a0            |           a1            |            a2           |  ....   
    // |b0 b1 b2 b3 b4 b5 b6 b7 | b0 b1 b2 b3 b4 b5 b6 b7 | b0 b1 b2 b3 b4 b5 b6 b7 |....
    // |-> -> -> -> -> -> -> -> | -> -> -> -> -> -> -> -> | -> -> -> -> -> -> -> -> | 
    // | 0  0  1  0  1  1  0  1 |  1  0  1  0 1  0  0  1  |  1  1  1  0  0  1  0  0 |
    //                       <---- (MOTOROLA)          <---- (MOTOROLA)          <---- (MOTOROLA)
    //

    // cycle through the message's signals and mark each bit with the signal's name
    CANMessageTypes::signal_set_type signal_name_set = msg->get_signals();
    CANMessageTypes::signal_set_type::iterator it = signal_name_set.begin();
    for (; it != signal_name_set.end(); it++ ) {
        // signal name
        CANBasicTypes::type_string signal_name = *it;
        // get the starting bit
        CANSignal *signal = _signalManager->get_signal(signal_name);
        CANBasicTypes::type_uinteger starting_bit = msg->get_signal_starting_bit(signal_name);
        if (msg->HasError()) {
            //something failed, manage error
            PropagateError("[AddSignalToMessage]", msg);
            return "";
            }

        unsigned int a_indx = int(starting_bit) / 8;
        unsigned int b_indx = starting_bit - a_indx*8;
        
        for (int i=0; i < signal->get_properties()->get_length(); i++) {
            if (a_indx>=8) {
                // a_indx can't be out of range
                std::stringstream s;
                s << "a_indx out of range while adding <" << signal_name << ">";
                PushError("AddSignalToMessage", s.str());
                return std::string("");
                }
            if (message_bits[a_indx][b_indx] != CANBasicTypes::type_string("")) {
                // signal collided with existing one
                std::stringstream s;
                s << "Collision between signals <" << signal_name << "> and <" << message_bits[a_indx][b_indx] << ">";
                return s.str();
                }
            // no collision, assign the signal's name
            message_bits[a_indx][b_indx] = signal_name;
            // go to the next bit
            if (++b_indx >= 8) {
                 b_indx = 0;
                 if (signal->get_properties()->get_byte_order() == CANSignalTypes::INTEL_BYTEORDER)
                     a_indx++;
                 else
                     a_indx--;
                }
            }
        }
    // no collision occurred
    return std::string("");
    }

// ----------------------------------------------------------------------------------
// ------------------------------- loading architecture
// ----------------------------------------------------------------------------------
/*
 _                    _ ____  ____   ____
| |    ___   __ _  __| |  _ \| __ ) / ___|
| |   / _ \ / _` |/ _` | | | |  _ \| |
| |__| (_) | (_| | (_| | |_| | |_) | |___
|_____\___/ \__,_|\__,_|____/|____/ \____|
*/
bool CANArchitectureManager::LoadDBC(std::string dbc_name) {
    CDbcWrapper dbc;
    dbc.ParseDBCFile(dbc_name, this, _ecuManager, _messageManager, _signalManager);
    if (dbc.HasError()) {
        PropagateError("LoadDBC", &dbc);
        return false;
        }
    // file parsed with no errors, architecture is ready
    return true;
    }

// ----------------------------------------------------------------------------------
// ------------------------------- managing architecture
// ----------------------------------------------------------------------------------
/*
    _       _     _ ____  _                   _ _____     __  __
   / \   __| | __| / ___|(_) __ _ _ __   __ _| |_   _|__ |  \/  | ___ ___ ___  __ _  __ _  ___
  / _ \ / _` |/ _` \___ \| |/ _` | '_ \ / _` | | | |/ _ \| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 / ___ \ (_| | (_| |___) | | (_| | | | | (_| | | | | (_) | |  | |  __|__ \__ \ (_| | (_| |  __/
/_/   \_\__,_|\__,_|____/|_|\__, |_| |_|\__,_|_| |_|\___/|_|  |_|\___|___/___/\__,_|\__, |\___|
                            |___/                                                   |___/
*/
bool CANArchitectureManager::AddSignalToMessage(    CANBasicTypes::type_string      signal_name, 
                                                    CANBasicTypes::type_uinteger    message_id,
                                                    CANBasicTypes::type_uinteger    starting_bit) {

    // check message existence
     if (!_messageManager->MessageExist(message_id)) {
         std::stringstream s;
         s << "trying to add signal <" << signal_name << "> to a non-existent message 0x" << hex << message_id;
         PushError("AddSignalToMessage", s.str());
         return false;
         }

    // check signal existence
    if (!_signalManager->SignalExist(signal_name)) {
         std::stringstream s;
         s << "adding undefined signal <" << signal_name << "> to message 0x" << hex << message_id;
         PushError("AddSignalToMessage", s.str());
         return false;
        }

    /* Signal and message are defined in the architecture. A signal is allowed to be packed 
       into the message only if it does not collide with the already packed signals */
    
    // first of all add the signal to the message
    CANMessage *msg = _messageManager->get_message(message_id);
    if (_messageManager->HasError()) {
        PropagateError("AddSignalToMessage", _messageManager);
        return false;
        }
    if (!msg->AddSignal(signal_name, starting_bit)) {
        // some error occurred adding the signal
        PropagateError("AddSignalToMessage", msg);
        return false;
        }

    // now check collisions. Collision is computed by associating the signal name to each bit it uses;
    std::string collision_description = CheckSignalCollisions(message_id);
    if (HasError()) {
        // collision occurred, return the error
        PushError("AddSignalToMessage", collision_description);
        msg->RemoveSignal(signal_name);
        return false;
        }
    
    // no collision, signal is succesfully added to the message
    return true;
    }


// ----------------------------------------------------------------------------------
// ------------------------------- hardware management
// ----------------------------------------------------------------------------------
/*
 ____       _   ____        __             _ _   _   _               _                        ____             _
/ ___|  ___| |_|  _ \  ___ / _| __ _ _   _| | |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|  _ \  _____   _(_) ___ ___
\___ \ / _ \ __| | | |/ _ \ |_ / _` | | | | | __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ | | |/ _ \ \ / / |/ __/ _ \
 ___) |  __/ |_| |_| |  __/  _| (_| | |_| | | |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |_| |  __/\ V /| | (_|  __/
|____/ \___|\__|____/ \___|_|  \__,_|\__,_|_|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|____/ \___| \_/ |_|\___\___|
*/
bool CANArchitectureManager::SetDefaultHardwareDevice(CANBasicTypes::type_string name) {
    // look for existing device
    CANHardwareInterface *HIF = _hardwareManager->GetHardwareInterface(name);
    if (!HIF)
        // device not found
        return false;
    else {
        // device exist, assign as default
        _defaultDevice = HIF->getName();
        return true;
        }
    }

/*
 ____       _   ____        __             _ _   _   _               _                        ____             _
/ ___|  ___| |_|  _ \  ___ / _| __ _ _   _| | |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|  _ \  _____   _(_) ___ ___
\___ \ / _ \ __| | | |/ _ \ |_ / _` | | | | | __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ | | |/ _ \ \ / / |/ __/ _ \
 ___) |  __/ |_| |_| |  __/  _| (_| | |_| | | |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |_| |  __/\ V /| | (_|  __/
|____/ \___|\__|____/ \___|_|  \__,_|\__,_|_|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|____/ \___| \_/ |_|\___\___|
*/
bool CANArchitectureManager::SetDefaultHardwareDevice(unsigned int deviceID) {
    // look for existing device
    CANHardwareInterface *HIF = _hardwareManager->GetHardwareInterface(deviceID);
    if (!HIF)
        // device not found
        return false;
    else {
        // device exist, assign as default
        _defaultDevice = HIF->getName();
        return true;
        }
    }

/*
 ____  _             _   ____               _       _             _____                    ____        __             _ _   _   _               _                        ____             _
/ ___|| |_ __ _ _ __| |_|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___| __ ___  _ __ ___ |  _ \  ___ / _| __ _ _   _| | |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|  _ \  _____   _(_) ___ ___
\___ \| __/ _` | '__| __| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_ | '__/ _ \| '_ ` _ \| | | |/ _ \ |_ / _` | | | | | __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ | | |/ _ \ \ / / |/ __/ _ \
 ___) | || (_| | |  | |_|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _|| | | (_) | | | | | | |_| |  __/  _| (_| | |_| | | |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |_| |  __/\ V /| | (_|  __/
|____/ \__\__,_|_|   \__|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|  |_|  \___/|_| |_| |_|____/ \___|_|  \__,_|\__,_|_|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|____/ \___| \_/ |_|\___\___|
                                                            |___/
*/
bool CANArchitectureManager::StartReceivingFromDefaultHardwareDevice() {
    return get_receiver_manager()->StartReceiving(GetDefaultHardwareInterface()->getName());
    }

/*
 ____  _              ____               _       _             _____                    ____        __             _ _   _   _               _                        ____             _
/ ___|| |_ ___  _ __ |  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___| __ ___  _ __ ___ |  _ \  ___ / _| __ _ _   _| | |_| | | | __ _ _ __ __| |_      ____ _ _ __ ___|  _ \  _____   _(_) ___ ___
\___ \| __/ _ \| '_ \| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_ | '__/ _ \| '_ ` _ \| | | |/ _ \ |_ / _` | | | | | __| |_| |/ _` | '__/ _` \ \ /\ / / _` | '__/ _ \ | | |/ _ \ \ / / |/ __/ _ \
 ___) | || (_) | |_) |  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _|| | | (_) | | | | | | |_| |  __/  _| (_| | |_| | | |_|  _  | (_| | | | (_| |\ V  V / (_| | | |  __/ |_| |  __/\ V /| | (_|  __/
|____/ \__\___/| .__/|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|  |_|  \___/|_| |_| |_|____/ \___|_|  \__,_|\__,_|_|\__|_| |_|\__,_|_|  \__,_| \_/\_/ \__,_|_|  \___|____/ \___| \_/ |_|\___\___|
               |_|                                       |___/
*/
bool CANArchitectureManager::StopReceivingFromDefaultHardwareDevice() {
    return get_receiver_manager()->StopReceiving(GetDefaultHardwareInterface()->getName());
    }
// ----------------------------------------------------------------------------------
// ------------------------------- start/stop ECUs
// ----------------------------------------------------------------------------------
/*
 ____  _             _   _____ ____ _   _
/ ___|| |_ __ _ _ __| |_| ____/ ___| | | |
\___ \| __/ _` | '__| __|  _|| |   | | | |
 ___) | || (_| | |  | |_| |__| |___| |_| |
|____/ \__\__,_|_|   \__|_____\____|\___/
*/
void CANArchitectureManager::StartECU(CANBasicTypes::type_string _ecuName) {
    // retrive the ecu descriptor
    CANEcu *ecu = _ecuManager->get_ecu(_ecuName);
    if (_ecuManager->HasError()) {
        PropagateError("[StartECU]", _ecuManager);
        return;
        }
    // ecu retrived
    CANEcuTypes::msg_id_set_type msg_list = ecu->get_transmitted_msg_id_list();
    CANEcuTypes::msg_id_set_type::iterator it = msg_list.begin();
    while (it != msg_list.end()) {
        // starts only periodic messages
        if (_messageManager->get_message(*it)->get_properties()->get_cycle_time() > 0)
            _transmitterManager->StartMessage(_messageManager->get_message(*it)->get_properties()->get_id(), _defaultDevice);
        it++;
        }
    }

/*
 ____  _             _   _____ ____ _   _             ____             _
/ ___|| |_ __ _ _ __| |_| ____/ ___| | | | ___  _ __ |  _ \  _____   _(_) ___ ___
\___ \| __/ _` | '__| __|  _|| |   | | | |/ _ \| '_ \| | | |/ _ \ \ / / |/ __/ _ \
 ___) | || (_| | |  | |_| |__| |___| |_| | (_) | | | | |_| |  __/\ V /| | (_|  __/
|____/ \__\__,_|_|   \__|_____\____|\___/ \___/|_| |_|____/ \___| \_/ |_|\___\___|
*/
void CANArchitectureManager::StartECUOnDevice( CANBasicTypes::type_string ECU_name, 
                                               CANBasicTypes::type_string HW_name) {
    // store actual default device
    CANBasicTypes::type_string active_device = _defaultDevice;
    // set temprairly the new default device
    _defaultDevice = HW_name;
    // start the ecu on this device
    StartECU(ECU_name);
    // restore the default device
    _defaultDevice = active_device;
    }

/*
 ____  _              _____ ____ _   _
/ ___|| |_ ___  _ __ | ____/ ___| | | |
\___ \| __/ _ \| '_ \|  _|| |   | | | |
 ___) | || (_) | |_) | |__| |___| |_| |
|____/ \__\___/| .__/|_____\____|\___/
               |_|
*/
void CANArchitectureManager::StopECU(CANBasicTypes::type_string _ecuName) {
    // retrive the ecu descriptor
    CANEcu *ecu = _ecuManager->get_ecu(_ecuName);
    if (_ecuManager->HasError()) {
        PropagateError("[StartECU]", _ecuManager);
        CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
        return;
        }
    // ecu retrived
    CANEcuTypes::msg_id_set_type msg_list = ecu->get_transmitted_msg_id_list();
    CANEcuTypes::msg_id_set_type::iterator it = msg_list.begin();
    while (it != msg_list.end()) {
        // stop periodic messages
        _transmitterManager->StopMessage(_messageManager->get_message(*it)->get_properties()->get_id());
        it++;
        }
    }

/*
 ____  _             _      _    _ _ _____ ____ _   _
/ ___|| |_ __ _ _ __| |_   / \  | | | ____/ ___| | | |
\___ \| __/ _` | '__| __| / _ \ | | |  _|| |   | | | |
 ___) | || (_| | |  | |_ / ___ \| | | |__| |___| |_| |
|____/ \__\__,_|_|   \__/_/   \_\_|_|_____\____|\___/
*/
void CANArchitectureManager::StartAllECU() {
    // cycle through all the ecu and activate their trantmitted message
    // nb: periodic messages are persistent, single-shot messages are sent just once
    CANEcuTypes::ecu_list_type ecu_list = _ecuManager->get_ecu_list();
    CANEcuTypes::ecu_list_type::iterator it = ecu_list.begin();
    while (it != ecu_list.end()) {
        StartECU(*it);
        it++;
        }
    }

/*
 ____  _             _      _    _ _ _____ ____ _   _             ____             _
/ ___|| |_ __ _ _ __| |_   / \  | | | ____/ ___| | | | ___  _ __ |  _ \  _____   _(_) ___ ___
\___ \| __/ _` | '__| __| / _ \ | | |  _|| |   | | | |/ _ \| '_ \| | | |/ _ \ \ / / |/ __/ _ \
 ___) | || (_| | |  | |_ / ___ \| | | |__| |___| |_| | (_) | | | | |_| |  __/\ V /| | (_|  __/
|____/ \__\__,_|_|   \__/_/   \_\_|_|_____\____|\___/ \___/|_| |_|____/ \___| \_/ |_|\___\___|
*/
void CANArchitectureManager::StartAllECUOnDevice(CANBasicTypes::type_string HW_name) {
    // store actual default device
    CANBasicTypes::type_string active_device = _defaultDevice;
    // set temprairly the new default device
    _defaultDevice = HW_name;
    // start the ecu on this device
    StartAllECU();
    // restore the default device
    _defaultDevice = active_device;
    }

/*
 ____  _                _    _ _ _____ ____ _   _
/ ___|| |_ ___  _ __   / \  | | | ____/ ___| | | |
\___ \| __/ _ \| '_ \ / _ \ | | |  _|| |   | | | |
 ___) | || (_) | |_) / ___ \| | | |__| |___| |_| |
|____/ \__\___/| .__/_/   \_\_|_|_____\____|\___/
               |_|
*/
void CANArchitectureManager::StopAllECU() {
    // cycle through all the ecu and deactivate their transmitted message
    CANEcuTypes::ecu_list_type ecu_list = _ecuManager->get_ecu_list();
    CANEcuTypes::ecu_list_type::iterator it = ecu_list.begin();
    while (it != ecu_list.end()) {
        StopECU(*it);
        it++;
        }
    }

// ----------------------------------------------------------------------------------
// ------------------------------- start/stop signals
// ----------------------------------------------------------------------------------
/*
 ____  _             _   ____  _                   _
/ ___|| |_ __ _ _ __| |_/ ___|(_) __ _ _ __   __ _| |
\___ \| __/ _` | '__| __\___ \| |/ _` | '_ \ / _` | |
 ___) | || (_| | |  | |_ ___) | | (_| | | | | (_| | |
|____/ \__\__,_|_|   \__|____/|_|\__, |_| |_|\__,_|_|
                                 |___/
*/
bool CANArchitectureManager::StartSignal(CANBasicTypes::type_string signal_name) {
    // retrive the signal
    CANSignal *signal = _signalManager->get_signal(signal_name);
    if (!signal) {
        // signal is not defined, trace the event
        std::stringstream s;
        s << "signal -" << signal_name << "- not found" << std::endl;
        PushError("StartSignal", s.str());
        return false;
        }
    // start all the messages that are packing this signal
    CANSignalTypes::message_set_type messages = signal->get_message_set();
    CANSignalTypes::message_set_type::iterator it = messages.begin();
    while (it != messages.end()) {
        // start the message (if periodic)
        if (_messageManager->get_message(*it)->get_properties()->get_cycle_time() > 0) {
            StartMessage(*it);
            if (HasError()) {
             // some error occurred, stack error and return
                PropagateError("StartSignal");
                return false;
                }
            }
        it++;
        }
    return true;
    }

/*
 ____  _             ____  _                   _
/ ___|| |_ ___  _ __/ ___|(_) __ _ _ __   __ _| |
\___ \| __/ _ \| '_ \___ \| |/ _` | '_ \ / _` | |
 ___) | || (_) | |_) |__) | | (_| | | | | (_| | |
|____/ \__\___/| .__/____/|_|\__, |_| |_|\__,_|_|
               |_|           |___/
*/
bool CANArchitectureManager::StopSignal(CANBasicTypes::type_string signal_name) {
    // retrive the signal
    CANSignal *signal = _signalManager->get_signal(signal_name);
    if (!signal) {
        // signal is not defined, trace the event
        std::stringstream s;
        s << "signal <" << signal_name << "> not found" << std::endl;
        PushError("StopSignal", s.str());
        return false;
        }
    // start all the messages that are packing this signal
    CANSignalTypes::message_set_type messages = signal->get_message_set();
    CANSignalTypes::message_set_type::iterator it = messages.begin();
    while (it != messages.end()) {
        // stop the message
        StopMessage(*it);
        if (HasError()) {
            // some error occurred, stack error and return
            PropagateError("[StartSignal]");
            return false;
            }
        it++;
        }
    return true;
    }

/*
 _____             _     _      ____  _                   _
| ____|_ __   __ _| |__ | | ___/ ___|(_) __ _ _ __   __ _| |
|  _| | '_ \ / _` | '_ \| |/ _ \___ \| |/ _` | '_ \ / _` | |
| |___| | | | (_| | |_) | |  __/___) | | (_| | | | | (_| | |
|_____|_| |_|\__,_|_.__/|_|\___|____/|_|\__, |_| |_|\__,_|_|
                                        |___/
*/
bool CANArchitectureManager::EnableSignal(CANBasicTypes::type_string signal_name) {
    // retrive the signal
    CANSignal *signal = _signalManager->get_signal(signal_name);
    if (!signal) {
        // signal is not defined, trace the event
        std::stringstream s;
        s << "signal -" << signal_name << "- not found" << std::endl;
        PushError("EnableSignal", s.str());
        return false;
        }
    signal->Enable();
    return true;
    }

/*
 ____  _           _     _      ____  _                   _
|  _ \(_)___  __ _| |__ | | ___/ ___|(_) __ _ _ __   __ _| |
| | | | / __|/ _` | '_ \| |/ _ \___ \| |/ _` | '_ \ / _` | |
| |_| | \__ \ (_| | |_) | |  __/___) | | (_| | | | | (_| | |
|____/|_|___/\__,_|_.__/|_|\___|____/|_|\__, |_| |_|\__,_|_|
                                        |___/
*/
bool CANArchitectureManager::DisableSignal(CANBasicTypes::type_string signal_name) {
    // retrive the signal
    CANSignal *signal = _signalManager->get_signal(signal_name);
    if (!signal) {
        // signal is not defined, trace the event
        std::stringstream s;
        s << "signal <" << signal_name << "> not found" << std::endl;
        PushError("DisableSignal", s.str());
        return false;
        }
    signal->Disable();
    return true;
    };

// ----------------------------------------------------------------------------------
// ------------------------------- start/stop messages
// ----------------------------------------------------------------------------------
/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANArchitectureManager::StartMessage(CANBasicTypes::type_string msg_name, CANBasicTypes::type_string HW_device) {
    bool retval = true;
    CSynch::IntraProcessLock("CANArchitectureManager__start_stop");
    CANMessage *msg = _messageManager->get_message(msg_name);
    if (_messageManager->HasError()) {
        PropagateError("StartMessage", _messageManager);
        retval = false;
        } else {
        if (msg->get_properties()->get_cycle_time() == 0)
            // one shot message
            _transmitterManager->OneShotMessage(msg->get_properties()->get_id(), HW_device);
        else
            // activate automatic transmission management
            _transmitterManager->StartMessage(msg->get_properties()->get_id(), HW_device);

        if (_transmitterManager->HasError()) {
            PropagateError("StartMessage", _transmitterManager);
            retval = false;
            }
        }
    // message started
    CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
    return retval;
    }

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANArchitectureManager::StartMessage(CANBasicTypes::type_string msg_name) {
    return StartMessage(msg_name, _defaultDevice);
    }

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANArchitectureManager::StartMessage(CANBasicTypes::type_uinteger msg_id, CANBasicTypes::type_string HW_device) {
    CSynch::IntraProcessLock("CANArchitectureManager__start_stop");
    CANMessage *msg = _messageManager->get_message(msg_id);
    if (_messageManager->HasError()) {
        PropagateError("StartMessage", _messageManager);
        CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
        return false;
        }
    if (msg->get_properties()->get_cycle_time() == 0) {
        // one shot message
        _transmitterManager->OneShotMessage(msg->get_properties()->get_id(), HW_device);
        }
    else {
        // activate automatic transmission management
        _transmitterManager->StartMessage(msg->get_properties()->get_id(), HW_device);
        }

    if (_transmitterManager->HasError()) {
        PropagateError("StartMessage", _transmitterManager);
        CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
        return false;
        }
    // message started
    CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
    return true;
    }

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANArchitectureManager::StartMessage(CANBasicTypes::type_uinteger msg_id) {
    return StartMessage(msg_id, _defaultDevice);
    }


/*
 ____  _              __  __
/ ___|| |_ ___  _ __ |  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _ \| '_ \| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_) | |_) | |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\___/| .__/|_|  |_|\___|___/___/\__,_|\__, |\___|
               |_|                              |___/
*/
bool CANArchitectureManager::StopMessage(CANBasicTypes::type_string msg_name) {
    CSynch::IntraProcessLock("CANArchitectureManager__start_stop");
    if (!_transmitterManager->StopMessage(msg_name)) {
        CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
        PropagateError("StopMessage", _transmitterManager);
        return false;
        }
    // message stopped
    CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
    return true;
    }

/*
 ____  _              __  __
/ ___|| |_ ___  _ __ |  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _ \| '_ \| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_) | |_) | |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\___/| .__/|_|  |_|\___|___/___/\__,_|\__, |\___|
               |_|                              |___/
*/
bool CANArchitectureManager::StopMessage(CANBasicTypes::type_uinteger msg_id) {
    CSynch::IntraProcessLock("CANArchitectureManager__start_stop");
    if (!_transmitterManager->StopMessage(msg_id)) {
        CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
        PropagateError("StopMessage", _transmitterManager);
        return false;
        };
    // message stopped
    CSynch::IntraProcessUnlock("CANArchitectureManager__start_stop");
    return true;
    }


/*
  ____      _   ____                     _       _   _
 / ___| ___| |_|  _ \  ___ ___  ___ _ __(_)_ __ | |_(_) ___  _ __
| |  _ / _ \ __| | | |/ _ Y __|/ __| '__| | '_ \| __| |/ _ \| '_ \
| |_| |  __/ |_| |_| |  __|__ \ (__| |  | | |_) | |_| | (_) | | | |
 \____|\___|\__|____/ \___|___/\___|_|  |_| .__/ \__|_|\___/|_| |_|
                                          |_|
*/
std::string CANArchitectureManager::GetDescription() {
#define EOL std::endl
    // Description of architecture
    std::stringstream s;
    // ------- ECU list
    s << "[ECU list]" << EOL << _ecuManager->GetDescription() << EOL;
    s << "[MESSAGES list]" << EOL << _messageManager->GetDescription() << EOL;
    s << "[SIGNALS list]" << EOL << _signalManager->GetDescription() << EOL;
    return s.str();

    }