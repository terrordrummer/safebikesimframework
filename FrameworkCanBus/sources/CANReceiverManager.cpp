#include "FrameworkCanBus/CANReceiverManager.h"
#include "FrameworkCanBus/CANHardwareInterface.h"

using namespace FrameworkCanBus;
using namespace std;

/*
 _____ _                        _          _ __  __      _   _               _
|_   _| |__  _ __ ___  __ _  __| | ___  __| |  \/  | ___| |_| |__   ___   __| |
  | | | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | |\/| |/ _ \ __| '_ \ / _ \ / _` |
  | | | | | | | |  __/ (_| | (_| |  __/ (_| | |  | |  __/ |_| | | | (_) | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|\___|\__,_|_|  |_|\___|\__|_| |_|\___/ \__,_|
*/
void CANReceiverManager::ThreadedMethod(std::string thread_name, void *params) {
    // check receiving flag
    if (_discardAll) return;
    
    bool received = true;
    _signalIsFromLoopbackMessage = false;
    // params contains the pointer to the HIF
    CANHardwareInterface *HIF = (CANHardwareInterface *)params;
    CANMessage msg;
    //if (msg.get_data().size()!=0)
    //    cout << "NO EMPTY MSG" << endl;
    // wait for a message
    if (!HIF->Receive(&msg)) {
        // look into the loopback buffer for new messages
        if (!HIF->LoopbackReceive(&msg)) {
            // no messages ready, wait 1ms
            Sleep(1);
            received = false;
            }
        else
            _signalIsFromLoopbackMessage = true;
    }

    if (received) {
        // call installed callback
        _callbackManager->ReceiveMessageCallbackCALL(msg.get_properties()->get_id(), CANCallbackManager::MSG_BEFORE_UNPACKING, &msg);
        // unpack signals only if not loopback (architecture is already updated at transmit time)
        UnpackMessage(&msg);
        // call installed callback
        _callbackManager->ReceiveMessageCallbackCALL(msg.get_properties()->get_id(), CANCallbackManager::MSG_AFTER_UNPACKING, &msg);
        }
    }

/*
 _   _                        _    __  __
| | | |_ __  _ __   __ _  ___| | _|  \/  | ___ ___ ___  __ _  __ _  ___
| | | | '_ \| '_ \ / _` |/ __| |/ / |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |_| | | | | |_) | (_| | (__|   <| |  | |  __|__ \__ \ (_| | (_| |  __/
 \___/|_| |_| .__/ \__,_|\___|_|\_\_|  |_|\___|___/___/\__,_|\__, |\___|
            |_|                                              |___/
*/
void CANReceiverManager::UnpackMessage(CANMessage *message) {
    // cycle through the packed signals and insert values into the data
    CANMessage *architecture_msg = _messageManager->get_message(message->get_properties()->get_id());
    if (architecture_msg == NULL) {
        // no message exist in the architecture, remove the error in the _messageManager
        _messageManager->PopLastError();
        return;
        }
    CANMessageTypes::signal_set_type aux_signals = architecture_msg->get_signals();
    CANMessageTypes::signal_set_type::iterator it = aux_signals.begin();
    while (it != aux_signals.end()) {
        // get the starting bit
        int starting_bit = architecture_msg->get_signal_starting_bit(*it);
        // get the signal descriptor
        CANSignal *signal = _signalManager->get_signal(*it);
        // get signal's properties
        CANSignalProperties *sig_prop = signal->get_properties();
        // signal's unpacking callback
        _callbackManager->ReceiveSignalCallbackCALL(sig_prop->get_name(), CANCallbackManager::SIGNAL_BEFORE_UNPACKING, signal);
        // unpacking bits from the signal packed data
        unsigned char cur_byte = starting_bit/8;
        unsigned char cur_bit = starting_bit - cur_byte*8;
        unsigned char direction;
        if (signal->get_properties()->get_byte_order() == CANSignalTypes::INTEL_BYTEORDER)
            direction = 1;
        else
            direction = -1;
        // signal packed bits value
        unsigned long long aux_val = 0;
        // cycle through the packed signal value's bits and extract its value
        for (int i=0; i < sig_prop->get_length(); i++) {
            // get the current byte
            unsigned long long byte_value = (*message)[cur_byte];
            //byte_value = byte_value >> cur_bit;
            unsigned long long bit = (byte_value >> cur_bit) % 2;
            byte_value = bit << i;
            aux_val = aux_val | byte_value;
            cur_bit++;
            if (cur_bit == 8) {
                cur_bit = 0;
                cur_byte += direction;
                }
            }
        // extract useful vars
        CANBasicTypes::type_float maxval = sig_prop->get_maxval();
        CANBasicTypes::type_float minval = sig_prop->get_minval();
        CANBasicTypes::type_float offset = sig_prop->get_offset();
        CANBasicTypes::type_float factor = sig_prop->get_factor();
        // calculate signal's value
		CANBasicTypes::type_float value = aux_val;
		if(sig_prop->get_value_type() == CANSignalTypes::SIGNED_SIGNAL) {
			long long max_range_val = 1<<sig_prop->get_length();
			if ( value > (max_range_val >> 1) )
				value = (long long)aux_val - max_range_val;
		}
		value = value*factor + offset;
        // signal's value is stored ONLY if it is not received form a loopback message
        if (!_signalIsFromLoopbackMessage)
            signal->set_value(value);
        // signal's unpacking callback
        _callbackManager->ReceiveSignalCallbackCALL(sig_prop->get_name(), CANCallbackManager::SIGNAL_AFTER_UNPACKING, signal);
        it++;
        }
    }

