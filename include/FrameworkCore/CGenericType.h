#pragma once;

// standard includes
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <math.h>
#include <cctype>

// Framework includes
#include "FrameworkCore/common_defines.h"
#include "FrameworkCore/CExtendedString.h"

//#define USE_ASSERTS
#ifdef USE_ASSERTS
#include <assert.h>
#endif

namespace FrameworkCore {

/** \brief Class for dynamic binding of variable type.
 *  \author Roberto Sartori, Dynamotion S.r.l.
 *
 * This class defines a dynamically binded variable whose content type is defined at
 * runtime. \n
 * By using this class it will be possible to create dynamic structures and heterogenous arrays at runtime with
 * no limits in the number of nested levels.
 *
 * To create a numeric object just assign a numeric result:
 * \code
 *  CGenericType g;
 *  g = 0.1;
 * \endcode
 *
 * To create a string object just assign a char pointer value or a string:
 * \code
 *  CGenericType g;
 *  g = "myString";
 * \endcode
 *
 * To create a struct use the '%' operator defining the fields as strings:
 * \code
 *  CGenericType g;
 *  g % "field1" = 0.1;
 *  g % "field2" = "myString";
 *  g % "root" % "node1" = 1;   // substructs are allowed
 * \endcode
 *
 * To construct an array use the square bracked operator:
 * \code
 *  CGenericType g;
 *  g[0] = 0.1;
 *  g[1] = 1;
 * \endcode
 *
 * Each time a generic type is assigned to a type which is different from the actual the data
 * content is deleted and the new data is assigned:
 *
 * \code
 *  CGenericType g;
 *  g = 0.1;            // g is a number
 *  g = "myString";     // now g is a string;
 *  g % "field1" = 0.1; // now g is a struct type
 * \endcode
 *
 * Operations between comptible types are allowed:
 * \code
 *  // MATH, +, -, *, / only if both are numeric
 *  CGenericType a,b,c;
 *  a = 0.1;
 *  b = 1;
 *  c = a+b;    // c is numberic = 1.1
 *
 *  // STRING, only if both are string
 *  CGenericType str1, str2, str3;
 *  str1 = "My name is ";
 *  str2 = "Roberto";
 *  str3 = str1 + str2; // str3 is "My name is Roberto"
 *
 * \endcode
 * according to the operator rules.
 *
 */
class framework_core_class CGenericType {
public:

    /** \typedef Identifies the available type of the data.                 */
    typedef enum  {
        t_numeric = 0,          /**< \brief Numeric type identifier.        */
        t_string  = 1,          /**< \brief String  type identifier.        */
        t_struct  = 2,          /**< \brief Struct  type identifier.        */
        t_array   = 3,          /**< \brief Array   type identifier.        */
        t_pointer = 4           /**< \brief Pointer type identifier.        */
        } type_id_enum;

    typedef long double                         numericType;   /**< \brief Numeric data container type.     */
    typedef CExtendedString                     stringType;    /**< \brief String data container type.      */
    typedef std::map<std::string, CGenericType> structType;    /**< \brief Struct data container type.      */
    typedef std::map<unsigned int,CGenericType> arrayType;     /**< \brief Array data container type.       */
    typedef void*                               pointerType;   /**< \brief Struct data container type.      */

private:
    type_id_enum    _valueType;         /**< \brief Dynamic value type of the actual data instance.         */

    numericType    _numericValue;       /**< \brief Numeric data.                                           */
    std::string    _stringValue;        /**< \brief String data.                                            */
    structType     _structValue;        /**< \brief Struct data.                                            */
    arrayType      _arrayValue;         /**< \brief Array data.                                             */
    pointerType    _pointerValue;       /**< \brief Pionter data.                                           */

private:
    /** \brief Undump a string into a generic type.
     *
     * This method is internal and impements a recursive scan of the string describing the objet content.
     * While scanning the object data is filled.
     */
    void RecursiveUndump(char **txt, CGenericType &obj);

    /** \brief Recursive pretty string generation.
     *
     * This method recursively scan the object content and generate a pretty string describing the object 
     * content.
     */
    std::string RecursivePretty(CGenericType &obj, int nestingLevel);

public:
    CGenericType();                     /**< \brief Default constructor.                                     */
    virtual ~CGenericType();            /**< \brief Default destructor.                                      */

