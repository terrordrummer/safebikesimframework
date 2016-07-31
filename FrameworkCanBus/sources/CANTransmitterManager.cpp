#include "FrameworkCanBus/CANTransmitterManager.h"
#include "FrameworkCanBus/CANHardwareConfiguration.h"
#include "FrameworkCanBus/CANHardwareInterface.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace std;

/*
  ____    _    _   _ _____                              _ _   _            __  __
 / ___|  / \  | \ | |_   _| __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___ _ __|  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| | | || '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \ '__| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  | | || | | (_| | | | \__ \ | | | | | | |_| ||  __/ |  | |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_| |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|_|  |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                                    |___/
*/
CANTransmitterManager::CANTransmitterManager() {
    SetErrorClassPrefix("CANTransmitterManager");
    _transmissionList.clear();
    _messageStatistics.clear();
    _periodicMessageTimeclock.clear();
    _messageManager = NULL;
    _hardwareManager = NULL;
    _signalManager = NULL;
    _callbackManager = NULL;
    //synch = CSynch();
    ThreadRun(std::string("CANTransmitterManager::TRANSMITTER"), NULL);
    };


/*
 /\/|___    _    _   _ _____                              _ _   _            __  __
|/\/ ___|  / \  | \ | |_   _| __ __ _ _ __  ___ _ __ ___ (_) |_| |_ ___ _ __|  \/  | __ _ _ __   __ _  __ _  ___ _ __
  | |     / _ \ |  \| | | || '__/ _` | '_ \/ __| '_ ` _ \| | __| __/ _ \ '__| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
  | |___ / ___ \| |\  | | || | | (_| | | | \__ \ | | | | | | |_| ||  __/ |  | |  | | (_| | | | | (_| | (_| |  __/ |
   \____/_/   \_\_| \_| |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|\__\___|_|  |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                                      |___/
*/
CANTransmitterManager::~CANTransmitterManager() {
    // Stop all messages
    _periodicMessageListType::iterator it = _transmissionList.begin();
    while (it != _transmissionList.end()) {
        // delete active message
        delete (*it);
        _transmissionList.erase(it);
        it++;
        };
    // terminate transmitter thread
    ThreadTerminate(std::string("CANTransmitterManager::TRANSMITTER"));
    _messageStatistics.clear();
    _periodicMessageTimeclock.clear();
    };

/*
 ____            _    __  __
|  _ \ __ _  ___| | _|  \/  | ___ ___ ___  __ _  __ _  ___
| |_) / _` |/ __| |/ / |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
|  __/ (_| | (__|   <| |  | |  __|__ \__ \ (_| | (_| |  __/
|_|   \__,_|\___|_|\_\_|  |_|\___|___/___/\__,_|\__, |\___|
                                                |___/
*/
void CANTransmitterManager::PackMessage(CANMessage *message) {
    // create local temporary data
    unsigned char msg_data[8] = {0,0,0,0,0,0,0,0};
    // cycle through the packed signals and insert values into the data
    CANMessageTypes::signal_set_type aux_signals = message->get_signals();
    CANMessageTypes::signal_set_type::iterator it = aux_signals.begin();
    while (it != aux_signals.end()) {
        // get the starting bit
        int starting_bit = message->get_signal_starting_bit(*it);
        // get the signal descriptor
        CANSignal *signal = _signalManager->get_signal(*it);
        // signal callback before packing
        _callbackManager->TransmitSignalCallbackCALL(*it, CANCallbackManager::SIGNAL_BEFORE_PACKING, signal);
        // get signal's properties
        CANSignalProperties *sig_prop = signal->get_properties();
        // extract useful vars
        double maxval = sig_prop->get_maxval();
        double minval = sig_prop->get_minval();
        double offset = sig_prop->get_offset();
        double factor = sig_prop->get_factor();
        double value  = signal->get_value();
        unsigned long long aux_val;
        // encode signal's value
        aux_val = static_cast<unsigned long long>((min(max(value,minval),maxval) - offset)/factor);
        // packing bits into the message's data
        unsigned char cur_byte = starting_bit/8;
        unsigned char cur_bit = starting_bit - cur_byte*8;
        unsigned char direction;
        if (signal->get_properties()->get_byte_order() == CANSignalTypes::INTEL_BYTEORDER)
            direction = 1;
        else
            direction = -1;
        // cycle through the message's bits and set its value
        for (int i=0; i < sig_prop->get_length(); i++) {
            unsigned char      byte_mask = 1 << cur_bit;
            //msg_data[cur_byte] = (msg_data[cur_byte] & ~byte_mask) | (((unsigned char)(aux_val >> i & 0x01)) << cur_bit);
            msg_data[cur_byte] = msg_data[cur_byte] | (((unsigned char)(aux_val >> i)) << cur_bit);
            cur_bit++;
            if (cur_bit == 8) {
                cur_bit = 0;
                cur_byte += direction;
                }
            }
        // signal callback after packing
        _callbackManager->TransmitSignalCallbackCALL(*it, CANCallbackManager::SIGNAL_AFTER_PACKING, signal);
        // signal packed
        it++;
        }
    // assign message's data
    message->set_data(msg_data);
    };

