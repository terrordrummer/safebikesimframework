#pragma once

#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANMessageManager.h"
#include "FrameworkCanBus/CANSignalManager.h"
#include "FrameworkCanBus/CANHardwareManager.h"
#include "FrameworkCanBus/CANCallbackManager.h"
#include "FrameworkCanBus/CANEcuManager.h"
#include <set>

namespace FrameworkCanBus {
    
    /** \brief Receiver manager class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class manages the receiving of messages from an assigned hardware device. The received
     * signals are unpacked and values are updated automatically. Receiving callbacks, if installedm are 
     * invoked automatically.
     */

    class framework_canbus_class CANReceiverManager : public CANBasicClass {
    private:
        /** \brief Container type of running threads.                   */
        typedef std::set<CANBasicTypes::type_string> _runningThreadsType;

        /** \brief Container type of message's black list.              */
        typedef std::set<CANBasicTypes::type_uinteger> _messageBlacklistType;

        /** \brief Container type of signal's black list.              */
        typedef std::set<CANBasicTypes::type_string> _signalBlacklistType;

        /** \brief Running threads list.                                */
        _runningThreadsType _runningThreads;

        /** \brief Ecu manager object pointer.                          */
        CANEcuManager *_ecuManager;

        /** \brief Message manager object pointer.                      */
        CANMessageManager *_messageManager;

        /** \brief Hardware manager object pointer.                     */
        CANHardwareManager *_hardwareManager;

        /** \brief Callnack manager object pointer.                     */
        CANCallbackManager *_callbackManager;

        /** \brief Signal_manager object pointer.                       */
        CANSignalManager *_signalManager;
        /** \brief This flag remind if a received signal comes from a
         *         loopback received message.
         */
        bool _signalIsFromLoopbackMessage;

        /** \brief Black list of messages excluded from the receiving.  */
        _messageBlacklistType _messageBlacklist;

        /** \brief Black list of signals excluded from the receiving.   */
        _signalBlacklistType _signalBlacklist;

        /** \brief Receiving flag.                                      */
        bool _discardAll;

        /** \brief Receiving message thread procedure.                  */
        virtual void ThreadedMethod(std::string thread_name, void *params);

        /** \brief Unpack message content and update signal values.     */
        void UnpackMessage(CANMessage *);
    public:
        /** \brief Default constructor.                                 */
        CANReceiverManager();

        /** \brief Default destructor.                                  */
        ~CANReceiverManager();

        /** \brief Set the managers.
          *
          * The message receiver module needs to access to some information about 
          * message structure, signals properties, etc. Managers are setted by
          * this method; managers must have been already created and architecture
          * setup has to be completed before starting the receiving from an hardware
          * device.
          */
        void SetManagers(   CANEcuManager       *em, 
                            CANMessageManager   *mm, 
                            CANSignalManager    *sm, 
                            CANHardwareManager  *hm,
                            CANCallbackManager  *cm);

        // ---------------------------------------------------------------
        //                        ENABLE/DISABLE FEATURES
        // ---------------------------------------------------------------
        /** \brief Start receiving messages from the given device.
          *
          * This method activates the receiving of messages from the given device according
          * to the current receiving filters. The device is defined by its name.
          */
        bool StartReceiving(CANBasicTypes::type_string HW_device);

        /** \brief Start receiving messages from the given device.
          *
          * This method activates the receiving of messages from the given device according
          * to the current receiving filters. The device is defined by its ID.
          */
        bool StartReceiving(unsigned int deviceID);

        /** \brief Stop receiving messages from the given device.
          *
          * This method stops the receiving of messages form the given device. The
          * device is defined by its name.
          */
        bool StopReceiving(CANBasicTypes::type_string HW_device);

        /** \brief Stop receiving messages from the given device.
          *
          * This method stops the receiving of messages form the given device. The
          * device is defined by its ID.
          */
        bool StopReceiving(unsigned int deviceID);

        // ---------------------------------------------------------------
        //                        ENABLE/DISABLE FEATURES
        // ---------------------------------------------------------------
        
        /** \brief Enable the receiving of the set of messages received by an ECU.  */
        bool ReceivingFilter_EnableECUMessages(CANBasicTypes::type_string ECU_name);

        /** \brief Disable the receiving of the set of messages received by an ECU. */
        bool ReceivingFilter_DisableECUMessages(CANBasicTypes::type_string ECU_name);
        
        /** \brief Enable the receiving of a specific CAN message.      */
        bool ReceivingFilter_EnableMessage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Disable the receiving of a specific message.         */
        bool ReceivingFilter_DisableMessage(CANBasicTypes::type_uinteger msg_id);

        /** \brief Enable the receiving of a specific signal.           */
        bool ReceivingFilter_EnableSignal(CANBasicTypes::type_string signal_name);

        /** \brief Disable the receiving of a specific signal.          */
        bool ReceivingFilter_DisableSignal(CANBasicTypes::type_string signal_name);

        /** \brief Enables all signals and messages for receiving.      */
        bool ReceivingFilter_EnableAll();

        /** \brief Disables all signals and messages from receiving.    */
        bool ReceivingFilter_DisableAll();

        };

    }