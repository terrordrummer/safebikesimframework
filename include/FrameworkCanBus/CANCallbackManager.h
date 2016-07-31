#pragma once

#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANMessage.h"
#include "FrameworkCanBus/CANSignal.h"

namespace FrameworkCanBus {

    /** \brief CAN Callback manager.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements the methods used to install/uninstall callbacks associated to messages 
     * and signals events.
     */
    class framework_canbus_class CANCallbackManager : public CANBasicClass {
    public:
        /** \brief Options for the message transmission callback.                       */
        typedef enum {
            MSG_BEFORE_PACKING,                         /**< \brief Callback is invoked before packing signals into the message.                               */
            MSG_AFTER_PACKING_BUT_BEFORE_TRANSMISSION,  /**< \brief Callback is invoked after all the signals are packed and just before the message is sent.  */ 
            MSG_AFTER_TRANSMISSION,                     /**< \brief Callback is invoked immediatly after the message was sent.                                 */
            } MessageTransmissionCallbackOPTIONS;

        /** \brief Options for the message receiving callback.                          */
        typedef enum {
            MSG_BEFORE_UNPACKING,   /**< \brief Callback is invoked wieh the message is received and before signals are unpacked.      */
            MSG_AFTER_UNPACKING     /**< \brief Callback is invoked after the signals are unpacked and updated.                        */
            } MessageReceivingCallbackOPTIONS;

        /** \brief Options for a signal callback.                                       */
        typedef enum {
            SIGNAL_BEFORE_PACKING, /**< \brief Callback is invoked before packing the signal.  */
            SIGNAL_AFTER_PACKING   /**< \brief Callback is invoked after packing the signal.   */
            } SignalTransmittingCallbackOPTIONS;

        /** \brief Options for a signal callback.                                       */
        typedef enum {
            SIGNAL_BEFORE_UNPACKING, /**< \brief Callback is invoked before unpacking the signal.  */
            SIGNAL_AFTER_UNPACKING   /**< \brief Callback is invoked after unpacking the signal.   */
            } SignalReceivingCallbackOPTIONS;

        /** \brief Callback type definition.                                            */
        typedef void (*CALLBACK_TYPE)(void *param);

    private:
        /** \brief Type defining the container of installed callbacks associated 
         *         to transmissint/receiving messages.                                  */
        typedef std::map<CANBasicTypes::type_uinteger, CALLBACK_TYPE> _msgCallbackMapType;

        /** \brief Type defining the container of installed callbacks associated
         *         to transmitting/receiving signals.                                   */
        typedef std::map<CANBasicTypes::type_string, CALLBACK_TYPE> _signalCallbackMapType;

        /** \brief Map of callbacks for the transmission of a message invoked before 
         *         packing signals.                                                     */
        _msgCallbackMapType _msgTransmissionCallbacksBeforePacking;

        /** \brief Map of callbacks for the transmission of a message invoked after 
         *         packing signals.                                                     */
        _msgCallbackMapType _msgTransmissionCallbacksAfterPacking;

        /** \brief Map of callbacks for the transmission of a message invoked after 
         *         the message is sent.                                                 */
        _msgCallbackMapType _msgTransmissionCallbacksAfterTransmission;

        /** \brief Map of callbacks associated to the receiving of a message and
         *         invoked before messages are packed.                                  */
        _msgCallbackMapType _msgReceivingCallbacksBeforeUnpacking;

        /** \brief Map of callbacks associated to the receiving of a message and 
         *         invoked after signals have been unpacked.                            */
        _msgCallbackMapType _msgReceivingCallbacksAfterUnpacking;

        /** \brief Map of callbacks invoked before the packing of a signal.             */
        _signalCallbackMapType _signalBeforePackingCallbacks;

        /** \brief Map of callbacks invoked after the packing of a signal.              */
        _signalCallbackMapType _signalAfterPackingCallbacks;

        /** \brief Map of callbacks invoked before the unpacking of a signal.           */
        _signalCallbackMapType _signalBeforeUnpackingCallbacks;
    
        /** \brief Map of callbacks invoked after the unpacking of a signal.            */
        _signalCallbackMapType _signalAfterUnpackingCallbacks;
    
    public:

        /** \brief Defaul constructor.      */
        CANCallbackManager();

        // ----------------------------------------------------------
        //                     CALLBACK INSTALL 
        // ----------------------------------------------------------
        /** \brief Install a callback associated to the transmission of a message.
         *  \param msg_id identifier of the message
         *  \param cbk callback pointer
         *  \param opt callback transmission option
         *  \return TRUE if success
         */
        bool InstallMessageTransmissionCallback(    CANBasicTypes::type_uinteger        msg_id, 
                                                    CALLBACK_TYPE                       cbk,
                                                    MessageTransmissionCallbackOPTIONS  opt);

        /** \brief Install a callback associated to the reception of a message.
         *  \param msg_id identifier of the message
         *  \param cbk callback pointer
         *  \param opt callback receiving option
         *  \return TRUE if success
         */
        bool InstallMessageReceivingCallback(       CANBasicTypes::type_uinteger        msg_id, 
                                                    CALLBACK_TYPE                       cbk,
                                                    MessageReceivingCallbackOPTIONS     opt);

