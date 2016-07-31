#include "FrameworkCanBus/CDbcWrapper.h"
#include "FrameworkCanBus/CANMessageProperties.h"
#include "FrameworkLUA/CWrapperLua.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

using namespace FrameworkCanBus;
using namespace FrameworkLUA;
using namespace std;

/*
  ____ ____  _        __        __
 / ___|  _ \| |__   __\ \      / / __ __ _ _ __  _ __   ___ _ __
| |   | | | | '_ \ / __\ \ /\ / / '__/ _` | '_ \| '_ \ / _ \ '__|
| |___| |_| | |_) | (__ \ V  V /| | | (_| | |_) | |_) |  __/ |
 \____|____/|_.__/ \___| \_/\_/ |_|  \__,_| .__/| .__/ \___|_|
                                          |_|   |_|
*/
CDbcWrapper::CDbcWrapper() {
    SetErrorClassPrefix(std::string("CDbcWrapper"));
    };

/*
 /\/|___ ____  _        __        __
|/\/ ___|  _ \| |__   __\ \      / / __ __ _ _ __  _ __   ___ _ __
  | |   | | | | '_ \ / __\ \ /\ / / '__/ _` | '_ \| '_ \ / _ \ '__|
  | |___| |_| | |_) | (__ \ V  V /| | | (_| | |_) | |_) |  __/ |
   \____|____/|_.__/ \___| \_/\_/ |_|  \__,_| .__/| .__/ \___|_|
                                            |_|   |_|
*/
CDbcWrapper::~CDbcWrapper() {
    // free all allocated data and clear all STL structures
    };

