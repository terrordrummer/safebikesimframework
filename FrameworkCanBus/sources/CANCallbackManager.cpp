#include "FrameworkCanBus/CANCallbackManager.h"

using namespace FrameworkCanBus;


/*
  ____    _    _   _  ____      _ _ _                _    __  __
 / ___|  / \  | \ | |/ ___|__ _| | | |__   __ _  ___| | _|  \/  | __ _ _ __   __ _  __ _  ___ _ __
| |     / _ \ |  \| | |   / _` | | | '_ \ / _` |/ __| |/ / |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
| |___ / ___ \| |\  | |__| (_| | | | |_) | (_| | (__|   <| |  | | (_| | | | | (_| | (_| |  __/ |
 \____/_/   \_\_| \_|\____\__,_|_|_|_.__/ \__,_|\___|_|\_\_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                                                   |___/
*/
CANCallbackManager::CANCallbackManager() {
    // clear all containers
    _msgTransmissionCallbacksBeforePacking.clear();
    _msgTransmissionCallbacksAfterPacking.clear();
    _msgTransmissionCallbacksAfterTransmission.clear();
    _msgReceivingCallbacksBeforeUnpacking.clear();
    _msgReceivingCallbacksAfterUnpacking.clear();
    _signalBeforePackingCallbacks.clear();
    _signalAfterPackingCallbacks.clear();
    _signalBeforeUnpackingCallbacks.clear();
    _signalAfterUnpackingCallbacks.clear();
    }

// ----------------------------------------------------------
//                     CALLBACK INSTALL 
// ----------------------------------------------------------

/* Install a callback associated to the transmission of a message.      */
bool CANCallbackManager::InstallMessageTransmissionCallback(    CANBasicTypes::type_uinteger        msg_id, 
                                                                CALLBACK_TYPE                       cbk,
                                                                MessageTransmissionCallbackOPTIONS  opt) {
    switch(opt) {
        case MSG_BEFORE_PACKING:
            // install callback
            _msgTransmissionCallbacksBeforePacking[msg_id] = cbk;
            break;
        case MSG_AFTER_PACKING_BUT_BEFORE_TRANSMISSION:
            // install callback
            _msgTransmissionCallbacksAfterPacking[msg_id] = cbk;
            break;
        case MSG_AFTER_TRANSMISSION:
            // install callback
            _msgTransmissionCallbacksAfterTransmission[msg_id] = cbk;
            break;
        default:
            return false;
        }
    return true;
    }


/* Install a callback associated to the reception of a message.         */
bool CANCallbackManager::InstallMessageReceivingCallback(   CANBasicTypes::type_uinteger        msg_id, 
                                                            CALLBACK_TYPE                       cbk,
                                                            MessageReceivingCallbackOPTIONS     opt) {
    switch(opt) {
        case MSG_BEFORE_UNPACKING:
            // install callback
            _msgReceivingCallbacksBeforeUnpacking[msg_id] = cbk;
            break;
        case MSG_AFTER_UNPACKING:
            // install callback
            _msgReceivingCallbacksAfterUnpacking[msg_id] = cbk;
            break;
        default:
            return false;
        }
    return true;
    }


/* Install a callback associated to the transmission of a signal.       */
bool CANCallbackManager::InstallSignalTransmissionCallback( CANBasicTypes::type_string          signal_name, 
                                                            CALLBACK_TYPE                       cbk,
                                                            SignalTransmittingCallbackOPTIONS   opt) {
    switch(opt) {
        case SIGNAL_BEFORE_PACKING:
            // install callback
            _signalBeforePackingCallbacks[signal_name] = cbk;
            break;
        case SIGNAL_AFTER_PACKING:
            // install callback
            _signalAfterPackingCallbacks[signal_name] = cbk;
            break;
        default:
            return false;
        }
    return true;
    }


/* Install a callback associated to the reception of a signal.          */
bool CANCallbackManager::InstallSignalReceivingCallback(    CANBasicTypes::type_string          signal_name, 
                                                            CALLBACK_TYPE                       cbk,
                                                            SignalReceivingCallbackOPTIONS       opt) {
    switch(opt) {
        case SIGNAL_BEFORE_UNPACKING:
            // install callback
            _signalBeforeUnpackingCallbacks[signal_name] = cbk;
            break;
        case SIGNAL_AFTER_UNPACKING:
            // install callback
            _signalAfterUnpackingCallbacks[signal_name] = cbk;
            break;
        default:
            return false;
            break;
        }
    return true;
    }