/*
 _____                              _ _   __  __
|_   _| __ __ _ _ __  ___ _ __ ___ (_) |_|  \/  | ___ ___ ___  __ _  __ _  ___
  | || '__/ _` | '_ \/ __| '_ ` _ \| | __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
  | || | | (_| | | | \__ \ | | | | | | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
  |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                    |___/
*/
void CANTransmitterManager::TransmitMessage(CANPeriodicMessage *periodic_message, CANBasicTypes::type_string HW_device) {
    _synch.InstanceLock("CANTransmitterManager::TransmitMessage");
    // transmit the message.....
    CANBasicTypes::type_uinteger msg_id = periodic_message->get_properties()->get_id();
    // resets message's data
    periodic_message->ResetData();
    // pre-packing callback
    _callbackManager->TransmitMessageCallbackCALL(  periodic_message->get_properties()->get_id(), 
                                                    CANCallbackManager::MSG_BEFORE_PACKING,
                                                    periodic_message);
    // pack signals
    PackMessage(periodic_message);
    // post-packing callback
    _callbackManager->TransmitMessageCallbackCALL(  periodic_message->get_properties()->get_id(), 
                                                    CANCallbackManager::MSG_AFTER_PACKING_BUT_BEFORE_TRANSMISSION,
                                                    periodic_message);
    
    // ------------------ TRANSMIT MESSAGE ---------------------
    _hardwareManager->GetHardwareInterface(HW_device)->Transmit(periodic_message);
    _hardwareManager->GetHardwareInterface(HW_device)->LoopbackTransmit(periodic_message);
    // ---------------------------------------------------------

    // post-transmission callback
    _callbackManager->TransmitMessageCallbackCALL(  periodic_message->get_properties()->get_id(), 
                                                    CANCallbackManager::MSG_AFTER_TRANSMISSION,
                                                    periodic_message);
    
    _synch.InstanceLock("CANTransmitterManager::_messageStatistics");
    // update message's statistics
    if (_messageStatistics[msg_id].n_transmissions == 0) {
        // init the first value of the mean time
        _messageStatistics[msg_id].t_mean = _periodicMessageTimeclock[msg_id].Toc();
        }
    else {
        // filter a bit the mean value
        double alpha = 0.9;
        _messageStatistics[msg_id].t_mean = _messageStatistics[msg_id].t_mean*alpha + (1.0-alpha)*_periodicMessageTimeclock[msg_id].Toc();
        }
    _periodicMessageTimeclock[msg_id].Tic();
    _messageStatistics[msg_id].n_transmissions++;
    _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
    // compute the next transmitting time (discard too old messages)
    CANBasicTypes::type_time next_transmitting_time = periodic_message->get_next_transmitting_time();
    CANBasicTypes::type_time cycle_time             = periodic_message->get_properties()->get_cycle_time();

	if (CTime::GetTIME() > next_transmitting_time + cycle_time) {
        periodic_message->set_next_transmitting_time(CTime::GetTIME() + cycle_time);
        } else {
            periodic_message->set_next_transmitting_time(next_transmitting_time + cycle_time);
        };
    
    _synch.InstanceUnlock("CANTransmitterManager::TransmitMessage");
    };