/*
  ____    _    _   _ __  __                               ____               _
 / ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___|  _ \ ___  ___ ___(_)_   _____ _ __
| |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \ |_) / _ \/ __/ _ \ \ \ / / _ \ '__|
| |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/  _ <  __/ (_|  __/ |\ V /  __/ |
 \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|_| \_\___|\___\___|_| \_/ \___|_|
                                               |___/
*/
CANReceiverManager::CANReceiverManager() {
    SetErrorClassPrefix(std::string("CANReceiverManager"));
    _runningThreads.clear();
    _messageBlacklist.clear();
    _signalBlacklist.clear();
    _messageManager  = NULL;
    _signalManager   = NULL;
    _hardwareManager = NULL;
    _discardAll = false;
    }

/*
 /\/|___    _    _   _ __  __                               ____               _
|/\/ ___|  / \  | \ | |  \/  | ___ ___ ___  __ _  __ _  ___|  _ \ ___  ___ ___(_)_   _____ _ __
  | |     / _ \ |  \| | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \ |_) / _ \/ __/ _ \ \ \ / / _ \ '__|
  | |___ / ___ \| |\  | |  | |  __|__ \__ \ (_| | (_| |  __/  _ <  __/ (_|  __/ |\ V /  __/ |
   \____/_/   \_\_| \_|_|  |_|\___|___/___/\__,_|\__, |\___|_| \_\___|\___\___|_| \_/ \___|_|
                                                 |___/
*/
CANReceiverManager::~CANReceiverManager() {
    // cycle through active threads and terminate them
    _runningThreadsType::iterator it = _runningThreads.begin();
    while(it!=_runningThreads.end()) {
        ThreadTerminate(*it);
        it++;
        }
    _messageBlacklist.clear();
    _signalBlacklist.clear();
    _messageManager  = NULL;
    _signalManager   = NULL;
    _hardwareManager = NULL;
    }

/*
 ____       _   __  __
/ ___|  ___| |_|  \/  | __ _ _ __   __ _  __ _  ___ _ __ ___
\___ \ / _ \ __| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__/ __|
 ___) |  __/ |_| |  | | (_| | | | | (_| | (_| |  __/ |  \__ \
|____/ \___|\__|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|  |___/
                                         |___/
*/
void CANReceiverManager::SetManagers(CANEcuManager *em, CANMessageManager *mm, CANSignalManager *sm, CANHardwareManager *hm, CANCallbackManager *cm) {
    _ecuManager      = em;
    _messageManager  = mm;
    _signalManager   = sm;
    _hardwareManager = hm;
    _callbackManager = cm;
    }
// ---------------------------------------------------------------
//                        ENABLE/DISABLE FEATURES
// ---------------------------------------------------------------
/*
 ____  _             _   ____               _       _
/ ___|| |_ __ _ _ __| |_|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _
\___ \| __/ _` | '__| __| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` |
 ___) | || (_| | |  | |_|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |
|____/ \__\__,_|_|   \__|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |
                                                            |___/
*/
bool CANReceiverManager::StartReceiving(CANBasicTypes::type_string HW_device) {
    ThreadRun(HW_device, (void *)_hardwareManager->GetHardwareInterface(HW_device));
    _runningThreads.insert(HW_device);
    return true;
    }

/*
 ____  _             _   ____               _       _
/ ___|| |_ __ _ _ __| |_|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _
\___ \| __/ _` | '__| __| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` |
 ___) | || (_| | |  | |_|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |
|____/ \__\__,_|_|   \__|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |
                                                            |___/
*/
bool CANReceiverManager::StartReceiving(unsigned int deviceID) {
    CANHardwareInterface *HIF = _hardwareManager->GetHardwareInterface(deviceID);
    if (!HIF) return false;
    ThreadRun(HIF->getName(), (void *)HIF);
    _runningThreads.insert(HIF->getName());
    return true;
    }