// ----------------------------------------------------------
//                  CALLBACK REMOVAL 
// ----------------------------------------------------------
/* Removes a callback associated to the transmission of a message with the given options.       */
bool CANCallbackManager::RemoveMessageTransmissionCallback( CANBasicTypes::type_uinteger msg_id ,
                                                            MessageTransmissionCallbackOPTIONS  opt) {
    
    switch(opt) {
        case MSG_BEFORE_PACKING:
            // install callback
            _msgTransmissionCallbacksBeforePacking.erase(msg_id);
            break;
        case MSG_AFTER_PACKING_BUT_BEFORE_TRANSMISSION:
            // install callback
            _msgTransmissionCallbacksAfterPacking.erase(msg_id);
            break;
        case MSG_AFTER_TRANSMISSION:
            // install callback
            _msgTransmissionCallbacksAfterTransmission.erase(msg_id);
            break;
        default:
            return false;
        }
    return true;
    }


/* Removes all the callbacks associated to the transmission of a message.                       */
bool CANCallbackManager::RemoveMessageTransmissionCallback( CANBasicTypes::type_uinteger msg_id) {
    _msgTransmissionCallbacksBeforePacking.erase(msg_id);
    _msgTransmissionCallbacksAfterPacking.erase(msg_id);
    _msgTransmissionCallbacksAfterTransmission.erase(msg_id);
    return true;
    }


/* Removes a callback associated to the reception of a message with the given option.           */
bool CANCallbackManager::RemoveMessageReceivingCallback(    CANBasicTypes::type_uinteger        msg_id,
                                                            MessageReceivingCallbackOPTIONS     opt) {
        switch(opt) {
        case MSG_BEFORE_UNPACKING:
            // install callback
            _msgReceivingCallbacksBeforeUnpacking.erase(msg_id);
            break;
        case MSG_AFTER_UNPACKING:
            // install callback
            _msgReceivingCallbacksAfterUnpacking.erase(msg_id);
            break;
        default:
            return false;
        }
    return true;
    }

/* Removes all the callbacks associated to the reception of a message.                          */
bool CANCallbackManager::RemoveMessageReceivingCallback(    CANBasicTypes::type_uinteger        msg_id) {
    _msgReceivingCallbacksBeforeUnpacking.erase(msg_id);
    _msgReceivingCallbacksAfterUnpacking.erase(msg_id);
    return true;
    }


/* Removes a callback associated to the transmission of a signal with the given option.         */
bool CANCallbackManager::RemoveSignalTransmissionCallback(  CANBasicTypes::type_string          signal_name,
                                                            SignalTransmittingCallbackOPTIONS   opt) {
    switch(opt) {
        case SIGNAL_BEFORE_PACKING:
            // install callback
            _signalBeforePackingCallbacks.erase(signal_name);
            break;
        case SIGNAL_AFTER_PACKING:
            // install callback
            _signalAfterPackingCallbacks.erase(signal_name);
            break;
        default:
            return false;
            break;
        }
    return true;
    }


/* Removes all the callbacks associated to the transmission of a signal.                        */
bool CANCallbackManager::RemoveSignalTransmissionCallback(  CANBasicTypes::type_string          signal_name) {
    _signalBeforePackingCallbacks.erase(signal_name);
    _signalAfterPackingCallbacks.erase(signal_name);
    return true;
    }


/* Removes a callback associated to the reception of a signal with the given option.            */
bool CANCallbackManager::RemoveSignalReceivingCallback(     CANBasicTypes::type_string          signal_name,
                                                            SignalReceivingCallbackOPTIONS      opt) {
        switch(opt) {
        case SIGNAL_BEFORE_PACKING:
            // install callback
            _signalBeforeUnpackingCallbacks.erase(signal_name);
            break;
        case SIGNAL_AFTER_PACKING:
            // install callback
            _signalAfterUnpackingCallbacks.erase(signal_name);
            break;
        default:
            return false;
            break;
        }
    return true;
    }   


/* Removes all the callbacks associated to the reception of a signal.                           */
bool CANCallbackManager::RemoveSignalReceivingCallback(     CANBasicTypes::type_string  signal_name) {
    _signalBeforeUnpackingCallbacks.erase(signal_name);
    _signalAfterUnpackingCallbacks.erase(signal_name);
    return true;
    }