/*
 _____                              _ _   __  __
|_   _| __ __ _ _ __  ___ _ __ ___ (_) |_|  \/  | ___ ___ ___  __ _  __ _  ___
  | || '__/ _` | '_ \/ __| '_ ` _ \| | __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
  | || | | (_| | | | \__ \ | | | | | | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
  |_||_|  \__,_|_| |_|___/_| |_| |_|_|\__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                    |___/
*/
void CANTransmitterManager::TransmitMessage(CANMessage *message, CANBasicTypes::type_string HW_device) {
    _synch.InstanceLock("CANTransmitterManager::TransmitMessage");
    // transmit the message...
    CANBasicTypes::type_uinteger msg_id = message->get_properties()->get_id();
    CANHardwareInterface *HIF = _hardwareManager->GetHardwareInterface(HW_device);
    // call the callback BEFORE packing the signals
    // pre-packing callback
    _callbackManager->TransmitMessageCallbackCALL(  message->get_properties()->get_id(), 
                                                    CANCallbackManager::MSG_BEFORE_PACKING,
                                                    message);
    // pack signals
    PackMessage(message);
    // post-packing callback
    _callbackManager->TransmitMessageCallbackCALL(  message->get_properties()->get_id(), 
                                                    CANCallbackManager::MSG_AFTER_PACKING_BUT_BEFORE_TRANSMISSION,
                                                    message);
    // ------------------ TRANSMIT MESSAGE ---------------------
    bool res =  HIF->Transmit(message);
    res = res & HIF->LoopbackTransmit(message);
    // ---------------------------------------------------------

    // post-transmission callback
    _callbackManager->TransmitMessageCallbackCALL(  message->get_properties()->get_id(), 
                                                    CANCallbackManager::MSG_AFTER_TRANSMISSION,
                                                    message);
    _synch.InstanceLock("CANTransmitterManager::_messageStatistics");
    if (res) {
        // update message's statistics
        if (_messageStatistics[msg_id].n_transmissions == 0) {
            // init the first value of the mean time
            _messageStatistics[msg_id].t_mean = _periodicMessageTimeclock[msg_id].Toc();
        } else {
            // filter a bit the mean value
            double alpha = 0.9;
            _messageStatistics[msg_id].t_mean = _messageStatistics[msg_id].t_mean*alpha + (1.0-alpha)*_periodicMessageTimeclock[msg_id].Toc();
            }
        _periodicMessageTimeclock[msg_id].Tic();
        _messageStatistics[msg_id].n_transmissions++;
        }
    _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
    _synch.InstanceUnlock("CANTransmitterManager::TransmitMessage");
    };

/*
 ____               _   __  __                               ____  _        _   _     _   _
|  _ \ ___ ___  ___| |_|  \/  | ___ ___ ___  __ _  __ _  ___/ ___|| |_ __ _| |_(_)___| |_(_) ___ ___
| |_) / _ Y __|/ _ \ __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \___ \| __/ _` | __| / __| __| |/ __/ __|
|  _ <  __|__ \  __/ |_| |  | |  __|__ \__ \ (_| | (_| |  __/___) | || (_| | |_| \__ \ |_| | (__\__ \
|_| \_\___|___/\___|\__|_|  |_|\___|___/___/\__,_|\__, |\___|____/ \__\__,_|\__|_|___/\__|_|\___|___/
                                                  |___/
*/
void CANTransmitterManager::ResetMessageStatistics(CANBasicTypes::type_uinteger message_id) {
    _synch.InstanceLock("CANTransmitterManager::_messageStatistics");
    // retrive the messsage
    statistics_map_type::iterator it = _messageStatistics.find(message_id);
    if (it == _messageStatistics.end()) {
        _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
        // no statistics collected for this message, trace event
        *this << "[ResetMessageStatistics] no available statistics for message 0x" << hex << message_id << std::endl;
        return;
        }
    // reset statistics
    
    _messageStatistics[message_id].n_transmissions = 0;
    _messageStatistics[message_id].t_mean          = 0;
    _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
    };


