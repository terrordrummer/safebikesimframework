#include "FrameworkCore/CGenericType.h"
#include <sstream>
#include <cctype>
#include <math.h>
#include <iostream>

using namespace FrameworkCore;
using namespace std;

///////////////////////////////////////////////////////////////////////
CGenericType::CGenericType() {
    // type numeric by default
    _valueType = t_numeric;
    // default values
    _numericValue = 0;
    _stringValue.assign("");
    _structValue.clear();
    _arrayValue.clear();
    }

///////////////////////////////////////////////////////////////////////
CGenericType::~CGenericType() {
    clear_data();
    }
///////////////////////////////////////////////////////////////////////
/*
 ____
|  _ \ _   _ _ __ ___  _ __
| | | | | | | '_ ` _ \| '_ \
| |_| | |_| | | | | | | |_) |
|____/ \__,_|_| |_| |_| .__/
                      |_|
*/
string CGenericType::Dump() {
    // local vars;
    structType::iterator field;
    // string stream for output
    stringstream ss;
    ss.str("");
    // produce the string description
    switch (_valueType) {
        case t_numeric: // numeric representation: '10'
            ss.precision(12);
            ss << "'" << _numericValue << "'";
            break;
        case t_string:  // string representation: "hello world"
            ss << "\"" << _stringValue << "\"";
            break;
        case t_struct: // struct representation: {"name1",<type dump>;"name2",<type dump>;}
            ss << "{";
            field = _structValue.begin();
            while(field != _structValue.end()) {
                ss << "\"" << field->first << "\"," << field->second.Dump() << ";";
                field++;
                }
            ss << "}";
            break;
        case t_array:   // array representation: ['1','0','0.4','6']
            ss << "[";
            for (unsigned int i=0; i<(unsigned int)_arrayValue.size(); i++) {
                ss << _arrayValue[i].Dump();
                if (i != (unsigned int)_arrayValue.size()-1)
                    ss << ";";
                }
            ss << "]";
            break;
        }
    // output the description
    return ss.str();
    }

/*
 ____                          _           _   _           _
|  _ \ ___  ___ _   _ _ __ ___(_)_   _____| | | |_ __   __| |_   _ _ __ ___  _ __
| |_) / _ \/ __| | | | '__/ __| \ \ / / _ \ | | | '_ \ / _` | | | | '_ ` _ \| '_ \
|  _ <  __/ (__| |_| | |  \__ \ |\ V /  __/ |_| | | | | (_| | |_| | | | | | | |_) |
|_| \_\___|\___|\__,_|_|  |___/_| \_/ \___|\___/|_| |_|\__,_|\__,_|_| |_| |_| .__/
                                                                            |_|
*/
/**
 *  \param txt string describing the object
 *  \param obj object that has to be created
 *
 * This method is used to parse a string and instantiate the generic type described by the string.
 * The approach is recursive and this method implements the recursive alorithm that undumps the string.
 */
void CGenericType::RecursiveUndump(char **txt, CGenericType &obj) {
    // exit condition
    CGenericType    x;
    stringstream    aux_str;
    int i;
    // null string will exit
    if (**txt == 0) {
        obj = "null string dump";
        return;
        }
    // read the beginning character
    char c = **txt;
    switch (c) {
        case '\'':
            // skip first '\'' char
            (*txt)++;
            // object is a number, extract the number string
            aux_str.str("");
            while (**txt != '\'') {
                char c = **txt;
                aux_str << c;
                (*txt)++;
                }
            // convert string to CGenericType numeric
            x = aux_str.str();
            obj = x.toNumber();
            // move to the next char
            (*txt)++;
            break;
        case '\"':
            // skip first '\"' char
            (*txt)++;
            // object is a string, extract the string
            aux_str.str("");
            while (**txt != '\"') {
                char c = **txt;
                aux_str << c;
                (*txt)++;
                }
            // convert string to CGenericType numeric
            obj = aux_str.str();
            // move to the next char
            (*txt)++;
            break;
        case '{':
            // object is a struct, get name
            // skip first \" char
            (*txt)++;
            while (**txt != '}') {
                (*txt)++;
                aux_str.str("");
                while (**txt != '\"') {
                    char c = **txt;
                    aux_str << c;
                    (*txt)++;
                    }
                // skip '\"' char
                (*txt)++;
                // skip ',' char
                (*txt)++;
                RecursiveUndump(txt, obj%(aux_str.str()));
                // skip final ';' char
                (*txt)++;
                }
            // skip '}' char
            (*txt)++;
            break;
        case '[':
             // skip first '[' char
            (*txt)++;
            // object is a vector, get elements
            i=0;
            while (**txt != ']') {
                RecursiveUndump(txt, obj[i++]);
                if (**txt != ']')
                    (*txt)++;
                }
            (*txt)++;
            break;
        default:
            // ERROR, return zero value
            obj = "Error undumping object";
            break;
        }

    }