// ----------------------------------------------------------
//                      CALLBACK INVOKATION
// ----------------------------------------------------------
/** \brief Performs the callback for the message if installed.              */
void CANCallbackManager::TransmitMessageCallbackCALL(   CANBasicTypes::type_uinteger        msg_id, 
                                                        MessageTransmissionCallbackOPTIONS  opt,
                                                        CANMessage *msg) {
    CALLBACK_TYPE cbk = NULL;
    _msgCallbackMapType::iterator it;
    switch(opt) {
        case MSG_BEFORE_PACKING:
            it = _msgTransmissionCallbacksBeforePacking.find(msg_id);
            if (it == _msgTransmissionCallbacksBeforePacking.end())
                return;
            cbk = it->second;
            break;
        case MSG_AFTER_PACKING_BUT_BEFORE_TRANSMISSION:
            it = _msgTransmissionCallbacksAfterPacking.find(msg_id);
            if (it == _msgTransmissionCallbacksAfterPacking.end())
                return;
            cbk = it->second;
            break;
        case MSG_AFTER_TRANSMISSION:
            it = _msgTransmissionCallbacksAfterTransmission.find(msg_id);
            if (it == _msgTransmissionCallbacksAfterTransmission.end())
                return;
            cbk = it->second;
            break;
        default:
            return;
        }
    // inkoke the callback
    cbk((void *)msg);
    }

/** \brief Performs the callback for the message if installed.              */
void CANCallbackManager::ReceiveMessageCallbackCALL(    CANBasicTypes::type_uinteger        msg_id, 
                                                        MessageReceivingCallbackOPTIONS     opt,
                                                        CANMessage *msg) {
    CALLBACK_TYPE cbk;
    _msgCallbackMapType::iterator it;
    switch(opt) {
        case MSG_BEFORE_UNPACKING:
            it = _msgReceivingCallbacksBeforeUnpacking.find(msg_id);
            if (it == _msgReceivingCallbacksBeforeUnpacking.end())
                return;
            cbk = it->second;
            break;
        case MSG_AFTER_UNPACKING:
            it = _msgReceivingCallbacksAfterUnpacking.find(msg_id);
            if (it == _msgReceivingCallbacksAfterUnpacking.end())
                return;
            cbk = it->second;
            break;
        default:
            return;
        }
    // inkoke the callback
    cbk((void *)msg);
    }

/** \brief Performs the callback for the signal if installed.               */
void CANCallbackManager::TransmitSignalCallbackCALL(    CANBasicTypes::type_string          signal_name, 
                                                        SignalTransmittingCallbackOPTIONS   opt,
                                                        CANSignal                           *signal) {
    CALLBACK_TYPE cbk;
    _signalCallbackMapType::iterator it;
    switch(opt) {
        case SIGNAL_BEFORE_PACKING:
            it = _signalBeforePackingCallbacks.find(signal_name);
            if (it == _signalBeforePackingCallbacks.end())
                return;
            cbk = it->second;
            break;
        case SIGNAL_AFTER_PACKING:
            it = _signalAfterPackingCallbacks.find(signal_name);
            if (it == _signalAfterPackingCallbacks.end())
                return;
            cbk = it->second;
            break;
        default:
            return;
        }
    // inkoke the callback
    cbk((void *)signal);
    }

/** \brief Performs the callback for the signal if installed.               */
void CANCallbackManager::ReceiveSignalCallbackCALL(     CANBasicTypes::type_string          signal_name, 
                                                        SignalReceivingCallbackOPTIONS      opt,
                                                        CANSignal                           *signal) {
    CALLBACK_TYPE cbk;
    _signalCallbackMapType::iterator it;
    switch(opt) {
        case SIGNAL_BEFORE_PACKING:
            it = _signalBeforeUnpackingCallbacks.find(signal_name);
            if (it == _signalBeforeUnpackingCallbacks.end())
                return;
            cbk = it->second;
            break;
        case SIGNAL_AFTER_PACKING:
            it = _signalAfterUnpackingCallbacks.find(signal_name);
            if (it == _signalAfterUnpackingCallbacks.end())
                return;
            cbk = it->second;
            break;
        default:
            return;
        }
    // inkoke the callback
    cbk((void *)signal);
    }