/*
 ____               _      _    _ _ __  __                               ____  _        _   _     _   _
|  _ \ ___ ___  ___| |_   / \  | | |  \/  | ___ ___ ___  __ _  __ _  ___/ ___|| |_ __ _| |_(_)___| |_(_) ___ ___
| |_) / _ Y __|/ _ \ __| / _ \ | | | |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \___ \| __/ _` | __| / __| __| |/ __/ __|
|  _ <  __|__ \  __/ |_ / ___ \| | | |  | |  __|__ \__ \ (_| | (_| |  __/___) | || (_| | |_| \__ \ |_| | (__\__ \
|_| \_\___|___/\___|\__/_/   \_\_|_|_|  |_|\___|___/___/\__,_|\__, |\___|____/ \__\__,_|\__|_|___/\__|_|\___|___/
                                                              |___/
*/
void CANTransmitterManager::ResetAllMessageStatistics() {
    _synch.InstanceLock("CANTransmitterManager::_messageStatistics");
    // clear all the existing statistics
    statistics_map_type::iterator it = _messageStatistics.begin();
    while (it != _messageStatistics.end()) {
        it->second.n_transmissions = 0;
        it->second.t_mean          = 0;
        it++;
        }
    _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
    };

/*
 ___                     _
|_ _|_ __  ___  ___ _ __| |_
 | || '_ \/ __|/ _ \ '__| __|
 | || | | \__ \  __/ |  | |_
|___|_| |_|___/\___|_|   \__|
*/
void CANTransmitterManager::Insert(CANPeriodicMessage *periodic_message) {
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
    // start from the beginning and find the first message which has higher next_trantmitting_time field
    _periodicMessageListType::iterator it = _transmissionList.begin();
    while (it != _transmissionList.end() && periodic_message->get_next_transmitting_time() > (*it)->get_next_transmitting_time())
        it++;
    _transmissionList.insert(it, periodic_message);
    _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
    };



/*
----------------------------------------------------------------------------------------------------------------
                     ____  _   _ ____  _     ___ ____ __  __ _____ _____ _   _  ___  ____  ____
                    |  _ \| | | | __ )| |   |_ _/ ___|  \/  | ____|_   _| | | |/ _ \|  _ \/ ___|
                    | |_) | | | |  _ \| |    | | |   | |\/| |  _|   | | | |_| | | | | | | \___ \
                    |  __/| |_| | |_) | |___ | | |___| |  | | |___  | | |  _  | |_| | |_| |___) |
                    |_|    \___/|____/|_____|___\____|_|  |_|_____| |_| |_| |_|\___/|____/|____/
----------------------------------------------------------------------------------------------------------------
*/


