#pragma once

#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANSignalTypes.h"
#include <string>

namespace FrameworkCanBus {
    /** \brief This class defines the set of signal properties. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * According to the CANAlyzer dbc file format, the following properties exist for any signal. \n
     * \li _name
     * \li _length
     * \li _byteOrder
     * \li _unit
     * \li _valueType
     * \li _factor
     * \li _offset
     * \li _minval
     * \li _maxval
     * \li _comment
     * \li is_table
     * \li _valueTable
     *
     */
    class framework_canbus_class CANSignalProperties : public CANBasicClass {
    private:
        CANBasicTypes::type_string              _name;              /**< \brief Signal _name.                     */
        CANBasicTypes::type_integer             _length;            /**< \brief Signal _length (in bit).          */
        CANSignalTypes::CANSignalByteOrder      _byteOrder;         /**< \brief Signal's byte order.             */
        CANBasicTypes::type_string              _unit;              /**< \brief Signal's _unit of measurement.    */
        CANSignalTypes::CANSignalValueType      _valueType;         /**< \brief Signal's value type.             */
        CANBasicTypes::type_float               _factor;            /**< \brief Signal's scale _factor.           */
        CANBasicTypes::type_float               _offset;            /**< \brief Signal's _offset.                 */
        CANBasicTypes::type_float               _minval;            /**< \brief Signal's minimum value.          */
        CANBasicTypes::type_float               _maxval;            /**< \brief Signal's maximum value.          */
        CANBasicTypes::type_string              _comment;           /**< \brief Signal's _comment.                */
        bool                                    _isTable;           /**< \brief TRUE if value is a table.        */
        CANSignalTypes::value_table_type        _valueTable;        /**< \brief Signal's value table _name.       */

    public:

        /** \brief Class constructor.       */
        CANSignalProperties(    CANBasicTypes::type_string              _name        = "",
                                CANBasicTypes::type_integer             _length      = 0,
                                CANSignalTypes::CANSignalByteOrder      _byteOrder  = CANSignalTypes::INTEL_BYTEORDER,
                                CANBasicTypes::type_string              _unit        = "",
                                CANSignalTypes::CANSignalValueType      _valueType  = CANSignalTypes::SIGNED_SIGNAL,
                                CANBasicTypes::type_float               _factor      = 1.0,
                                CANBasicTypes::type_float               _offset      = 0.0,
                                CANBasicTypes::type_float               _minval      = 0.0,
                                CANBasicTypes::type_float               _maxval      = 1.0);
        
        /** \brief Getter of _name propertiy.                                                    */
        CANBasicTypes::type_string get_name() {return _name;};
        /** \brief Getter of _length propertiy.                                                  */
        CANBasicTypes::type_integer get_length() {return _length;};
        /** \brief Getter of _byteOrder propertiy.                                              */
        CANSignalTypes::CANSignalByteOrder get_byte_order() {return _byteOrder;};
        /** \brief Getter of _unit propertiy.                                                    */
        CANBasicTypes::type_string get_unit() {return _unit;};
        /** \brief Getter of _valueType propertiy.                                              */
        CANSignalTypes::CANSignalValueType get_value_type() {return _valueType;};
        /** \brief Getter of _factor propertiy.                                                  */
        CANBasicTypes::type_float get_factor() {return _factor;};
        /** \brief Getter of _offset propertiy.                                                  */
        CANBasicTypes::type_float get_offset() {return _offset;};
        /** \brief Getter of _minval propertiy.                                                  */
        CANBasicTypes::type_float get_minval() {return _minval;};
        /** \brief Getter of _maxval propertiy.                                                  */
        CANBasicTypes::type_float get_maxval() {return _maxval;};
        /** \brief Getter of _comment propertiy.                                                 */
        CANBasicTypes::type_string get_comment() {return _comment;};
        /** \brief Getter of _valueTable propertiy.                                             */
        CANSignalTypes::value_table_type get_value_table() {return _valueTable;};
        /** \brief Getter of the value descriptor                                               */
        CANBasicTypes::type_string get_value_table_descriptor(unsigned int _index) ;

        /** \brief Setter of _name property.                                                     */
        bool set_name(CANBasicTypes::type_string);
        /** \brief Setter of _length property.                                                   */
        bool set_length(CANBasicTypes::type_integer);
        /** \brief Setter of _byteOrder property.                                               */
        bool set_byte_order(CANSignalTypes::CANSignalByteOrder);
        /** \brief Setter of _unit property.                                                     */
        bool set_unit(CANBasicTypes::type_string);
        /** \brief Setter of _valueType property.                                               */
        bool set_value_type(CANSignalTypes::CANSignalValueType);
        /** \brief Setter of _factor property.                                                   */
        bool set_factor(CANBasicTypes::type_float);
        /** \brief Setter of _offset property.                                                   */
        bool set_offset(CANBasicTypes::type_float);
        /** \brief Setter of _minval property.                                                   */
        bool set_minval(CANBasicTypes::type_float);
        /** \brief Setter of _maxval property.                                                   */
        bool set_maxval(CANBasicTypes::type_float);
        /** \brief Setter of _comment property.                                                  */
        bool set_comment(CANBasicTypes::type_string);
        /** \brief Setter of _valueTable property.                                              */
        bool set_value_table(CANSignalTypes::value_table_type table);
        
        /** \brief Seter of the _valueTable descriptor at given position.                       */
        bool set_value_table_descriptor(unsigned int _index, CANBasicTypes::type_string descriptor);

        /** \brief Setter of all parameters.                                                    */
        bool set_properties(CANSignalProperties *prop);
        
        /** \brief Class description.
         *  \return signal properties description
         */
        virtual std::string GetDescription();
        };


    }