/*
 _   _           _
| | | |_ __   __| |_   _ _ __ ___  _ __
| | | | '_ \ / _` | | | | '_ ` _ \| '_ \
| |_| | | | | (_| | |_| | | | | | | |_) |
 \___/|_| |_|\__,_|\__,_|_| |_| |_| .__/
                                  |_|
*/
/**
 *  \param dump_text string describing the object
 *
 * This method fills the object data as described by the dump text.
 */
void CGenericType::Undump(string &dump_text) {
    // do the work using a recursive procedure
    char *txt;
    txt = (char *)dump_text.c_str();
    RecursiveUndump(&txt, *this);
    }

///////////////////////////////////////////////////////////////////////
bool CGenericType::operator == (CGenericType &obj_compare) {
        structType::iterator it_struct;
        arrayType::iterator  it_array;
        // different types are never equal
        if (_valueType != obj_compare.get_type_id()) return false;
        // compare depends on type
        switch(_valueType) {
            case t_numeric: // numeric value has to be the same
                return _numericValue == obj_compare.toNumber();
            case t_string:  // string must be equal
                return _stringValue.compare(obj_compare.toString()) == 0;
            case t_struct: // all fields has to be the same
                // number of fields must be the same
                if (size() != obj_compare.size()) return false;
                it_struct = _structValue.begin();
                while (it_struct != _structValue.end()) {
                    // search the same field inside the comparison obj
                    string *name = (string *)&(it_struct->first);
                    if (!(it_struct->second == (obj_compare % *name)))
                        return false;
                    it_struct++;
                    }
                return true;
            case t_array:   // size and elements have to be equal
                if (size() != obj_compare.size()) return false;
                // compare all elements
                it_array = _arrayValue.begin();
                while(it_array != _arrayValue.end()) {
                    // look for the existence of an object with the same index
                    if (!(it_array->second == obj_compare[it_array->first])) return false;
                    it_array++;
                    }
                return true;
            case(t_pointer):
                // pointer address must be equal
                return _pointerValue == obj_compare.toPointer();
            }
        return false;
        }


/*
 ____                          _           ____           _   _
|  _ \ ___  ___ _   _ _ __ ___(_)_   _____|  _ \ _ __ ___| |_| |_ _   _
| |_) / _ \/ __| | | | '__/ __| \ \ / / _ \ |_) | '__/ _ \ __| __| | | |
|  _ <  __/ (__| |_| | |  \__ \ |\ V /  __/  __/| | |  __/ |_| |_| |_| |
|_| \_\___|\___|\__,_|_|  |___/_| \_/ \___|_|   |_|  \___|\__|\__|\__, |
                                                                  |___/
*/
string CGenericType::RecursivePretty(CGenericType &obj, int nestingLevel) {
    // string returned
    stringstream retval;
    vector<string> fields;

    retval.str("");
    fields.clear();
    // switch on type
    switch(obj.get_type_id()) {
    case t_numeric:
        retval << obj.toNumber();
        break;

    case t_string:
        retval << "'" << obj.toString() << "'";
        break;

    case t_struct:
        retval << '{' << endl;
        fields = obj.GetFields();
        for (vector<string>::iterator it = fields.begin(); it != fields.end(); it++) {
            // add indention
            for (int j=0; j<nestingLevel; j++)
                retval << "   ";
            // write field name
            retval << *it << " ";
            retval << RecursivePretty(obj % *it, nestingLevel+1) << endl;
        }
        for (int j=0; j<nestingLevel; j++)
            retval << "   ";
        retval << '}';
        break;

    case t_array:
        retval << '[' << endl;
        fields = obj.GetFields();
        for (vector<string>::iterator it = fields.begin(); it != fields.end(); it++) {
            // add indention
            for (int j=0; j<nestingLevel; j++)
                retval << "   ";
            // write field name
            retval << *it << " ";
            retval << RecursivePretty(obj % *it, nestingLevel+1) << endl;
        }
        for (int j=0; j<nestingLevel; j++)
            retval << "   ";
        retval << ']';
        break;

    default:
        retval << "---";
    }
    return retval.str();
}


/*
 ____           _   _
|  _ \ _ __ ___| |_| |_ _   _
| |_) | '__/ _ \ __| __| | | |
|  __/| | |  __/ |_| |_| |_| |
|_|   |_|  \___|\__|\__|\__, |
                        |___/
*/
string CGenericType::Pretty() {
    return RecursivePretty(*this, 0);
}