/*
 ____       _   __  __
/ ___|  ___| |_|  \/  | __ _ _ __   __ _  __ _  ___ _ __ ___
\___ \ / _ \ __| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__/ __|
 ___) |  __/ |_| |  | | (_| | | | | (_| | (_| |  __/ |  \__ \
|____/ \___|\__|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|  |___/
                                         |___/
*/
void CANTransmitterManager::SetManagers(CANMessageManager *mm, CANSignalManager *sm, CANHardwareManager *hm, CANCallbackManager *cm) {
    _messageManager  = mm;
    _signalManager   = sm;
    _hardwareManager = hm;
    _callbackManager = cm;
    };

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANTransmitterManager::StartMessage(CANBasicTypes::type_uinteger    msg_id, 
                                         CANBasicTypes::type_string      HW_device) {
    CANMessage *msg = _messageManager->get_message(msg_id);
    if (_messageManager->HasError()) {
        PropagateError("StartMessage", _messageManager);
        return false;
        }
    return StartMessage(msg_id, -1.0, -1, HW_device);
    };

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANTransmitterManager::StartMessage(CANBasicTypes::type_string  msg_name, 
                                         CANBasicTypes::type_string  HW_device) {
    CANMessage *msg = _messageManager->get_message(msg_name);
    if (_messageManager->HasError()) {
        PropagateError("StartMessage", _messageManager);
        return false;
        }
    return StartMessage(msg_name, -1.0, -1, HW_device);
    };

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANTransmitterManager::StartMessage(CANBasicTypes::type_uinteger msg_id, 
                                         double max_lifetime, 
                                         long max_n_transmissions,
                                         CANBasicTypes::type_string  HW_device) {
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
    // create an active message and put it into the trantmission list
    CANMessage *msg = _messageManager->get_message(msg_id);
    if (_messageManager->HasError()) {
        PropagateError("StartMessage", _messageManager);
        _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
        return false;
        };
    // message retrived, create the active message copy and insert the message into the transmission list
    CANPeriodicMessage *am = new CANPeriodicMessage(msg, HW_device);
    am->set_max_lifetime(max_lifetime);
    am->set_max_n_transmissions(max_n_transmissions);
    am->set_next_transmitting_time(CTime::GetTIME() + msg->get_properties()->get_cycle_time());
    Insert(am);
    // initialize statistics if needed
    if (_messageStatistics.find(msg_id) == _messageStatistics.end()) {
        _messageStatistics[msg_id].n_transmissions = 0;
        _messageStatistics[msg_id].t_mean = msg->get_properties()->get_cycle_time();
        };
    _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
    return true;
    };

/*
 ____  _             _   __  __
/ ___|| |_ __ _ _ __| |_|  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _` | '__| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_| | |  | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\__,_|_|   \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                   |___/
*/
bool CANTransmitterManager::StartMessage(CANBasicTypes::type_string msg_name, 
                                         double max_lifetime, 
                                         long max_n_transmissions,
                                         CANBasicTypes::type_string  HW_device) {
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
    // create an active message and put it into the trantmission list
    CANMessage *msg = _messageManager->get_message(msg_name);
    if (_messageManager->HasError()) {
        PropagateError("StartMessage", _messageManager);
        _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
        return false;
        };
    // message retrived, create the active message copy and insert the message into the transmission list
    CANPeriodicMessage *am = new CANPeriodicMessage(msg, HW_device);
    am->set_max_lifetime(max_lifetime);
    am->set_max_n_transmissions(max_n_transmissions);
    am->set_next_transmitting_time(CTime::GetTIME() + msg->get_properties()->get_cycle_time());
    Insert(am);
    // initialize statistics if needed
    if (_messageStatistics.find(msg->get_properties()->get_id()) == _messageStatistics.end()) {
        _messageStatistics[msg->get_properties()->get_id()].n_transmissions = 0;
        _messageStatistics[msg->get_properties()->get_id()].t_mean = msg->get_properties()->get_cycle_time();
        };
    _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
    return true;
    };

/*
 ____  _              __  __
/ ___|| |_ ___  _ __ |  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _ \| '_ \| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_) | |_) | |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\___/| .__/|_|  |_|\___|___/___/\__,_|\__, |\___|
               |_|                              |___/
*/
bool CANTransmitterManager::StopMessage(CANBasicTypes::type_uinteger msg_id) {
    // scan the message list and find the required message
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
    _periodicMessageListType::iterator it = _transmissionList.begin();
    while (it != _transmissionList.end()) {
        if (msg_id == (*it)->get_properties()->get_id()) {
            // remove statistical info
            _messageStatistics.erase(msg_id);
            // delete the active message
            delete (*it);
            // remove the active message fron the trnamission list
            _transmissionList.erase(it);
            _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
            return true;
            };
        it++;
        }
    _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
    // message not found, trace event
    *this << "[StopMessage] message 0x" << hex << msg_id << "not found" << std::endl;
    return false;
    };

