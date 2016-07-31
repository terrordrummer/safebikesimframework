#pragma once

#include "CANBasicTypes.h"
#include <vector>
#include <set>

namespace FrameworkCanBus {
    /** \brief Signal-specific types.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     * 
     * This class contains all the non-basic types needed to define some signal's properties.
     */
    class framework_canbus_class CANSignalTypes {
    public:
        /** \brief Table value type.                                 */
        typedef std::vector<CANBasicTypes::type_string> value_table_type;

        /** \brief Signal byte order specifier.                      */
        typedef enum {
            INTEL_BYTEORDER,       /**< \brief Intel byte order     */
            MOTOROLA_BYTEORDER     /**< \brief Motorola byte order  */
            } CANSignalByteOrder;

        /** \brief Signal's value type specifier.                    */
        typedef enum {
            SIGNED_SIGNAL,         /**< \brief signed value         */
            UNSIGNED_SIGNAL,       /**< \brief unsigned value       */
            IEEE_FLOAT_SIGNAL,     /**< \brief ieee float           */
            IEEE_DOUBLE_SIGNAL     /**< \brief ieee double          */
            } CANSignalValueType;

        /** \brief Set type containing the list of messages which are packing a signal.  */
        typedef std::set<CANBasicTypes::type_string> message_set_type;

        };

    }
