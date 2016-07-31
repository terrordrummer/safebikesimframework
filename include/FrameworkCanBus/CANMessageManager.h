#pragma once

#include "FrameworkCore/CSynch.h"
#include "FrameworkCanBus/CANMessage.h"
#include "FrameworkCanBus/CANSignalManager.h"
#include "FrameworkCanBus/CANTransmitterManager.h"
#include <map>

namespace FrameworkCanBus {

// declaration needed by the compiled because of a cross-reference
class CANSignalManager;
class CANTransmitterManager;

    /** \brief CAN messages manager.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class collectes all the messages present in the CANBUS architecture. In particular, this class
     * allows to:
     * \li add a new message to the architecture
     * \li remove a message from the architecture
     * \li get a message reference by its name or its identifier
     *
     * It's worth to highlight that this class is only a container of the existing messages, no methods
     * are defined here to modify properties of messages or to set relationships between messages, 
     * ECUs and signals in the architecrture.
     */
    class framework_canbus_class CANMessageManager : public CANBasicClass {
    private:
        /** \brief Type defining the map between names and messages.        */
        typedef std::map<CANBasicTypes::type_string, CANMessage *>      _messageByNameType;

        /** \brief Type defining the map between id and messasges.          */
        typedef std::map<CANBasicTypes::type_uinteger, CANMessage *>    _messageByIdType;

        /** \brief Existing messages referenced by name.                    */
        _messageByNameType _messageByName;

        /** \brief Existing messages referenced by id.                      */
        _messageByIdType _messageById;

        /** brief Synchronization object.                                   */
        FrameworkCore::CSynch _synch;

    public:
        // --------------------------------------------------------------------------------------
        // --------------------------------------- constructors/destructors
        // --------------------------------------------------------------------------------------
        /** \brief Class constructor.           */
        CANMessageManager();
        /** \brief Class destructor.            */
        ~CANMessageManager();

        // --------------------------------------------------------------------------------------
        // --------------------------------------- getters
        // --------------------------------------------------------------------------------------
        /** \brief This method returns a pointer to the message object identified by its id.
         *  \param message_id idenfitier of the message
         *  \return message pointer on success, NULL if message does not exist.
         *
         * This method returns a reference to the message obtained from the given id. If the 
         * message does not exist yet a NULL result will be returned and an error state will be 
         * raised.
         */
        CANMessage *get_message(CANBasicTypes::type_uinteger message_id);

        /** \brief This method returns a pointer to the message object identified by its name.
         *  \param message_name name of the message
         *  \return message pointer on success, NULL if message does not exist.
         *
         * This method returns a reference to the message obtained from the given name. If the 
         * message does not exist yet a NULL result will be returned and an error state will be 
         * raised.
         */
        CANMessage *get_message(CANBasicTypes::type_string message_name);

        // --------------------------------------------------------------------------------------
        // --------------------------------------- architecture construction
        // --------------------------------------------------------------------------------------
        /** \brief Adds a message to the architecture. 
         *  \param message_properties properties of the new message
         *  \return TRUE on success
         *
         * Since it is not allowed to have duplicated message names or id inside the architecture, a check is
         * done before adding the message. If an existing message with the same name or id already exists then
         * a <b>false</b> value is returned and the message will not be added to the architeture otherwise
         * the message will be added to the architecture and a <b>true</b> value will be returned.
         */
        bool AddMessage(CANMessageProperties *message_properties);

        /** \brief Removes a message from the architecture.
         *  \param message_id identified of the message
         *  \return TRUE on success, FALSE if message can't be removed or does not exist
         *
         * If no messages with the given id exist in the architecture then a <b>false</b> value will be
         * returned and no messages will be removed from the architecture, otherwise the message 
         * will be removed from the architecture and a <b>true</b> value will be returned.
         */
        bool RemoveMessage(CANBasicTypes::type_uinteger message_id);

        /** \brief Removes a message from the architecture.
         *  \param message_name name of the message
         *  \return TRUE on success, FALSE if message can't be removed or does not exist
         *
         * If no messages with the given id exist in the architecture then a <b>false</b> value is
         * returned and no messages are removed from the architecture, otherwise the message will 
         * be removed from the architecture and a <b>true</b> valuewill be returned.
         */
        bool RemoveMessage(CANBasicTypes::type_string message_name);
        // --------------------------------------------------------------------------------------
        // --------------------------------------- aux
        // --------------------------------------------------------------------------------------
        /** \brief Check if the message exist in the architecture.
         *  \param message_name name of the message
         *  \return TRUE if message exist
         *
         * Return values:
         * \li <b>true</b> if the message exist
         * \li <b>false</b> if the message does not exist
         */
        bool MessageExist(CANBasicTypes::type_string message_name);

        /** \brief Check if the message exist in the architecture.
         *  \param message_id identified of the message
         *  \return TRUE if message exist
         *
         * Return values:
         * \li <b>true</b> if the message exist
         * \li <b>false</b> if the message does not exist
         */
        bool MessageExist(CANBasicTypes::type_uinteger message_id);

        // --------------------------------------------------------------------------------------
        // --------------------------------------- aux
        // --------------------------------------------------------------------------------------
        /** \brief Class description.
         *  \return message description string
         *
         * The returned string contains the list of description strings of all existing messages in the architecture.
         */
        virtual std::string GetDescription();
    };

    }