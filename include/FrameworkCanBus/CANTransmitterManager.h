#pragma once

#include "FrameworkCore/CSynch.h"
#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANPeriodicMessage.h"
#include "FrameworkCanBus/CANMessageStatistics.h"
#include "FrameworkCanBus/CANMessageManager.h"
#include "FrameworkCanBus/CANSignalManager.h"
#include "FrameworkCanBus/CANHardwareManager.h"
#include "FrameworkCanBus/CANCallbackManager.h"
#include <list>
#include <map>
#include <string>

namespace FrameworkCanBus {
class CANMessageManager;
class CANSignalManager;

    /** \brief This class manages the message transmission on the can.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class manages the transmitted messages which are messages that are written on the CANBUS.
     * Two classes of messages are considered:
     * \li <b>periodic messages</b>: these messages are cyclic and are periodically trantmitted to the CANBUS
     * \li <b>one-shot messages</b>: these messages are trantmitted on the CANBUS only once
     *
     * The periodic trantmission of a message is managed by this class in order to avoid the user to write code
     * to manage messages timing. Once a periodic message is started it will update the message with the current
     * values of the signals that are packed into the message: the user has only to update the signal's values
     * and the message transmitter will do the remaining work. \n
     * The management of one-shot messages is much simpler: the user will explicitly call the one-shot method 
     * to write immediatly the message on the CANBUS with the current values of the signal's packed into the
     * message.
     *
     * This class manages also the statistical information about all messages. This statistical data is:
     * \li average trantmission time
     * \li lifetime of the message
     * \li next time at which the message will be written
     * \li number of times the message has been written
     *
     */
    class framework_canbus_class CANTransmitterManager : public CANBasicClass {
    public:
        /** \brief Active message's statistics values type.                 */
        typedef std::map<CANBasicTypes::type_uinteger, CANMessageStatistics> statistics_map_type;
    
    private:
        /** \brief Type of container of active messages.                    */
        typedef std::list<CANPeriodicMessage *> _periodicMessageListType;

        /** \brief Time clock for an active message map by id.              */
        typedef std::map<CANBasicTypes::type_uinteger, FrameworkCore::CTime> _periodicMessageTimeclockType;

        /** \brief Transmit message list.                                   */
        _periodicMessageListType _transmissionList;

        /** \brief Message statistcs map by message id.                     */
        statistics_map_type _messageStatistics;

        /** \brief Time clock for an active message.                        */
        _periodicMessageTimeclockType _periodicMessageTimeclock;

        /** \brief Message manager object pointer.                          */
        CANMessageManager *_messageManager;

        /** \brief Hardware manager object pointer.                         */
        CANHardwareManager *_hardwareManager;

        /** \brief Signal manager object pointer.                           */
        CANSignalManager *_signalManager;

        /** \brief Callback manager object pointer.                         */
        CANCallbackManager  *_callbackManager;

        /** \brief Synchronization object.                                  */
        CSynch _synch;
        
        /** \brief Transmission message thread procedure.                   */
        virtual void ThreadedMethod(std::string thread_name, void *params);
        
        /** \brief This method packs all the signal's values and write the message to the CAN Hardware.  */
        void TransmitMessage(CANMessage *periodic_message, CANBasicTypes::type_string HW_device);

        /** \brief This method packs all the signal's values and write the message to the CAN Hardware.  */
        void TransmitMessage(CANPeriodicMessage *periodic_message, CANBasicTypes::type_string HW_device);

        /** \brief This method inserts the message into the active list messages mantaining the ordering on
         *         the next_time_to_trantmit value.  
         */
        void Insert(CANPeriodicMessage *message);

        /** \brief This method packs the signal's value into the message data.                           */
        void PackMessage(CANMessage *message);

        /** \brief Resets the statistics of a message.                                      */
        void ResetMessageStatistics(CANBasicTypes::type_uinteger message_id);
    
        /** \brief Resets all the statistics.                                               */
        void ResetAllMessageStatistics();

    public:
        // -----------------------------------------------------------------------------------
        // ------------------------------ defaut constructors-destructors
        // -----------------------------------------------------------------------------------

        /** \brief Class constructor.                                                       */
        CANTransmitterManager();

        /** \brief Class constructor.                                                       */
        ~CANTransmitterManager();

        // -----------------------------------------------------------------------------------
        // ------------------------------  aux
        // -----------------------------------------------------------------------------------

        /** \brief Initialized of message manager and signal manager.                       */
        void SetManagers(   CANMessageManager   *mm, 
                            CANSignalManager    *sm, 
                            CANHardwareManager  *hm,
                            CANCallbackManager  *cm);

        // -----------------------------------------------------------------------------------
        // ------------------------------ START/STOP MESSAGES
        // -----------------------------------------------------------------------------------

        /** \brief Activate a message.                                                      */
        bool StartMessage(  CANBasicTypes::type_uinteger    msg_id,     //< Message's ID.
                            CANBasicTypes::type_string      HW_device   //< Hardware device's name.
                            );

        /** \brief Activate a message.                                                      */
        bool StartMessage(  CANBasicTypes::type_string      msg_name,   //< Message's ID.
                            CANBasicTypes::type_string      HW_device   //< Hardware device's name.
                            );

        /** \brief Starts a message limiting its lifetime or its number of publications.    */
        bool StartMessage(  CANBasicTypes::type_uinteger    msg_id,             //< Message's ID.
                            double                          max_lifetime,       //< Expiring lifetime. Set 0 for no time expiration.
                            long                            max_transmissions,  //< Maximum number of sending times. Set 0 for no limits.
                            CANBasicTypes::type_string      HW_device           //< Hardware device's name.
                            );

        /** \brief Starts a message limiting its lifetime or its number of publications.    */
        bool StartMessage(  CANBasicTypes::type_string      msg_name,           //< Message's name.
                            double                          max_lifetime,       //< Expiring lifetime. Set 0 for no time expiration.
                            long                            max_transmissions,  //< Maximum number of sending times. Set 0 for no limits.
                            CANBasicTypes::type_string      HW_device           //< Hardware device's name.
                            );

        /** \brief Transmit the message once.                                               */
        bool OneShotMessage(    CANBasicTypes::type_uinteger msg_id,    //< Message's ID.
                                CANBasicTypes::type_string HW_device    //< Hardware device's name.
                                );

        /** \brief Transmit the message once.                                               */
        bool OneShotMessage(    CANBasicTypes::type_string msg_name,    //< Message's name.
                                CANBasicTypes::type_string HW_device    //< Hardware device's name.
                                );

        /** \brief Deactivate a message.                                                    */
        bool StopMessage(       CANBasicTypes::type_uinteger msg_id     //< Message's ID.
            );

        /** \brief Deactivate a message.                                                    */
        bool StopMessage(       CANBasicTypes::type_string msg_name     //< Message's name.
            );

        // -----------------------------------------------------------------------------------
        // ------------------------------ STATISTICS
        // -----------------------------------------------------------------------------------
        
        /** \brief Get the statistics of a message                                          */
        bool GetStatistics(CANBasicTypes::type_uinteger msg_id, CANMessageStatistics *stat);

        /** \brief Get the statistics of a message                                          */
        bool GetStatistics(CANBasicTypes::type_string msg_name, CANMessageStatistics *stat);

        /** \brief Get the whole statistics data                                            */
        statistics_map_type GetStatistics() {return _messageStatistics;};

        // -----------------------------------------------------------------------------------
        // ------------------------------ AUX
        // -----------------------------------------------------------------------------------
        /** \brief Class descriptor.
         *  \return message transmitter description
         */
        virtual std::string GetDescription();

        };
     
    }