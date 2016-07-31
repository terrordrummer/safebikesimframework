#pragma once

#include "FrameworkCanBus/CANMessage.h"
#include "FrameworkCore/CTime.h"
#include <string>

namespace FrameworkCanBus {

    /** \brief Periodic message descriptor. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * A periodic message is a message that is transmitted on the CANBUS with a fixed transmission period. 
     * For each of these kind of messages it is possible to define two parameters:
     *
     * \li the maximum number of transmissions
     * \li the maximum life time
     *
     * When the maximum number of transmissions of a message is reached the messaged is automatically stopped. 
     * If the lifetime is elapsed then the message is automatically stopped.
     */
    class framework_canbus_class CANPeriodicMessage : public CANMessage  {
    protected:
        /** \brief Value of next time at which the message will be transmitted
         *         on the CAN bus.  
         */
        CANBasicTypes::type_time        next_transmitting_time;

        /** \brief Maximum number of transmissions for this message.                    */
        CANBasicTypes::type_uinteger    max_n_transmissions;

        /** \brief Message's maximum time life.                                         */
        FrameworkCore::CTime::time_type max_lifetime;

        /** \brief Hardware name on which the message will be sent.                     */
        CANBasicTypes::type_string      HW_device;
        
    public :
        /** \brief Class constructor.                                                   */
        CANPeriodicMessage();

        /** \brief Class destructor.                                                    */
        ~CANPeriodicMessage();
        
        /** \brief Class constructor.                                                   */
        CANPeriodicMessage( CANMessage                  *msg, 
                            CANBasicTypes::type_string   HW_name
                            );

        /** \brief Class constructor.                                                   */
        CANPeriodicMessage( CANMessage                      *msg, 
                            CANBasicTypes::type_uinteger     max_n_transmissions, 
                            FrameworkCore::CTime::time_type  max_lifetime,
                            CANBasicTypes::type_string       HW_name
                            );
        
        /** \brief Getter of maximum number of transmissions.                           */
        CANBasicTypes::type_uinteger get_max_n_transmissions() {return max_n_transmissions;};

        /** \brief Getter of maximum time life.                                         */
        FrameworkCore::CTime::time_type get_max_lifetime() {return max_lifetime;};

        /** \brief Getter of next transmitting time.                                    */
        CANBasicTypes::type_time get_next_transmitting_time() {return next_transmitting_time;};

        /** \brief Getter of hardware device's name.                                    */
        CANBasicTypes::type_string get_HW_device() {return HW_device;};

        /** \brief Setter of maximum number of transmissions.                           */
        void set_max_n_transmissions(CANBasicTypes::type_uinteger p_max_n_transmissions) {max_n_transmissions = p_max_n_transmissions;};

        /** \brief Setter of maximum number of transmissions.                           */
        void set_max_lifetime(FrameworkCore::CTime::time_type p_max_lifetime) {max_lifetime = p_max_lifetime;};

        /** \brief Setter of next transmitting time.                                    */
        void set_next_transmitting_time(CANBasicTypes::type_time p_next_transmitting_time) {next_transmitting_time = p_next_transmitting_time;};

        /** \brief Class description.
         *  \return periodic message description
         */
        virtual std::string GetDescription();

        };

    }