        /** \brief Install a callback associated to the transmission of a signal.
         *  \param signal_name name of the signal
         *  \param cbk callback pointer
         *  \param opt callback transmission option
         *  \return TRUE if success
         */
        bool InstallSignalTransmissionCallback(     CANBasicTypes::type_string          signal_name, 
                                                    CALLBACK_TYPE                       cbk,
                                                    SignalTransmittingCallbackOPTIONS   opt);

        /** \brief Install a callback associated to the reception of a signal.
         *  \param signal_name name of the signal
         *  \param cbk callback pointer
         *  \param opt callback receiving option
         *  \return TRUE if success
         */
        bool InstallSignalReceivingCallback(        CANBasicTypes::type_string          signal_name, 
                                                    CALLBACK_TYPE                       cbk,
                                                    SignalReceivingCallbackOPTIONS      opt);

        // ----------------------------------------------------------
        //                  CALLBACK REMOVAL 
        // ----------------------------------------------------------
        /** \brief Removes a callback associated to the transmission of a message with the given options.
         *  \param msg_id identifier of the message
         *  \param opt callback transmission option
         *  \return TRUE if success
         */
        bool RemoveMessageTransmissionCallback( CANBasicTypes::type_uinteger        msg_id ,
                                                MessageTransmissionCallbackOPTIONS  opt);

        /** \brief Removes all the callbacks associated to the transmission of a message.
         *  \param msg_id identifier of the message
         *  \return TRUE if success
         */
        bool RemoveMessageTransmissionCallback( CANBasicTypes::type_uinteger msg_id);


        /** \brief Removes a callback associated to the reception of a message with the given option.  
         *  \param msg_id identifier of the message
         *  \param opt callback receiving option
         *  \return TRUE if success
         */
        bool RemoveMessageReceivingCallback(    CANBasicTypes::type_uinteger        msg_id,
                                                MessageReceivingCallbackOPTIONS     opt);
        /** \brief Removes all the callbacks associated to the reception of a message.
         *  \param msg_id identifier of the message
         *  \return TRUE if success
         */
        bool RemoveMessageReceivingCallback(    CANBasicTypes::type_uinteger        msg_id);


        /** \brief Removes a callback associated to the transmission of a signal with the given option.
         *  \param signal_name name of the signal
         *  \param opt callback transmission option
         *  \return TRUE if success
         */
        bool RemoveSignalTransmissionCallback(  CANBasicTypes::type_string          signal_name,
                                                SignalTransmittingCallbackOPTIONS   opt);

        /** \brief Removes all the callbacks associated to the transmission of a signal.
         *  \param signal_name name of the signal
         *  \return TRUE if success
         */
        bool RemoveSignalTransmissionCallback(  CANBasicTypes::type_string          signal_name);

        /** \brief Removes a callback associated to the reception of a signal with the given option.
         *  \param signal_name name of the signal
         *  \param opt callback receiving option
         *  \return TRUE if success
         */
        bool RemoveSignalReceivingCallback(     CANBasicTypes::type_string          signal_name,
                                                SignalReceivingCallbackOPTIONS      opt);        

        /** \brief Removes all the callbacks associated to the reception of a signal.
         *  \param signal_name name of the signal
         *  \return TRUE if success
         */
        bool RemoveSignalReceivingCallback(     CANBasicTypes::type_string          signal_name);  

        // ----------------------------------------------------------
        //                      CALLBACK INVOKATION
        // ----------------------------------------------------------
        /** \brief Performs the callback for the message if installed.
         *  \param msg_id identifier of the message
         *  \param opt callback transmission option
         *  \param msg message data passed to the callback
         *
         */
        void TransmitMessageCallbackCALL(   CANBasicTypes::type_uinteger        msg_id, 
                                            MessageTransmissionCallbackOPTIONS  opt,
                                            CANMessage                          *msg);

        /** \brief Performs the callback for the message if installed.
         *  \param msg_id identifier of the message
         *  \param opt callback receiving option
         *  \param msg message data passed to the callback
         */
        void ReceiveMessageCallbackCALL(    CANBasicTypes::type_uinteger        msg_id, 
                                            MessageReceivingCallbackOPTIONS     opt,
                                            CANMessage                          *msg);

        /** \brief Performs the callback for the signal if installed.
         *  \param signal_name name of the signal
         *  \param opt callback transmission option
         *  \param signal signal data passed to the callback
         */
        void TransmitSignalCallbackCALL(    CANBasicTypes::type_string          signal_name, 
                                            SignalTransmittingCallbackOPTIONS   opt,
                                            CANSignal                           *signal);

        /** \brief Performs the callback for the signal if installed.
         *  \param signal_name name of the signal
         *  \param opt callback receiving option
         *  \param signal signal data passed to the callback
         */
        void ReceiveSignalCallbackCALL(     CANBasicTypes::type_string          signal_name, 
                                            SignalReceivingCallbackOPTIONS      opt,
                                            CANSignal                           *signal);

        };

    }