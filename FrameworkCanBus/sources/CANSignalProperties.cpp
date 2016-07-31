#include "FrameworkCanBus/CANSignalProperties.h"
#include <sstream>

using namespace FrameworkCanBus;
using namespace std;



/*
  ____    _    _   _ ____  _                   _ ____                            _   _
 / ___|  / \  | \ | / ___|(_) __ _ _ __   __ _| |  _ \ _ __ ___  _ __   ___ _ __| |_(_) ___ ___
| |     / _ \ |  \| \___ \| |/ _` | '_ \ / _` | | |_) | '__/ _ \| '_ \ / _ \ '__| __| |/ _ Y __|
| |___ / ___ \| |\  |___) | | (_| | | | | (_| | |  __/| | | (_) | |_) |  __/ |  | |_| |  __|__ \
 \____/_/   \_\_| \_|____/|_|\__, |_| |_|\__,_|_|_|   |_|  \___/| .__/ \___|_|   \__|_|\___|___/
                             |___/                              |_|
*/
CANSignalProperties::CANSignalProperties(    CANBasicTypes::type_string             name,
                                             CANBasicTypes::type_integer            length,
                                             CANSignalTypes::CANSignalByteOrder     byte_order,
                                             CANBasicTypes::type_string             unit,
                                             CANSignalTypes::CANSignalValueType     value_type,
                                             CANBasicTypes::type_float              factor,
                                             CANBasicTypes::type_float              offset,
                                             CANBasicTypes::type_float              minval,
                                             CANBasicTypes::type_float              maxval) {
    // default initializations
    SetErrorClassPrefix(std::string("CANSignalProperties"));
    _name.clear();
    _unit.clear();
    _valueTable.clear();
    // assign properties
    set_name(name);
    set_length(length);
    set_byte_order(byte_order);
    set_unit(unit);
    set_value_type(value_type);
    set_factor(factor);
    set_offset(offset);
    set_minval(minval);
    set_maxval(maxval);
    _valueTable.clear();
    };

/*
            _                _               _        _     _             _                    _       _
  __ _  ___| |_  __   ____ _| |_   _  ___   | |_ __ _| |__ | | ___     __| | ___ ___  ___ _ __(_)_ __ | |_ ___  _ __
 / _` |/ _ \ __| \ \ / / _` | | | | |/ _ \  | __/ _` | '_ \| |/ _ \   / _` |/ _ Y __|/ __| '__| | '_ \| __/ _ \| '__|
| (_| |  __/ |_   \ V / (_| | | |_| |  __/  | || (_| | |_) | |  __/  | (_| |  __|__ \ (__| |  | | |_) | || (_) | |
 \__, |\___|\__|___\_/ \__,_|_|\__,_|\___|___\__\__,_|_.__/|_|\___|___\__,_|\___|___/\___|_|  |_| .__/ \__\___/|_|
 |___/        |_____|                   |_____|                  |_____|                        |_|
*/
CANBasicTypes::type_string CANSignalProperties::get_value_table_descriptor(unsigned int index) {
    // value must exist
    try {
        return _valueTable.at(index);
        }
    catch (std::out_of_range) {
        std::stringstream s;
        s << "index " << index << " out of rance in signal " << get_name() << " (max=" << _valueTable.size() << ")";
        PushError("get_value_table", s.str());
        CANBasicTypes::type_string g;
        g ="";
        return g;
        }
    };

/*
          _
 ___  ___| |_     _ __   __ _ _ __ ___   ___
/ __|/ _ \ __|   | '_ \ / _` | '_ ` _ \ / _ \
\__ \  __/ |_    | | | | (_| | | | | | |  __/
|___/\___|\__|___|_| |_|\__,_|_| |_| |_|\___|
            |_____|
*/
bool CANSignalProperties::set_name(CANBasicTypes::type_string name) {
    _name = name;
    return true;
    };

/*
          _       _                  _   _
 ___  ___| |_    | | ___ _ __   __ _| |_| |__
/ __|/ _ \ __|   | |/ _ \ '_ \ / _` | __| '_ \
\__ \  __/ |_    | |  __/ | | | (_| | |_| | | |
|___/\___|\__|___|_|\___|_| |_|\__, |\__|_| |_|
            |_____|            |___/
*/
bool CANSignalProperties::set_length(CANBasicTypes::type_integer length) {
    if (length > 64) {
        std::stringstream s;
        s << "wrong length value of " << length << " in signal <" << get_name() << ">";
        PushError("set_length", s.str());
        _length = 0;
        return false;
        }
    _length = length;
    return true;
    };

