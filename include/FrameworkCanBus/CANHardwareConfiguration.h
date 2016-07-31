#pragma once

#include "FrameworkCanBus/CANBasicTypes.h"

namespace FrameworkCanBus {
    /** \brief This class defines the configuration of a CANBUS hardware device.  
     *  \author Roberto Sartori, Dynamotion S.r.l.
     */
    class framework_canbus_class CANHardwareConfiguration {
    public:
        /** \brief CAN speed enumeration values.    */
        typedef enum CAN_BAUDRATE {
                BAUDRATE_1M,    /**< \brief 1M   baud   */
                BAUDRATE_500K,  /**< \brief 500k baud   */
                BAUDRATE_250K,  /**< \brief 250K baud   */
                BAUDRATE_125K,  /**< \brief 125K baud   */
                BAUDRATE_100K,  /**< \brief 100K baud   */
                BAUDRATE_50K,   /**< \brief 50K  baud   */
                BAUDRATE_20K,   /**< \brief 20K  baud   */
                BAUDRATE_10K,   /**< \brief 10K  baud   */
                BAUDRATE_5K     /**< \brief 5K   baud   */
            };
        /** \brief CAN speed.   */
        CAN_BAUDRATE baudrate;

        /** \brief Loopback flag.
         *
         * When this flag is set to TRUE the hardware will push a copy of each transmitted message in the
         * receiving queue.
         */
        bool loopback;

        /** \brief Default constructor.     */
        CANHardwareConfiguration();

        /** \brief Constructor from parameters.
         *  \param baudrate baudrate code of the device
         *  \param loopback TRUE if loopback is enabled on this device
         */
        CANHardwareConfiguration(CAN_BAUDRATE baudrate, bool loopback);

        /** \brief Constructor from parameters.
         *  \param baudrate baudrate string of the device
         *  \param loopback TRUE if loopback is enabled on this device
         */
        CANHardwareConfiguration(CANBasicTypes::type_string baudrate, bool loopback);

        /** \brief Convert the baud rate into a string description. */
        static CAN_BAUDRATE baudrate_from_string(CANBasicTypes::type_string);

        /** \brief Convert a string description of the baudrate into the baudrate value */
        static CANBasicTypes::type_string baudrate_to_string(CAN_BAUDRATE);

        };

    }