/*
 ____  _              ____               _       _
/ ___|| |_ ___  _ __ |  _ \ ___  ___ ___(_)_   _(_)_ __   __ _
\___ \| __/ _ \| '_ \| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` |
 ___) | || (_) | |_) |  _ <  __/ (_|  __/ |\ V /| | | | | (_| |
|____/ \__\___/| .__/|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |
               |_|                                       |___/
*/
bool CANReceiverManager::StopReceiving(CANBasicTypes::type_string HW_device) {
    ThreadTerminate(HW_device);
    _runningThreads.erase(HW_device);
    return true;
    }

/*
 ____  _              ____               _       _
/ ___|| |_ ___  _ __ |  _ \ ___  ___ ___(_)_   _(_)_ __   __ _
\___ \| __/ _ \| '_ \| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` |
 ___) | || (_) | |_) |  _ <  __/ (_|  __/ |\ V /| | | | | (_| |
|____/ \__\___/| .__/|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |
               |_|                                       |___/
*/
bool CANReceiverManager::StopReceiving(unsigned int deviceID) {
    CANHardwareInterface *HIF = _hardwareManager->GetHardwareInterface(deviceID);
    if (!HIF) return false;
    ThreadTerminate(HIF->getName());
    _runningThreads.erase(HIF->getName());
    return true;
    }

// ---------------------------------------------------------------
//                        ENABLE/DISABLE FEATURES
// ---------------------------------------------------------------

/*
 ____               _       _             _____ _ _ _             _____             _     _      _____ ____ _   _ __  __
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ | ____|_ __   __ _| |__ | | ___| ____/ ___| | | |  \/  | ___ ___ ___  __ _  __ _  ___ ___
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__||  _| | '_ \ / _` | '_ \| |/ _ \  _|| |   | | | | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ Y __|
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |___| | | | (_| | |_) | |  __/ |__| |___| |_| | |  | |  __|__ \__ \ (_| | (_| |  __|__ \
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|_____|_| |_|\__,_|_.__/|_|\___|_____\____|\___/|_|  |_|\___|___/___/\__,_|\__, |\___|___/
                                    |___/                   |_____|                                                                         |___/
*/
bool CANReceiverManager::ReceivingFilter_EnableECUMessages(CANBasicTypes::type_string ECU_name) {
    // get the list of messages received by the given ecu
    CANEcuTypes::msg_id_set_type msg_set = _ecuManager->get_ecu(ECU_name)->get_received_msg_id_list();
    // remove all the received messages from the black list
    CANEcuTypes::msg_id_set_type::iterator it_msg = msg_set.begin();
    while (it_msg != msg_set.end()) {
        _messageBlacklist.erase(*it_msg);
        it_msg++;
        }
    // get the list of received signals
    CANEcuTypes::signal_set_type signal_set = _ecuManager->get_ecu(ECU_name)->get_received_signal_list();
    // remove the signals from the black list
    CANEcuTypes::signal_set_type::iterator it_sig = signal_set.begin();
    while(it_sig != signal_set.end()) {
        _signalBlacklist.erase(*it_sig);
        it_sig++;
        }
    return true;
    }

/*
 ____               _       _             _____ _ _ _             ____  _           _     _      _____ ____ _   _ __  __
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ |  _ \(_)___  __ _| |__ | | ___| ____/ ___| | | |  \/  | ___ ___ ___  __ _  __ _  ___ ___
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__|| | | | / __|/ _` | '_ \| |/ _ \  _|| |   | | | | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ Y __|
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |_| | \__ \ (_| | |_) | |  __/ |__| |___| |_| | |  | |  __|__ \__ \ (_| | (_| |  __|__ \
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|____/|_|___/\__,_|_.__/|_|\___|_____\____|\___/|_|  |_|\___|___/___/\__,_|\__, |\___|___/
                                    |___/                   |_____|                                                                         |___/
*/
bool CANReceiverManager::ReceivingFilter_DisableECUMessages(CANBasicTypes::type_string ECU_name) {
        // get the list of messages received by the given ecu
    CANEcuTypes::msg_id_set_type msg_set = _ecuManager->get_ecu(ECU_name)->get_received_msg_id_list();
    // remove all the received messages from the black list
    CANEcuTypes::msg_id_set_type::iterator it_msg = msg_set.begin();
    while (it_msg != msg_set.end()) {
        _messageBlacklist.insert(*it_msg);
        it_msg++;
        }
    // get the list of received signals
    CANEcuTypes::signal_set_type signal_set = _ecuManager->get_ecu(ECU_name)->get_received_signal_list();
    // remove the signals from the black list
    CANEcuTypes::signal_set_type::iterator it_sig = signal_set.begin();
    while(it_sig != signal_set.end()) {
        _signalBlacklist.insert(*it_sig);
        it_sig++;
        }
    return true;
    }