/*
          _       _           _                        _
 ___  ___| |_    | |__  _   _| |_ ___     ___  _ __ __| | ___ _ __
/ __|/ _ \ __|   | '_ \| | | | __/ _ \   / _ \| '__/ _` |/ _ \ '__|
\__ \  __/ |_    | |_) | |_| | ||  __/  | (_) | | | (_| |  __/ |
|___/\___|\__|___|_.__/ \__, |\__\___|___\___/|_|  \__,_|\___|_|
            |_____|     |___/       |_____|
*/
bool CANSignalProperties::set_byte_order(CANSignalTypes::CANSignalByteOrder byte_order) {
    if (byte_order != CANSignalTypes::MOTOROLA_BYTEORDER &&
        byte_order != CANSignalTypes::INTEL_BYTEORDER) {
            std::stringstream s;
            s << "wrong byte_order assignment in signal <" << get_name() << ">";
            PushError("set_byte_order", s.str());
            return false;
        }
    _byteOrder = byte_order;
    return true;
    };

/*
          _                  _ _
 ___  ___| |_    _   _ _ __ (_) |_
/ __|/ _ \ __|  | | | | '_ \| | __|
\__ \  __/ |_   | |_| | | | | | |_
|___/\___|\__|___\__,_|_| |_|_|\__|
            |_____|
*/
bool CANSignalProperties::set_unit(CANBasicTypes::type_string unit) {
    _unit = unit;
    return true;
    };

/*
          _                _               _
 ___  ___| |_  __   ____ _| |_   _  ___   | |_ _   _ _ __   ___
/ __|/ _ \ __| \ \ / / _` | | | | |/ _ \  | __| | | | '_ \ / _ \
\__ \  __/ |_   \ V / (_| | | |_| |  __/  | |_| |_| | |_) |  __/
|___/\___|\__|___\_/ \__,_|_|\__,_|\___|___\__|\__, | .__/ \___|
            |_____|                   |_____|  |___/|_|
*/
bool CANSignalProperties::set_value_type(CANSignalTypes::CANSignalValueType value_type) {
    if (value_type != CANSignalTypes::SIGNED_SIGNAL &&
        value_type != CANSignalTypes::UNSIGNED_SIGNAL &&
        value_type != CANSignalTypes::IEEE_FLOAT_SIGNAL &&
        value_type != CANSignalTypes::IEEE_DOUBLE_SIGNAL) {
            std::stringstream s;
            s << "wrong signal_type assignment in signal <" << get_name() << ">";
            PushError("set_value_type", s.str());
            return false;
        }
    _valueType = value_type;
    return true;
    };

/*
          _        __            _
 ___  ___| |_     / _| __ _  ___| |_ ___  _ __
/ __|/ _ \ __|   | |_ / _` |/ __| __/ _ \| '__|
\__ \  __/ |_    |  _| (_| | (__| || (_) | |
|___/\___|\__|___|_|  \__,_|\___|\__\___/|_|
            |_____|
*/
bool CANSignalProperties::set_factor(CANBasicTypes::type_float factor) {
    _factor = factor;
    return true;
    };

/*
          _             __  __          _
 ___  ___| |_     ___  / _|/ _|___  ___| |_
/ __|/ _ \ __|   / _ \| |_| |_/ __|/ _ \ __|
\__ \  __/ |_   | (_) |  _|  _\__ \  __/ |_
|___/\___|\__|___\___/|_| |_| |___/\___|\__|
            |_____|
*/
bool CANSignalProperties::set_offset(CANBasicTypes::type_float offset) {
    _offset = offset;
    return true;
    };

/*
          _                 _                 _
 ___  ___| |_     _ __ ___ (_)_ ____   ____ _| |
/ __|/ _ \ __|   | '_ ` _ \| | '_ \ \ / / _` | |
\__ \  __/ |_    | | | | | | | | | \ V / (_| | |
|___/\___|\__|___|_| |_| |_|_|_| |_|\_/ \__,_|_|
            |_____|
*/
bool CANSignalProperties::set_minval(CANBasicTypes::type_float minval) {
    _minval = minval;
    return true;
    };

