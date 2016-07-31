#pragma once

#include "FrameworkCanBus/common_defines.h"
#include "FrameworkCore/CExtendedString.h"

namespace FrameworkCanBus {

    /** \brief Basic types for CAN module.  
     *  \author Roberto Sartori, Dynamotion S.r.l. 
     * 
     * This class defines the basic types used by all other classes of CAN Manager project.
     */
    class framework_canbus_class CANBasicTypes {
    public:
        typedef double                          type_float;     /**< \brief Float numeric value type.              */
        typedef int                             type_integer;   /**< \brief Signed integer numeric value type.     */
        typedef unsigned int                    type_uinteger;  /**< \brief Unsigned integer numeric value type.   */
        typedef FrameworkCore::CExtendedString  type_string;    /**< \brief String type.                           */
        typedef double                          type_time;      /**< \brief Time value in seconds.                 */
        };


    }