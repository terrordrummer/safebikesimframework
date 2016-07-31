#pragma once

#include "FrameworkCore/CSynch.h"
#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANMessageManager.h"
#include "FrameworkCanBus/CANSignal.h"
#include <map>
#include <string>

namespace FrameworkCanBus {
// declaration needed by the compiled because of a cross-reference
class CANMessageManager;

    /** \brief CAN signal manager. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     *
     * It's worth to highlight that this class is only a container of the existing signals, no methods
     * are defined here to modify properties of signals or to set relationships between signals, 
     * ECUs and messages in the architecrture.
     */
    class framework_canbus_class CANSignalManager : public CANBasicClass {
    private:
        /** \brief Map type referencing a signal by its name.                                       */
        typedef std::map<CANBasicTypes::type_string, CANSignal *> _signalByNameType;

        /** \brief Signal reference map by name.                                                    */
        _signalByNameType _signalByName;

        /** \brief Signal access synchronization object.                                            */
        FrameworkCore::CSynch _synch;

    public:
        /** \brief Class constructor.                                                               */
        CANSignalManager();

        /** \brief Class destructor.                                                                */
        ~CANSignalManager();

        /** \brief Check if the signal exist in the architecture.
         *  \param signal_name name of the signal
         *  \return signal exsisting flag
         *
         * Return values:
         * \li <b>true</b> if the signal exist
         * \li <b>false</b> if the signal does not exist
         */
        bool SignalExist(CANBasicTypes::type_string signal_name);

        /** \brief This method returns a reference to the signal object identified by its name.
         *  \param signal_name name of the signal
         *  \return signal reference
         *
         * This method returns a reference to the signal identified from the given name. If the 
         * signal does not exist yet a NULL result will be returned and an error state will be 
         * raised.
         */
        CANSignal *get_signal(CANBasicTypes::type_string signal_name);

        /** \brief Adds a signal to the architecture. 
         *  \param signal signal reference
         *
         * Since it is not allowed to have duplicated signal names inside the architecture, a check is
         * done before adding the signal. If an existing signal with the same name already exists then
         * a <b>false</b> value is returned and the signal will not be added to the architeture otherwise
         * a <b>true</b> value is returned and the signal will be added to the architecture.
         */
        bool AddSignal(CANSignalProperties *signal);

        /** \brief Removes a signal from the architecture.
         *  \param signal_name name of the signal to be deleted
         *
         * If no signals with the given name exist in the architecture then a <b>false</b> value will be
         * returned and no messages will be removed from the architecture, otherwise the message 
         * will be removed from the architecture and a <b>true</b> value will be returned.
         */
        bool RemoveMessage(CANBasicTypes::type_string signal_name);

        /** \brief signal manager's description
         *  \return signal manager description
         */
        virtual std::string GetDescription();

        };

    }