/*
 ____               _       _             _____ _ _ _             _____             _     _      __  __
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ | ____|_ __   __ _| |__ | | ___|  \/  | ___ ___ ___  __ _  __ _  ___
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__||  _| | '_ \ / _` | '_ \| |/ _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |___| | | | (_| | |_) | |  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|_____|_| |_|\__,_|_.__/|_|\___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                    |___/                   |_____|                                                        |___/
*/
bool CANReceiverManager::ReceivingFilter_EnableMessage(CANBasicTypes::type_uinteger msg_id) {
    _messageBlacklist.erase(msg_id);
    return true;
    }

/*
 ____               _       _             _____ _ _ _             ____  _           _     _      __  __
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ |  _ \(_)___  __ _| |__ | | ___|  \/  | ___ ___ ___  __ _  __ _  ___
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__|| | | | / __|/ _` | '_ \| |/ _ \ |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |_| | \__ \ (_| | |_) | |  __/ |  | |  __|__ \__ \ (_| | (_| |  __/
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|____/|_|___/\__,_|_.__/|_|\___|_|  |_|\___|___/___/\__,_|\__, |\___|
                                    |___/                   |_____|                                                        |___/
*/
bool CANReceiverManager::ReceivingFilter_DisableMessage(CANBasicTypes::type_uinteger msg_id) {
    _messageBlacklist.insert(msg_id);
    return true;
    }

/*
 ____               _       _             _____ _ _ _             _____             _     _      ____  _                   _
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ | ____|_ __   __ _| |__ | | ___/ ___|(_) __ _ _ __   __ _| |
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__||  _| | '_ \ / _` | '_ \| |/ _ \___ \| |/ _` | '_ \ / _` | |
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |___| | | | (_| | |_) | |  __/___) | | (_| | | | | (_| | |
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|_____|_| |_|\__,_|_.__/|_|\___|____/|_|\__, |_| |_|\__,_|_|
                                    |___/                   |_____|                                      |___/
*/
bool CANReceiverManager::ReceivingFilter_EnableSignal(CANBasicTypes::type_string signal_name) {
    _signalBlacklist.erase(signal_name);
    return true;
    }

/*
 ____               _       _             _____ _ _ _             ____  _           _     _      ____  _                   _
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ |  _ \(_)___  __ _| |__ | | ___/ ___|(_) __ _ _ __   __ _| |
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__|| | | | / __|/ _` | '_ \| |/ _ \___ \| |/ _` | '_ \ / _` | |
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |_| | \__ \ (_| | |_) | |  __/___) | | (_| | | | | (_| | |
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|____/|_|___/\__,_|_.__/|_|\___|____/|_|\__, |_| |_|\__,_|_|
                                    |___/                   |_____|                                      |___/
*/
bool CANReceiverManager::ReceivingFilter_DisableSignal(CANBasicTypes::type_string signal_name) {
    _signalBlacklist.insert(signal_name);
    return true;
    }

/*
 ____               _       _             _____ _ _ _             _____             _     _         _    _ _
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ | ____|_ __   __ _| |__ | | ___   / \  | | |
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__||  _| | '_ \ / _` | '_ \| |/ _ \ / _ \ | | |
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |___| | | | (_| | |_) | |  __// ___ \| | |
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|_____|_| |_|\__,_|_.__/|_|\___/_/   \_\_|_|
                                    |___/                   |_____|
*/
bool CANReceiverManager::ReceivingFilter_EnableAll() {
    // empty all blacklists
    _messageBlacklist.clear();
    _signalBlacklist.clear();
    _discardAll = false;
    return true;
    }

/*
 ____               _       _             _____ _ _ _             ____  _           _     _         _    _ _
|  _ \ ___  ___ ___(_)_   _(_)_ __   __ _|  ___(_) | |_ ___ _ __ |  _ \(_)___  __ _| |__ | | ___   / \  | | |
| |_) / _ \/ __/ _ \ \ \ / / | '_ \ / _` | |_  | | | __/ _ \ '__|| | | | / __|/ _` | '_ \| |/ _ \ / _ \ | | |
|  _ <  __/ (_|  __/ |\ V /| | | | | (_| |  _| | | | ||  __/ |   | |_| | \__ \ (_| | |_) | |  __// ___ \| | |
|_| \_\___|\___\___|_| \_/ |_|_| |_|\__, |_|   |_|_|\__\___|_|___|____/|_|___/\__,_|_.__/|_|\___/_/   \_\_|_|
                                    |___/                   |_____|
*/
bool CANReceiverManager::ReceivingFilter_DisableAll() {
    // add all messages into the blacklist
    _discardAll = true;
    return true;
    }