/*
 ____  _              __  __
/ ___|| |_ ___  _ __ |  \/  | ___ ___ ___  __ _  __ _  ___
\___ \| __/ _ \| '_ \| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
 ___) | || (_) | |_) | |  | |  __|__ \__ \ (_| | (_| |  __/
|____/ \__\___/| .__/|_|  |_|\___|___/___/\__,_|\__, |\___|
               |_|                              |___/
*/
bool CANTransmitterManager::StopMessage(CANBasicTypes::type_string msg_name) {
    // scan the message list and find the required message
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
    _periodicMessageListType::iterator it = _transmissionList.begin();
    while (it != _transmissionList.end()) {
        if (msg_name == (*it)->get_properties()->get_name()) {
            // delete the active message
            delete (*it);
            // remove the active message fron the trnamission list
            _transmissionList.erase(it);
            _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
            return true;
            };
        it++;
    }
    _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
    // message not found, trace event
    *this << "[StopMessage] message <" << msg_name << "> not found" << std::endl;
    return false;
    };

/*
  ___             ____  _           _   __  __
 / _ \ _ __   ___/ ___|| |__   ___ | |_|  \/  | ___ ___ ___  __ _  __ _  ___
| | | | '_ \ / _ \___ \| '_ \ / _ \| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |_| | | | |  __/___) | | | | (_) | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
 \___/|_| |_|\___|____/|_| |_|\___/ \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                  |___/
*/
bool CANTransmitterManager::OneShotMessage(CANBasicTypes::type_uinteger msg_id,
                                      CANBasicTypes::type_string   HW_device) {
    // message must exist
    CANMessage *msg = _messageManager->get_message(msg_id);
    if (_messageManager->HasError()) {
        PropagateError("OneShotMessage", _messageManager);
        return false;
        }
    // message retrived, trantmit the message on the CANBUS
    TransmitMessage(msg, HW_device);
    return true;
    };

/*
  ___             ____  _           _   __  __
 / _ \ _ __   ___/ ___|| |__   ___ | |_|  \/  | ___ ___ ___  __ _  __ _  ___
| | | | '_ \ / _ \___ \| '_ \ / _ \| __| |\/| |/ _ Y __/ __|/ _` |/ _` |/ _ \
| |_| | | | |  __/___) | | | | (_) | |_| |  | |  __|__ \__ \ (_| | (_| |  __/
 \___/|_| |_|\___|____/|_| |_|\___/ \__|_|  |_|\___|___/___/\__,_|\__, |\___|
                                                                  |___/
*/
bool CANTransmitterManager::OneShotMessage(CANBasicTypes::type_string msg_name,
                                      CANBasicTypes::type_string   HW_device) {
    CANMessage *msg = _messageManager->get_message(msg_name);
    if (_messageManager->HasError()) {
        PropagateError("OneShotMessage", _messageManager);
        return false;
        }
    // message retrived, transmit the message on the CANBUS
    TransmitMessage(msg, HW_device);
    return true;
    };

/*
 _____ _                        _          _ __  __      _   _               _
|_   _| |__  _ __ ___  __ _  __| | ___  __| |  \/  | ___| |_| |__   ___   __| |
  | | | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | |\/| |/ _ \ __| '_ \ / _ \ / _` |
  | | | | | | | |  __/ (_| | (_| |  __/ (_| | |  | |  __/ |_| | | | (_) | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|\___|\__,_|_|  |_|\___|\__|_| |_|\___/ \__,_|
*/
void CANTransmitterManager::ThreadedMethod(std::string thread_name, void *params) {
    CTime::time_type this_time;
    std::list<CANPeriodicMessage *>::iterator it;
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
    // do something only if the list is not empty
    if (_transmissionList.begin() != _transmissionList.end()) {
        while (1) {
            // extract the message at the beginning of the list
            it = _transmissionList.begin();
            this_time = CTime::GetTIME();
            if (it != _transmissionList.end() && (*it)->get_next_transmitting_time() <= this_time) {
                // TIME TO TRANSMIT THE MESSAGE
                // store the message pointer
                CANPeriodicMessage *periodic_message = *it;
                // trantmit the message
                TransmitMessage(periodic_message, periodic_message->get_HW_device());
                // check for lifetime
                CANBasicTypes::type_uinteger msg_id = periodic_message->get_properties()->get_id();
                CANBasicTypes::type_time msg_timeclock = _periodicMessageTimeclock[msg_id].Toc();
                // remove the message from the front of the list
                _transmissionList.erase(it);
                if ((periodic_message->get_max_lifetime() > 0) && (msg_timeclock > periodic_message->get_max_lifetime()) ||
                    (periodic_message->get_max_n_transmissions() > 0) && (_messageStatistics[msg_id].n_transmissions >= periodic_message->get_max_n_transmissions())) {
                    // lifetime expired or maximum number of times the message was sent reached, remove the mesage
                    delete periodic_message;
                } else {
                    // re-insert the mesage in the list
                    Insert(periodic_message);
                    }
                }
            else
                break;
            }
        // time to sleep a while
        if (it != _transmissionList.end()) {
            DWORD time_to_sleep = static_cast<DWORD>(((*it)->get_next_transmitting_time() - this_time)*1000);
            _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
            Sleep(time_to_sleep);
            }
    } else  {
        _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
        Sleep(100);
        }
    };