/*
 ____                     ____  ____   ____ _____ _ _
|  _ \ __ _ _ __ ___  ___|  _ \| __ ) / ___|  ___(_) | ___
| |_) / _` | '__/ __|/ _ \ | | |  _ \| |   | |_  | | |/ _ \
|  __/ (_| | |  \__ \  __/ |_| | |_) | |___|  _| | | |  __/
|_|   \__,_|_|  |___/\___|____/|____/ \____|_|   |_|_|\___|
*/
bool CDbcWrapper::ParseDBCFile(     std::string filename,
                                    CANArchitectureManager *architecture_manager,
                                    CANEcuManager *_ecuManager,
                                    CANMessageManager *_messageManager,
                                    CANSignalManager *_signalManager) {
    
    std::ifstream ifs;          // file stream
    std::string architecture;   // file architecture
    ifs.clear();
    architecture.clear();
    
    ifs.open(filename.c_str(), std::ifstream::in);
    if (!ifs.is_open()) {
        std::stringstream s;
        s.clear();
        s << "file not found <" << filename << ">";
        PushError("ParseDBCFile", s.str());
        return false;
        }

    // search and fill the ECU list
    set<CANBasicTypes::type_string> ECU_list;
    ECU_list.clear();
    while (!ifs.eof()) {
        std::getline(ifs,architecture);
        if (architecture.find("BU_:") != architecture.npos) {
            // check for empty list
            if (architecture.length()==4)
                break;
            // architecture found, extrace ECU list
            std::string list = architecture.substr(5,architecture.length());
            int pos = 0;
            while (pos != list.length()) {
                size_t next_space = list.find(" ");
                if (next_space == list.npos) {
                    // last name
                    ECU_list.insert(list);
                    break;
                } else {
                    ECU_list.insert(list.substr(0,next_space));
                    list = list.substr(next_space+1,list.length());
                    }
                };
            break;
            }
        }

    // add ECUs to the manager
    _ecuManager->AddECUList(ECU_list);
    if (_ecuManager->HasError()) {
        // some error occurred while adding an ECU to the architecture
        PropagateError("ParseDBCFile", _ecuManager);
        ifs.close();
        return false;
        };

    // follow the lines and parse messages data
    while (!ifs.eof()) {
        // read architecture
        std::getline(ifs,architecture);
        // skip empty lines
        if (architecture.length() != 0) {
            // look to the architecture header
            if (architecture.substr(0,4).compare("BO_ ")==0) {
                // ECU which is transmitting this message
                CANEcu *transmitting_ecu;
                // local message properties
                CANMessageProperties msg_prop;
                // MESSAGE DATA
                CANBasicTypes::type_uinteger id;
                CANBasicTypes::type_string msg_name;
                CANBasicTypes::type_uinteger dlc;
                CANBasicTypes::type_string transmitter;
                // read properties
                std::stringstream s;
                s.clear();
                s.str(architecture.substr(4,std::string::npos));
                s >> id;
                s >> msg_name;
                s >> dlc;
                s >> transmitter;
                // exclude vector independent message containing the unassigned signals
                if (msg_name.compare("VECTOR__INDEPENDENT_SIG_MSG:") != 0) {
                    // assign properties
                    msg_prop.set_name(msg_name.substr(0,msg_name.length()-1));
                    msg_prop.set_id(id);
                    msg_prop.set_dlc(dlc);
                    if (transmitter.compare("Vector__XXX") != 0) 
                        msg_prop.set_transmitter(transmitter);
                    else
                        msg_prop.set_transmitter("");
                    // check properties consistency
                    if (msg_prop.HasError()) {
                        PropagateError("ParseDBCFile", &msg_prop);
                        ifs.close();
                        return false;
                        }
                    // add message to the architecture
                    _messageManager->AddMessage(&msg_prop);
                    if (_messageManager->HasError()) {
                        PropagateError("ParseDBCFile", _messageManager);
                        ifs.close();
                        return false;
                        }
                    
                    // add the message to the transmitter ECU
                    if (msg_prop.get_transmitter().length()>0) {
                        // transmitter assigned, adding the message in its transmitted list
                        transmitting_ecu = _ecuManager->get_ecu(msg_prop.get_transmitter());
                        if (_ecuManager->HasError()) {
                            // error retriving the transmitter ECU, considered an error
                            PropagateError("ParseDBCFile", _ecuManager);
                            ifs.close();
                            return false;
                            }
                        // ECU retrived, assign transmitted message
                        transmitting_ecu->AddTransmittedMessage(msg_prop.get_id());
                        if (transmitting_ecu->HasError()) {
                            // error assigning transmitted message
                            PropagateError("ParseDBCFile", transmitting_ecu);
                            ifs.close();
                            return false;
                            }
                        }
                    else
                        transmitting_ecu = NULL;

                    /* while cycling through the signals receiving, signal's receiving ECUs set is created
                     * and the message id is assigned to all the ECU in the set as receiving message
                     */
                    std::set<CANBasicTypes::type_string> message_receiving_ecu_list;
                    message_receiving_ecu_list.clear();
                    
                    // CYCLE THROUGH SIGNALS PACKED IN THE MESSAGE
                    std::getline(ifs,architecture);
                    while (architecture.length()>0) {
                        std::string tmp = "";
                        char        single_char;
                        // read signal properties
                        CANBasicTypes::type_string      signal_name;
                        CANBasicTypes::type_uinteger    starting_bit;
                        CANBasicTypes::type_uinteger    byte_order_code;
                        char                            signal_type_code;
                        CANBasicTypes::type_uinteger    length;
                        CANBasicTypes::type_float       factor;
                        CANBasicTypes::type_float       offset;
                        CANBasicTypes::type_float       minval;
                        CANBasicTypes::type_float       maxval;
                        CANBasicTypes::type_string      unit;
                        std::string                     listeners;
                        
                        if (architecture.substr(0,4).compare(" SG_") != 0) {
                            // some error syntax occurred
                            PushError("ParseDBCFile", "SG_ signal declaration not found");
                            ifs.close();
                            return false;
                            }

                        s.clear();
                        s.str(architecture.substr(5,std::string::npos));
                        s >> signal_name >> tmp;
                        s >> starting_bit >> single_char;
                        s >> length >> single_char;
                        s >> byte_order_code;
                        s >> signal_type_code >> single_char;
                        s >> factor >> single_char;
                        s >> offset >> single_char >> single_char;
                        s >> minval >> single_char;
                        s >> maxval >> single_char;
                        std::getline(s, tmp, '"');
                        std::getline(s, unit, '"');
                        std::getline(s, listeners);

                        // CREATE THE SIGNAL AND ADD IT TO THE ARCHITECTURE
                        CANSignalProperties sig_prop;
                        sig_prop.set_name(signal_name);
                        sig_prop.set_length(length);
                        sig_prop.set_factor(factor);
                        sig_prop.set_minval(minval);
                        sig_prop.set_maxval(maxval);
                        sig_prop.set_unit(unit);
                        if (byte_order_code == 0)
                            sig_prop.set_byte_order(CANSignalTypes::MOTOROLA_BYTEORDER);
                        else
                            sig_prop.set_byte_order(CANSignalTypes::INTEL_BYTEORDER);
                        if (signal_type_code == '-')
                            sig_prop.set_value_type(CANSignalTypes::SIGNED_SIGNAL);
                        else
                        sig_prop.set_value_type(CANSignalTypes::UNSIGNED_SIGNAL);
                        // data check
                        if (sig_prop.HasError()) {
                            PropagateError("ParseDBCFile", &sig_prop);
                            ifs.close();
                            return false;
                            }
                        // data ok, add signal to the architecture
                        _signalManager->AddSignal(&sig_prop);

                        // add signal to the current message
                        unsigned int real_starting_bit = starting_bit;
                        if (sig_prop.get_byte_order() == CANSignalTypes::MOTOROLA_BYTEORDER) {
                            // in case of motorola byte order, the starting mit is indeed the FINAL bit, we have to
                            // re-calculate the real starting bit
                            int row = int(starting_bit) / 8;
                            int col = starting_bit - row*8;
                            for (unsigned int i = 0 ; i < length - 1; i++) {
                                if (--col < 0) {
                                    col = 7;
                                    row++;
                                    }
                                }
                            real_starting_bit = row*8 + col;
                            };
                        architecture_manager->AddSignalToMessage(sig_prop.get_name(), msg_prop.get_id(), real_starting_bit);
                        // check errors
                        if (HasError()) {
                            PropagateError("ParseDBCFile");
                            ifs.close();
                            return false;
                            };

                        // add the message in the packing list of the signal
                        CANSignal *signal = _signalManager->get_signal(sig_prop.get_name());
                        signal->AddMessage(msg_prop.get_name());
                        
                        // add the signal to the transmitter ECU in the transmitting signal list
                        if (transmitting_ecu != NULL) {
                            transmitting_ecu->AddTransmittedSignal(sig_prop.get_name());
                            if (transmitting_ecu->HasError()) {
                                // some error occurred while adding the transmitting signal
                                PropagateError("ParseDBCFile", transmitting_ecu);
                                ifs.close();
                                return false;
                                }
                            }

                        // add the current signal to the listener ECU in their listener signal list
                        CANBasicTypes::type_string signal_listener_ecu_name = "";
                        std::stringstream listener_stream;
                        listener_stream.str(listeners);
                        std::getline(listener_stream,signal_listener_ecu_name,',');
                        signal_listener_ecu_name = signal_listener_ecu_name.Strip();
                        while (signal_listener_ecu_name.length() > 0) {
                            // new listener, ECU Vector__XXX as listener means that no listeners exist for this signal
                            if (signal_listener_ecu_name.Strip().compare("Vector__XXX") != 0) {
                                // valid ECU name
                                CANEcu *ECU = _ecuManager->get_ecu(signal_listener_ecu_name);
                                if (_ecuManager->HasError()) {
                                    // for some reason ECU name error occurred while retriving the ECU by its name
                                    PropagateError("ParseDBCFile", _ecuManager);
                                    ifs.close();
                                    return false;
                                    }
                                // ECU retrived, now add the signal to the ECU's signal listening list
                                ECU->AddReceivedSignal(sig_prop.get_name());

                                // refresh ECU message receiving list
                                message_receiving_ecu_list.insert(signal_listener_ecu_name);
                                }
                            signal_listener_ecu_name.assign("");
                            std::getline(listener_stream,signal_listener_ecu_name,',');
                            signal_listener_ecu_name = signal_listener_ecu_name.Strip();
                            }

                        // finally, add the message to the receiving ECUs in their receiving message list
                        std::set<CANBasicTypes::type_string>::iterator it = message_receiving_ecu_list.begin();
                        while (it != message_receiving_ecu_list.end()) {
                            CANEcu *ECU = _ecuManager->get_ecu(*it);
                            if (_ecuManager->HasError()) {
                                // some error is occurred while retriving the ECU
                                PropagateError("ParseDBCFile", _ecuManager);
                                ifs.close();
                                return false;
                                }
                            ECU->AddReceivedMesssage(msg_prop.get_id());
                            it++;
                            };

                        // next architecture
                        std::getline(ifs,architecture);
                        }
                    }
                }
            else if (architecture.substr(0,4).compare("CM_ ")==0) {
                // COMMENT
                std::stringstream s;
                // get the comment target identifier
                std::string comment_identifier;
                s.str(architecture.substr(4,std::string::npos));
                s >> comment_identifier;
                if (comment_identifier.compare("BO_") == 0) {
                    // comment to message
                    CANBasicTypes::type_uinteger msg_id;
                    std::string tmp,comment;
                    s >> msg_id;
                    std::getline(s,tmp,'"');
                    std::getline(s,comment,'"');
                    std::getline(s,tmp,';');
                    // message 3221225472 is dummy message containing unassigned signals, comment is discharged
                    if (msg_id != 3221225472) {
                        CANMessage *msg = _messageManager->get_message(msg_id);
                        if (_messageManager->HasError()) {
                            PropagateError("ParseDBCFile", _messageManager);
                            ifs.close();
                            return false;
                            }
                        msg->get_properties()->set_comment(comment);
                        // now parse the comment as a lua string
                        CWrapperLua *WL = new CWrapperLua();
                        WL->SetErrorLevel(CWrapperLua::DontLog);
                        WL->DoString(comment);
                        // check if some basic variables are defined
                        // 1. cycle_time
                        int cycle_time;
                        if (WL->GetInt("cycle_time",cycle_time)) {
                            // convert cycle time from ms to sec
                            msg->get_properties()->set_cycle_time(double(cycle_time)/1000.0);
                            }
                        // at the end destroy the wrapper
                        delete WL;
                    }
                }
                else if (comment_identifier.compare("SG_") == 0) {
                    // comment to signal
                    unsigned int msg_id;
                    std::string signal_name;
                    std::string tmp,comment;
                    s >> msg_id >> signal_name;
                    std::getline(s,tmp,'"');
                    std::getline(s,comment,'"');
                    std::getline(s,tmp,';');
                    // message 3221225472 is dummy message containing unassigned signals, comment is discharged
                    if (msg_id != 3221225472) {
                        CANSignal *signal = _signalManager->get_signal(signal_name);
                        if (_signalManager->HasError()) {
                            PropagateError("ParseDBCFile", _signalManager);
                            ifs.close();
                            return false;
                            };
                        signal->get_properties()->set_comment(comment);
                        }
                    }
                }
            else if (architecture.substr(0,5).compare("VAL_ ")==0) {
                // SIGNAL TABLE
                std::stringstream s;
                s.str(architecture.substr(5,std::string::npos));
                unsigned int msg_id;
                std::string signal_name;
                unsigned int index;
                std::string descriptor;
                std::string tmp;
                // get the message and the signal's name
                s >> msg_id;
                s >> signal_name;
                while (1) {
                    // read the value table pair
                    s >> index;
                    if (s.fail())  {
                        /* stream fails when value is not numerical, this happens when the stream reaches 
                         * the ';' char at thenend of the architecture
                         */
                        break;
                        }
                    std::getline(s,tmp,'"');
                    std::getline(s,descriptor,'"');
                    _signalManager->get_signal(signal_name)->get_properties()->set_value_table_descriptor(index,descriptor);
                    }
                // a signal containing a table value is necessairly an unsigned type
                _signalManager->get_signal(signal_name)->get_properties()->set_value_type(CANSignalTypes::UNSIGNED_SIGNAL);
                }
            }
        }

    ifs.close();
    return true;
    };