/*
          _                                       _
 ___  ___| |_     _ __ ___   __ ___  ___   ____ _| |
/ __|/ _ \ __|   | '_ ` _ \ / _` \ \/ | \ / / _` | |
\__ \  __/ |_    | | | | | | (_| |>  < \ V / (_| | |
|___/\___|\__|___|_| |_| |_|\__,_/_/\_\ \_/ \__,_|_|
            |_____|
*/
bool CANSignalProperties::set_maxval(CANBasicTypes::type_float maxval) {
    _maxval = maxval;
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
bool CANSignalProperties::set_comment(CANBasicTypes::type_string comment) {
    _comment = comment;
    return true;
    };

/*
          _                _               _        _     _
 ___  ___| |_  __   ____ _| |_   _  ___   | |_ __ _| |__ | | ___
/ __|/ _ \ __| \ \ / / _` | | | | |/ _ \  | __/ _` | '_ \| |/ _ \
\__ \  __/ |_   \ V / (_| | | |_| |  __/  | || (_| | |_) | |  __/
|___/\___|\__|___\_/ \__,_|_|\__,_|\___|___\__\__,_|_.__/|_|\___|
            |_____|                   |_____|
*/
bool CANSignalProperties::set_value_table(CANSignalTypes::value_table_type value_table) {
    if (_valueTable.size() > 0) {
        *this << "[set_value_table] new value table will overwrite the existing one";
        };
    _valueTable = value_table;
    return true;
    };

/*
          _                _               _        _     _             _                    _       _
 ___  ___| |_  __   ____ _| |_   _  ___   | |_ __ _| |__ | | ___     __| | ___ ___  ___ _ __(_)_ __ | |_ ___  _ __
/ __|/ _ \ __| \ \ / / _` | | | | |/ _ \  | __/ _` | '_ \| |/ _ \   / _` |/ _ Y __|/ __| '__| | '_ \| __/ _ \| '__|
\__ \  __/ |_   \ V / (_| | | |_| |  __/  | || (_| | |_) | |  __/  | (_| |  __|__ \ (__| |  | | |_) | || (_) | |
|___/\___|\__|___\_/ \__,_|_|\__,_|\___|___\__\__,_|_.__/|_|\___|___\__,_|\___|___/\___|_|  |_| .__/ \__\___/|_|
            |_____|                   |_____|                  |_____|                        |_|
*/
bool CANSignalProperties::set_value_table_descriptor(unsigned int index, CANBasicTypes::type_string descriptor) {
    try {
        if (_valueTable.at(index).length()>0) {
            // value already assigned, overwrite it but signal the warning
            *this << "[set_value_table] overwrite table value \"" << _valueTable.at(index) 
                  << "\" at index " << index << " with new value \"" << descriptor 
                  << "\" in signal <" << get_name() << ">\n";
            }
        } catch(...){};
    if (index > _valueTable.size()-1 || _valueTable.empty())
        _valueTable.resize(index+1);
    _valueTable[index] = descriptor;
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
bool CANSignalProperties::set_properties(CANSignalProperties *prop) {
    set_name(prop->get_name());
    set_length(prop->get_length());
    set_byte_order(prop->get_byte_order());
    set_unit(prop->get_unit());
    set_value_type(prop->get_value_type());
    set_factor(prop->get_factor());
    set_offset(prop->get_offset());
    set_minval(prop->get_minval());
    set_maxval(prop->get_maxval());
    set_value_table(prop->get_value_table());
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
std::string CANSignalProperties::GetDescription() {
    std::stringstream s;
    // write message properties
    s << "Name               : " << get_name() << endl;
    s << "Lenght             : " << dec << get_length();
    if (get_length() == 1 )
        s << " bit" << endl;
    else
        s << " bits" << endl;
    s << "Byte order         : ";
    if (get_byte_order() == CANSignalTypes::INTEL_BYTEORDER) 
        s << "INTEL" << endl;
    else
        s << "MOTOROLA" << std::endl;
    s << "Unit               : " << get_unit() << endl;
    s << "Value type         : ";
    switch(get_value_type()) {
        case CANSignalTypes::SIGNED_SIGNAL:
            s << "SIGNED" << endl;
            break;
        case CANSignalTypes::UNSIGNED_SIGNAL:
            s << "UNSIGNED" << endl;
            break;
        case CANSignalTypes::IEEE_FLOAT_SIGNAL:
            s << "IEEE FLOAT" << endl;
            break;
        case CANSignalTypes::IEEE_DOUBLE_SIGNAL:
            s << "IEEE DOUBLE" << endl;
            break;
        };
    s << "Offset             : " << get_offset() << endl;
    s << "Minimum val        : " << get_minval() << endl;
    s << "Maximum val        : " << get_maxval() << endl;
    // value table values (in dbc format)
    s << "Value table name   : ";
    for (unsigned int i=0; i < _valueTable.size(); i++) {
        if (_valueTable[i].length()>0)
            s << i << " \"" << _valueTable[i] << "\"  ";
        }
    s << endl;
    s << "Comment            : \"" << get_comment() << "\"";
    return s.str();
    }