/*
  ____      _   ____  _        _   _     _   _
 / ___| ___| |_/ ___|| |_ __ _| |_(_)___| |_(_) ___ ___
| |  _ / _ \ __\___ \| __/ _` | __| / __| __| |/ __/ __|
| |_| |  __/ |_ ___) | || (_| | |_| \__ \ |_| | (__\__ \
 \____|\___|\__|____/ \__\__,_|\__|_|___/\__|_|\___|___/
*/
bool CANTransmitterManager::GetStatistics(CANBasicTypes::type_uinteger msg_id, CANMessageStatistics *stat) {
    _synch.InstanceLock("CANTransmitterManager::_messageStatistics");
    // search statistics for given message
    statistics_map_type::iterator it = _messageStatistics.find(msg_id);
    if (it == _messageStatistics.end()) {
        _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
        // message not found, log event
        std::stringstream s;
        s << "message 0x" << hex << msg_id << " not found" << std::endl;
        PushError("GetStatistics", s.str());
        return false;
        };
    *stat = it->second;
    _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
    return true;
    };

/*
  ____      _   ____  _        _   _     _   _
 / ___| ___| |_/ ___|| |_ __ _| |_(_)___| |_(_) ___ ___
| |  _ / _ \ __\___ \| __/ _` | __| / __| __| |/ __/ __|
| |_| |  __/ |_ ___) | || (_| | |_| \__ \ |_| | (__\__ \
 \____|\___|\__|____/ \__\__,_|\__|_|___/\__|_|\___|___/
*/
bool CANTransmitterManager::GetStatistics(CANBasicTypes::type_string msg_name, CANMessageStatistics *stat) {
    _synch.InstanceLock("CANTransmitterManager::_messageStatistics");
    // search statistics for given message
    CANMessage *msg = _messageManager->get_message(msg_name);
    if (_messageManager->HasError()) {
        PropagateError("GetStatistics", _messageManager);
        _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
        return false;
        }
    statistics_map_type::iterator it = _messageStatistics.find(msg->get_properties()->get_id());
    if (it == _messageStatistics.end()) {
        // message not found, log event
        std::stringstream s;
        s << "message <" << msg_name << "> not found" << std::endl;
        PushError("GetStatistics", s.str());
        _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
        return false;
        };
    *stat = it->second;
    _synch.InstanceUnlock("CANTransmitterManager::_messageStatistics");
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
std::string CANTransmitterManager::GetDescription() {
    _synch.InstanceLock("CANTransmitterManager::_transmissionList");
#define EOL std::endl
    std::stringstream s;
    s << "[CANMEssageTransmitter]" << EOL;
    s << "-- active messages list --" << EOL;
    _periodicMessageListType::iterator it = _transmissionList.begin();
    while (it != _transmissionList.end()) {
        s << "name: <" << (*it)->get_properties()->get_name() << ">, id 0x" << hex << (*it)->get_properties()->get_id() << EOL;
        it++;
        };
    s << "--------------------------" << EOL;
    _synch.InstanceUnlock("CANTransmitterManager::_transmissionList");
    return s.str();
    };