    std::string Dump();                 /**< \brief Dump the object content into a string format.            */
    void        Undump(std::string &);  /**< \brief Undump the object from its string description.           */
    __inline void Undump(char *s) {     /**< \brief Undump the object from its string description.           */
        std::string S(s);
        Undump(S);
    }


    std::string Pretty();               /**< \brief Produce an easy-to-read description of the data content. */

    // ------------------------------------------------
    //              CLASS CASTING
    // ------------------------------------------------
    /** \brief explicit type casting.
     *
     *  \return bool value.
     */
    __inline bool toBool() {
        return (bool)(_numericValue != 0);
        }   
    /** \brief explicit type casting.       
     *
     *  \return char value.
     */
    __inline char toChar() {
        return (char)_numericValue;
        }   
    /** \brief explicit type casting.       
     *
     *  \return unsigned char value.
     */
    __inline unsigned char toUnsignedChar() {
        return (unsigned char)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return short value.
     */
    __inline short toShort() {
        return (short)_numericValue;
        }   
    /** \brief explicit type casting.       
     *
     *  \return unsigned short value.
     */
    __inline unsigned short toUnsignedShort() {
        return (unsigned short)_numericValue;
        }    
    /** \brief explicit type casting.       
     *
     *  \return int value.
     */
    __inline int toInt() {
        return (int)_numericValue;
        }    
    /** \brief explicit type casting.       
     *
     *  \return unsigned int value.
     */
    __inline unsigned int toUnsignedInt() {
        return (unsigned int)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return long value.
     */
    __inline long toLong() {
        return (long)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return unsigned long value.
     */
    __inline unsigned long toUnsignedLong() {
        return (unsigned long)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return long long value.
     */
    __inline long long toLongLong() {
        return (long long)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return unsigned long long value.
     */
    __inline unsigned long long toUnsignedLongLong() {
        return (unsigned long long)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return float value.
     */
    __inline float toFloat() {
        return (float)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return double value.
     */
    __inline double toDouble() {
        return (double)_numericValue;
        }
    /** \brief explicit type casting.       
     *
     *  \return long double value.
     */
    __inline long double toLongDouble() {
        return _numericValue;
        }

    /** \brief explicit type casting.       
     *
     *  \return void * value.
     */
    __inline void *toPointer() {
        return (void *)_pointerValue;
        }
    // ------------------------------------------------
    //              explicit type conversion
    // ------------------------------------------------
    /** \brief Convert content to numerical value.
     *
     *  \return numeric type value.
     *
     *
     * Conversion to number is allowed only if content type is numeric or a string containing a convertible value.
     */
    __inline numericType toNumber() {
        switch(_valueType) {
            case(t_numeric):
                return _numericValue;
            case(t_string):
                return atof(_stringValue.c_str());
            case(t_struct):
                return 0.0;
            case(t_array):
                return 0.0;
            case(t_pointer):
#pragma warning(disable: 4311)
                return 0.0;
#pragma warning(default: 4311)
            }
        return 0;
        }

    /** \brief Convert content to string.
     *
     * \return String data.
     *
     * String conversion is allowed only if data type is a numerical value or a string value otherwise
     * the returned string contains a escriptor of the kind of data stored into the variable.
     *
     */
    __inline std::string toString() {
        std::stringstream ss;
        ss.clear();
        std::string result;
        switch(_valueType) {
            case(t_numeric):
                ss << _numericValue;
                result.assign(ss.str());
                break;
            case(t_string):
                result = _stringValue;
                break;
            case(t_struct):
                result = "[struct]";
                break;
            case(t_array):
                result = "[array]";
                break;
            case (t_pointer):
                result = "[pointer]";
                break;
            }
        return result;
        }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    /** \brief Struct inspection.
     *
     * This method returns the list of field names present in the structure. If the type is not a structure then
     * an empty list is returned.
     *
     * \return Field name list.
     */
    std::vector<std::string> GetFields() {
        std::vector<std::string> res;
        res.clear();
        if (_valueType == t_struct) {
            structType::iterator it = _structValue.begin();
            while (it != _structValue.end()) {
                res.push_back(it->first);
                it++;
            }
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    /** \brief Returns the identifier of the object's type.
     *
     *  \return Enumerative identifier code.
     */
    __inline type_id_enum get_type_id() { return _valueType; }

    /** \brief Returns the description of the current object's type.
     *
     *  \return String type description.
     */
    __inline std::string get_type_description() {
        switch(_valueType) {
            case(t_numeric):
                return "numeric";
            case(t_string):
                return "string";
            case(t_struct):
                return "struct";
            case(t_array):
                return "array";
            case(t_pointer):
                return "pointer";
            }
        return "undefined";
        }

    /** \brief Retrive the pointer to the data container.
     *
     *  \return Data pointer.
     *
     * This method is used if it is necessary to use the data container from the external of this class.
     * <b>The use of this method is not recommended. </b>
     */
    __inline void *get_value_ptr() {
        switch(_valueType) {
            case(t_numeric):
                return (void *)&_numericValue;
            case(t_string):
                return (void *)&_stringValue;
            case(t_struct):
                return (void *)&_structValue;
            case(t_array):
                return (void *)&_arrayValue;
            case(t_pointer):
                return (void *)&_pointerValue;
            }
        return NULL;
        }

    /** \brief Retrive the size of the data.
     *
     *  \return Size of the data.
     *
     *  The following value is returned depending on the runtime type:
     *  \li numeric: size is 1
     *  \li string: string length
     *  \li struct: number of the first-level fields
     *  \li array: size of the array
     */
    __inline unsigned int size() {
        switch(_valueType) {
            case t_numeric:
                return 1;
            case t_string:
                return (unsigned int)_stringValue.size();
            case t_struct:
                return (unsigned int)_structValue.size();
            case t_array:
                return (unsigned int)_arrayValue.size();
            case t_pointer:
                return 1;
            }
        return 0;
        }

    /** \brief Check if the given field exist.
     *
     *  \param field_name field name to check
     *  \return \b TRUE if the field exist, \b FALSE otherwise
     *
     * If the field is a struct type then checks if the given field is part of the
     * actual field set.
     */
    __inline bool hasField(std::string field_name) {
        // type must be a struct
        if (_valueType != t_struct)
            return false;
        else
            // return true if field is present
            return (_structValue.find(field_name) != _structValue.end());
        }

    /** \brief Remove the given field if present.
     *
     *  \param field_name field name to check
     *  \return \b TRUE if the field has been removed, \b FALSE otherwise
     *
     * If the field is a struct type then checks if the given field is part of the
     * actual field set and in this case removes it.
     */
    __inline bool RemoveField(std::string field_name) {
        // type must be a struct
        if (_valueType != t_struct)
            return false;
        else
            // return true if field is present
            return( _structValue.erase(field_name) > 0);
        }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    /** \brief Clear data content.                              */
    __inline void ClearData() {clear_data();}

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    /** \brief Sets the data type as numeric.                   */
    __inline void SetNumeric() {
        clear_data();
        _valueType = t_numeric;
        }
    /** \brief Sets the data type as string.                    */
    __inline void SetString() {
        clear_data();
        _valueType = t_string;
        }
    /** \brief Sets the data type as struct.                    */
    __inline void SetStruct() {
        clear_data();
        _valueType = t_struct;
        }
    /** \brief Sets the data type as array.                     */
    __inline void SetArray() {
        clear_data();
        _valueType = t_array;
        }
    /** \brief Sets the data type as pointer.                   */
    __inline void SetPointer() {
        clear_data();
        _valueType = t_pointer;
        }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    /** \brief CGenericType assignment.
     *
     *  \param assign_obj generic type to assign
     */
    __inline void operator = (const CGenericType& assign_obj) {
        // clear obj data if is actually different
        if ( ((CGenericType&)assign_obj).get_type_id() != _valueType  ||
             ((CGenericType&)assign_obj).get_type_id() == t_struct    ||
             ((CGenericType&)assign_obj).get_type_id() == t_array         )
             clear_data();

        // assign new data
        switch(((CGenericType&)assign_obj).get_type_id()) {
            case(t_numeric):
                _numericValue = ((CGenericType&)assign_obj).toNumber();
                _valueType = t_numeric;
                break;
            case(t_string):
                _stringValue = ((CGenericType&)assign_obj).toString();
                _valueType = t_string;
                break;
            case(t_struct):
                assign_struct((structType *)((CGenericType&)assign_obj).get_value_ptr());
                _valueType = t_struct;
                break;
            case(t_array):
                assign_array((arrayType *)((CGenericType&)assign_obj).get_value_ptr());
                _valueType = t_array;
                break;
            case(t_pointer):
                _pointerValue = (pointerType)((CGenericType&)assign_obj).toPointer();
                _valueType = t_pointer;
                break;
            }
        }

    /** \brief bool assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (bool value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief char assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (char value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief unsigned char assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (unsigned char value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        } 
    /** \brief short assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (short value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief unsigned short assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (unsigned short value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief int assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (int value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief unsigned int assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (unsigned int value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief long assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (long value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief unsigned long assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (unsigned long value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief long long assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (long long value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = (numericType)value;
        }
    /** \brief unsigned long long assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (unsigned long long value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = (numericType)value;
        }
    /** \brief float assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (float value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief double assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (double value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief long double assignment.
     *
     *  \param value value to assign
     */
    __inline void operator = (long double value) {
        if (_valueType != t_numeric) {
            clear_data();
            _valueType = t_numeric;
            }
        _numericValue = value;
        }
    /** \brief string assignment.
     *
     *  \param str string to assign
     */
    __inline void operator = (std::string& str) {
        if (_valueType != t_string) {
            clear_data();
            _valueType = t_string;
            }
        _stringValue = str;
        }
    /** \brief string assignment.
     *
     *  \param str string to assign
     */
    __inline void operator = (const char *str) {
        if (_valueType != t_string) {
            clear_data();
            _valueType = t_string;
            }
        _stringValue = std::string(str);
        }
    /** \brief pointer assignment.
     *
     *  \param pointer pointer to assign
     */
    __inline void operator = (void *pointer) {
        if (_valueType != t_pointer) {
            clear_data();
            _valueType = t_pointer;
            }
        _pointerValue = (pointerType)pointer;
        }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR + sum between two CGenericType objects. <b>Valid only for numerical</b>.
     *
     *  \param sumobj numeric object to sum
     *  \return sum result as a new object
     */
    __inline CGenericType operator + (CGenericType& sum_obj) {
        CGenericType c;
        // aritmetic is a correct operation only for numeric values
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);  
#endif
        if (_valueType == t_numeric && sum_obj.get_type_id() == t_numeric)
            c = _numericValue + sum_obj.toNumber();
        else if (_valueType == t_string && sum_obj.get_type_id() == t_string)
            c = _stringValue + sum_obj.toString();
        return c;
        }

    /** \brief OPERATOR - subtraction between two CGenericType objects. <b>Valid only for numerical</b>.
     *
     *  \param sub_obj numeric object to subtract
     *  \return subtraction result as a new object
     */
    __inline CGenericType operator - (CGenericType& sub_obj) {
        CGenericType c;
        // valid for numerical values only
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
        assert(sub_obj.get_type_id() == t_numeric);
#endif
        // sum nueric 
        c = _numericValue - sub_obj.toNumber();
        return c;
        }

    /** \brief OPERATOR * multiplication between two CGenericType objects. <b>Valid only for numerical</b>.
     *
     *  \param mult_obj numeric object to multiplicate
     *  \return multiplication result as a new object
     */
    __inline CGenericType operator * (CGenericType& mult_obj) {
        CGenericType c;
        // valid for numerical values only
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
        assert(mult_obj.get_type_id() == t_numeric);
#endif
        // sum nueric 
        c = _numericValue * mult_obj.toNumber();
        return c;
        }

    /** \brief OPERATOR / numerical division between two CGenericType objects. <b>Valid only for numerical</b>.
     *
     *  \param div_obj numeric object to divide by
     *  \return division result as a new object
     */
    __inline CGenericType operator / (CGenericType& div_obj) {
        CGenericType c;
        // valid for numerical values only
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
        assert(div_obj.get_type_id() == t_numeric);
#endif
        // sum nueric 
        c = _numericValue / div_obj.toNumber();
        return c;
        }

    
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    /** \brief UNARY OPERATOR + 
     *
     *  \return result depends by the runtime type.
     *
     * Depending on the current type:
     * \li t_numeric: rounds the numerical value to the closest integer
     * \li t_string: uppercases a string
     * 
     * Do nothing in all other cases.
     */
    __inline CGenericType &operator + () {
        switch (_valueType) {
            case t_numeric:
                _numericValue = floor(_numericValue) + floor(_numericValue+0.5);
                break;
            case t_string:
                for (unsigned int i=0; i<_stringValue.length(); i++)
                    _stringValue[i] = std::toupper(_stringValue[i]);
                break;
            case t_struct:
                break;
            case t_array:
                break;
            }
        return *this;
        }

    /** \brief UNARY OPERATOR - 
     *
     *  \return result depends by the runtime type.
     *
     * Depending on the current type:
     * \li t_numeric: floors the numerical value
     * \li t_string: downcases a string
     * 
     * Do nothing in all other cases.
     */
    __inline CGenericType &operator - () {
        switch (_valueType) {
            case t_numeric:
                _numericValue = floor(_numericValue);
                break;
            case t_string:
                for (unsigned int i=0; i<_stringValue.length(); i++)
                    _stringValue[i] = std::tolower(_stringValue[i]);
                break;
            case t_struct:
                break;
            case t_array:
                break;
            }
        return *this;
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR ++ : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief left hand side ++: increment a number by 1 before using it.
     *
     *  \result generic type whose numerical value has been incremented by 1.
     */
    __inline CGenericType &operator ++ () {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += 1.0;
        return *this;
        }

    /** \brief right hand side ++: increment a number by 1 after using it.  
     *
     *  \result generic type value before being incremented by 1.
     */
    __inline CGenericType operator ++ (int value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        CGenericType c;
        c = _numericValue;
        _numericValue += 1.0;
        return c;
        }

    /** \brief left hand side -- decrement a number by 1 before using it.
     *
     *  \result generic type whose numerical value has been decremented by 1.
     */
    __inline CGenericType &operator -- () {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue -= 1.0;
        return *this;
        }

    /** \brief right hand side -- decrement a number by 1 after using it.
     *
     *  \result generic type value before being decremented by 1.
     */
    __inline CGenericType operator -- (int value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        CGenericType c;
        c = _numericValue;
        _numericValue -= 1.0;
        return c;
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR += : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (CGenericType& add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        if (((CGenericType&)add_value).get_type_id() != _valueType)
            *this = add_value;
        else {
            // assign new data
            switch(((CGenericType&)add_value).get_type_id()) {
                case(t_numeric):
                    _numericValue += ((CGenericType&)add_value).toNumber();
                    break;
                case(t_string):
                    _stringValue = _stringValue + ((CGenericType&)add_value).toString();
                    break;
                case(t_struct):
                    assign_struct((structType *)((CGenericType&)add_value).get_value_ptr());
                    break;
                case(t_array):
                    assign_array((arrayType *)((CGenericType&)add_value).get_value_ptr());
                    break;
                case(t_pointer):
                    _pointerValue = (pointerType)((CGenericType&)add_value).toPointer();
                    break;
                }
        }
        
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value char value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (char add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value unsigned char value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (unsigned char add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }  

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value short value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (short add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value unsigned short value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (unsigned short add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value int value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (int add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value unsigned value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (unsigned int add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value long value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (long add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value unsigned long value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (unsigned long add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value long long value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (long long add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value unsigned long long value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (unsigned long long add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value float value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (float add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value double value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (double add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

    /** \brief OPERATOR += : adds a number to the actual numerical value.
     *
     *  \param add_value long double value to be added.
     *  \return CGenericType instance updated by this operation.
     */
    __inline CGenericType &operator += (long double add_value) {
#ifdef USE_ASSERTS 
        assert(_valueType == t_numeric);
#endif
        _numericValue += (numericType)add_value;
        return *this;
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR == : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR == : comparison between objects.                    */
    __inline bool operator == (CGenericType& obj_compare);

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (char value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (unsigned char value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (short value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (unsigned short value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (int value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (unsigned int value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (long value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (unsigned long value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (long long value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (unsigned long long value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (float value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (double value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between numeric.
     *
     *  \param value value to be compared.
     *  \return \b TRUE if numeric values are equal, \b FALSE otherwise.
     */
    __inline bool operator == (long double value) {
#ifdef USE_ASSERT
        assert(_valueType == t_numeric);
#else
        if (_valueType != t_numeric) return false;
#endif
        return _numericValue == (numericType)value;
        }

    /** \brief OPERATOR == : comparison between strings.
     *
     *  \param str string to be compared.
     *  \return \b TRUE if string contents are equal, \b FALSE otherwise.
     */
    __inline bool operator == (const char *str) {
#ifdef USE_ASSERT
        assert(_valueType == t_string);
#else
        if (_valueType != t_string) return false;
#endif
        return _stringValue.compare(std::string(str)) == 0;
        }

    /** \brief OPERATOR == : comparison between strings.
     *
     *  \param str string to be compared.
     *  \return \b TRUE if string contents are equal, \b FALSE otherwise.
     */
    __inline bool operator == (std::string &str) {
#ifdef USE_ASSERT
        assert(_valueType == t_string);
#else
        if (_valueType != t_string) return false;
#endif
        return _stringValue.compare(str) == 0;
        }

    /** \brief OPERATOR == : comparison between pointers.
     *
     *  \param pointer pointer address to be compared.
     *  \return \b TRUE if pointer addresses are equal, \b FALSE otherwise.
     */
    __inline bool operator == (void *pointer) {
#ifdef USE_ASSERT
        assert(_valueType == t_pointer);
#else
        if (_valueType != t_pointer) return false;
#endif
        return _pointerValue == (pointerType)pointer;
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR != : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR != : comparison between generic types.
     *
     *  \param obj_compare object to be compared
     *  \return \b TRUE if contents are not the same, \b FALSE otherwise
     */
__inline bool operator != (CGenericType& obj_compare) {
        return !(*this == obj_compare);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (char value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (unsigned char value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (short value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (unsigned short value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (int value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (unsigned int value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (long value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (unsigned long value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (long long value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (unsigned long long value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (float value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (double value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between numeric.
     *
     *  \param value numeric value to be compared
     *  \return \b TRUE if values are not the same, \b FALSE otherwise
     */
    __inline bool operator != (long double value) {
        return !(*this == value);
        }

    /** \brief OPERATOR != : comparison between strings.
     *
     *  \param str string value to be compared
     *  \return \b TRUE if strings are different, \b FALSE otherwise
     */
    __inline bool operator != (const char *str) {
        return !(*this == str);
        }

    /** \brief OPERATOR != : comparison between strings.
     *
     *  \param str string value to be compared
     *  \return \b TRUE if strings are different, \b FALSE otherwise
     */
    __inline bool operator != (std::string &str) {
        return !(*this == str);
        }

    /** \brief OPERATOR != : comparison between pointers.
     *
     *  \param pointer pointer address to be compared
     *  \return \b TRUE if addresses are different, \b FALSE otherwise
     */
    __inline bool operator != (void *pointer) {
        return !(*this == pointer);
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR > : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR > : greather than.
     *
     *  \param obj_compare object to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object or the parameter type is not numeric than the result is undefined.
     */
__inline bool operator > (CGenericType& obj_compare) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > obj_compare.toNumber();
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (unsigned char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (unsigned short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (unsigned int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (unsigned long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (unsigned long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (float value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

    /** \brief OPERATOR > : greather than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator > (long double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue > (numericType)value;
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR < : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR < : less than.
     *
     *  \param obj_compare object to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object or the parameter type is not numeric than the result is undefined.
     */
__inline bool operator < (CGenericType& obj_compare) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < obj_compare.toNumber();
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (unsigned char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (unsigned short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (unsigned int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (unsigned long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (unsigned long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (float value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    /** \brief OPERATOR < : less than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator < (long double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue < (numericType)value;
        }

    
///////////////////////////////////////////////////////////////////////
// OPERATOR >= : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR < : great equal than.
     *
     *  \param obj_compare object to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object or the parameter type is not numeric than the result is undefined.
     */
__inline bool operator >= (CGenericType& obj_compare) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= obj_compare.toNumber();
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (unsigned char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (unsigned short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (unsigned int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (unsigned long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (unsigned long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (float value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator >= (long double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue >= (numericType)value;
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR <= : valid only for numeric types
///////////////////////////////////////////////////////////////////////
    /** \brief OPERATOR < : less equal than.
     *
     *  \param obj_compare object to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object or the parameter type is not numeric than the result is undefined.
     */
__inline bool operator <= (CGenericType& obj_compare) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= obj_compare.toNumber();
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (unsigned char value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (unsigned short value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (unsigned int value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (unsigned long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (unsigned long long value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (float value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }

    /** \brief OPERATOR < : great equal than.
     *
     *  \param value value to be compared
     *  \return \b TRUE if current object is greater, \b FALSE otherwise
     *
     * If type of the object type is not numeric than the result is undefined.
     */
    __inline bool operator <= (long double value) {
#ifdef USE_ASSERT
    assert(_valueType == t_numeric);
#endif
        return _numericValue <= (numericType)value;
        }


///////////////////////////////////////////////////////////////////////
// OPERATOR [] for array type
///////////////////////////////////////////////////////////////////////
    
    /** \brief OPERATOR []: array indexing
     *
     *  \param indx index of the element
     *  \return element at \c index position
     *
     * If index is not yet assigned then the necessary elements will be initialized.
     */
    __inline CGenericType &operator [](unsigned int indx) {

        if (_valueType != t_array) {
            clear_data();
            _valueType = t_array;
            }

        arrayType::iterator it = _arrayValue.find(indx);
        if (it == _arrayValue.end()) {
            // create as many _instances as needed to fit the array until the actual index
            for (unsigned int i = (unsigned int)_arrayValue.size(); i<=indx; i++)
                _arrayValue[i] = 0;
            }

        return _arrayValue[indx];
        }
///////////////////////////////////////////////////////////////////////
// OPERATOR % dynamic struct field reference, convert variable into
//            a struct and creates dynamically the field
///////////////////////////////////////////////////////////////////////
    /** \brief Field reference operator.
     *
     *  \param field_name string field name to reference
     *  \return object referenced by the field
     */
    __inline CGenericType &operator % (std::string& field_name) {
        // verify if field is struct
        if (_valueType != t_struct) {
            clear_data();
            _valueType = t_struct;
            }
        // look for the field_name
        structType::iterator struct_it = _structValue.find(field_name);
        if (struct_it != _structValue.end()) {
            // field founded, check the type of requested field
            return struct_it->second;
            }
        else {
            // field does not exist, create a pointer to it and a new CGenericType containing the data
            _structValue[field_name] = 0;
            return _structValue[field_name];
            }
        }

    /** \brief Field reference operator.
     *
     *  \param field_name character field name to reference
     *  \return object referenced by the field
     */
    __inline CGenericType &operator % (const char *field_name) {
        // verify if field is struct
        if (_valueType != t_struct) {
            clear_data();
            _valueType = t_struct;
            }
        // look for the field_name
        structType::iterator struct_it = _structValue.find(field_name);
        if (struct_it != _structValue.end()) {
            // field founded, check the type of requested field
            return struct_it->second;
            }
        else {
            // field does not exist, create a pointer to it and a new CGenericType containing the data
            _structValue[field_name] = 0;
            return _structValue[field_name];
            }
        }
    
    /** \brief Field reference operator.
     *
     *  \param field_name field name to reference as generic type supposed to be casted into string
     *  \return object referenced by the field
     */
    __inline CGenericType &operator % (CGenericType& field_name) {
        // verify if field is struct
        if (_valueType != t_struct) {
            clear_data();
            _valueType = t_struct;
            }
        // look for the field_name
        structType::iterator struct_it = _structValue.find(field_name.toString());
        if (struct_it != _structValue.end()) {
            // field founded, check the type of requested field
            return struct_it->second;
            }
        else {
            // field does not exist, create a pointer to it and a new CGenericType containing the data
            _structValue[field_name.toString()] = 0;
            return _structValue[field_name.toString()];
            }
        }

///////////////////////////////////////////////////////////////////////
// OPERATOR << writes object content into the output stream
///////////////////////////////////////////////////////////////////////
    /** \brief Generic type stream operation.
     *
     *  \param out output stream
     *  \param instance generic type instance to put into the stream
     *  \return stream updated
     *
     * By default a generic type flushes into the stream its dump string.
     */
    friend std::ostream& operator<<( std::ostream& out, CGenericType &instance ) {
        out << instance.Dump();
        return out;
        }


private:
    /** \brief Clear data of a generic type.                */
    __inline void clear_data() {
        // clear numerical and string data
        _numericValue = 0;
        _stringValue.assign("");
        // clear struct data
        if (_structValue.size() > 0) _structValue.clear();
        // clear array data
        if (_arrayValue.size() > 0) _arrayValue.clear();
        }

    /** \brief Assign a struct to a generic type.
     *
     *  \param assign_obj struct to be asigned
     */
    __inline void assign_struct(structType *assign_obj) {
        structType::iterator struct_it = (*assign_obj).begin();
        while (struct_it != (*assign_obj).end()) {
            _structValue[struct_it->first] = struct_it->second;
            struct_it++;
            }
        }

    /** \brief Assign an array to a generic type.
     *
     *  \param assign_obj array to be asigned
     */
    __inline void assign_array(arrayType *assign_obj) {
        arrayType::iterator array_it = (*assign_obj).begin();
        while (array_it != (*assign_obj).end()) {
            _arrayValue[array_it->first] = (*assign_obj)[array_it->first];
            array_it++;
            }
        }
    };

}
