/** \mainpage CANBUS module main page.
 *  \author Roberto Sartori, Dynamotion S.r.l.
 * 
 * This manual documents the the CANBUS manager module API developed to manage a CAN BUS 
 * network architecture. \n
 * The module is written in C++ language and it is distributed as a dynamic linked library (DLL) under
 * Microsoft Windows platform. It is compatible with Peak-System USB and PCI devices.
 * 
 * This manual is divided into the following sections: \n \n
 * \subpage canbus_architecture : general description of the canbus architecture management \n \n
 * \subpage managers : description of the architecture management subdivision \n \n
 * \subpage examples : some example of API usage
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \page canbus_architecture Architecture management
 * The CANBUS module offers a high-level set of API used to manage the CANBUS network architecture.
 * In particular, <b>architecture management</b> concerns the management of the elements which compose
 * a CANBUS network (ECUs, messages and signals), the way they are connected each other (i.e. which signals 
 * are packed in which messages, which messages are sent by which ECUs...) and the rules with which data is 
 * exchanged (hardware routing). \n
 * In a CANBUS architecture its possible to do the following operations:
 *
 * \section Signal_management Signal management
 * \li create a signal with its properties (min val, max val, units, etc)
 * \li remove a signal
 * \li specify by which ECU a signal is transmitted
 * \li specity by whose ECUs a signal is received
 *
 * \section Message_management Message management
 * \li create a message with its properties (byte order, frequency, length, etc)
 * \li remove a message
 * \li define whose (and how) signals are packed in a message
 *
 * \section ECU_management ECU management
 * \li create/remove an ECU
 * \li mark an ECU as transmitter of a message/signal
 * \li mark an ECU as receiver of a message/signal
 *
 * \section Hardware_management Hardware management
 * \li scan the available CAN hardware devices
 * \li start/stop data transmission of a hardware device
 * \li start/stop reception of a hardware device
 * \li raw message transmission on the canbus
 * \li raw message reception from the canbus
 *
 * \section Transmission_management Transmission management
 * \li execute a single message transmission
 * \li activate/deactivate automatic transmission of periodic messages
 * \li enable automatic transmission of a message for a fixed number of times with a fixed frequency
 * \li enable automatic transmission of a message for a fixed number of seconds with a fixed frequency
 * \li collect transmission statistics of all transmitted messages like the number of times the 
 *     message was transmitted and the average transmission period
 * \li enable/disable signals (default disabled signal's bit packing value is zero)
 * \li bind a message trnsmission to a specific hardware device (different messages 
 *     can be transmitted over different devices)
 *
 * \section Receiving_management Receiving management
 * \li enable automatic message reception management
 * \li setup receiving filters for messages and signals
 *
 * \section Events_management Events management
 * For an efficient asynchronous management of the CANBUS data, the architecture management offers the
 * tools to install some user-defined callbacks which will be called in the occurence of the
 * following events: 
 *
 * <I>message callbacks events</I>
 * \li before signals are packed into a message
 * \li after signals have been packed in a message but before its transmission
 * \li immediatly after a message transmission
 *
 * <I>signal callback events</I>
 * \li before a signal is going to be packed into a message that is going to be transmitted
 * \li after a signals has been packed into a message which is going to be transmitted
 * \li before a signal is going to be unpacked and updated because a message which was packing it has 
 *     been received
 * \li after a signal has been updated because a message which was packing it has been received
 *
 * For a more detailed description of how to manage these functions see \subpage managers.
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \page managers Managers
 * The CANBUS module is divided into \b managers each of which is dedicated to some
 * specific functions. The list of available managers is the following:
 *
 * \section Signal_manager Signal manager
 * It manages all the signals in the architecture. It allows to add and remove signals from
 * the architecture and it has to be used to get a reference to any existing signal. See \link 
 * FrameworkCanBus::CANSignalManager CANSignalManager\endlink for implementation details.
 *
 * \section Message_manager Message manager
 * It manages all the messages in the architecture. It allows to add and remove messages from
 * the architecture and it has to be used to get a reference to any existing message. See 
 * \link FrameworkCanBus::CANMessageManager CANMessageManager\endlink for implementation details.
 *
 * \section ECU_manager ECU manager
 * It manages all the ECUs in the architecture. It allows to add and remove ECUs from the architecture
 * and it has to be used to get a reference to any existing ECU. See \link FrameworkCanBus::CANEcuManager 
 * CANEcuManager\endlink for implementation details.
 *
 * \section Hardware_manager Hardware manager
 * It manages the available CANBUS hardware devices. It performs an automatic scan of the system hardware and 
 * provides a list of the available devices, included a "virtual CANBUS device" which is an emulated device useful
 * for testing architectures on local machines which do not have any CANBUS hardware device. For any device a 
 * loopback tecnique is implemented and available. The hardware manager returns a reference to any device 
 * through a standard interface which allows to activate/deactivate the device and to transmit/receive 
 * data at low-level. See \link FrameworkCanBus::CANHardwareManager CANHardwareManager\endlink for 
 * implementation details.
 *
 * \section Transmitter_manager Transmitter manager
 * It manages the high-level transmission of messages. It implements automatic packing of signals to create
 * the correct message data to be transmitted over the CANBUS network according to the signal's properties and
 * packing position. Periodic messages are automatically transmitted with the right timing.
 * Transmitter manager has methods to manage non-periodic messages, also called <b>one-shot messages</b>, which 
 * can be automatically packed and transmitted once any time needed. As a consequence user can develope its
 * own transmitting strategy and timing management if needed.
 * For each transmitted message some statistical data (number of times the message was sent, average
 * transmission period, etc) are collected and can be obtained. See \link FrameworkCanBus::CANTransmitterManager
 * CANTransmitterManager \endlink for implementation details.
 *
 * \section Receiver_manager Receiver manager
 * It manages the reception of messages from the enabled devices. It automatically unpacks the received 
 * messages and updates the packed signal values. Receiving filters allows to select which messages can 
 * be receivied and which signals have to be updated. See \link FrameworkCanBus::CANReceiverManager 
 * CANReceiverManager\endlink for implementation details.
 *
 * \section Callback_manager Callback manager
 * It allows to install some callbacks associated to message transmission/receiving and to signals 
 * packing/unpacking events. User can be intrested in capturing some significant events like in the 
 * following examples:
 *
 * \li "the message 0x100 is going to be transmitted"
 * \li "the message 0x4F has been received and unpacked"
 * \li "the signal SPEED is going to be updated from the CANBUS with a new value"
 * \li "the signal TEMPERATURE has been updated"
 * \li "the signal TIMESTAMP is going to be packed into a message for transmission"
 *
 * all these events can be catched installing proper callabcks associated to message or signal events. More 
 * in details, the list of events that can be associated to each message are:
 *
 * \li the message has to be transmitted and its data is going to be packet according to the current 
 *     signal values
 * \li the message's data has been generated and the message is going to be transmitted on the CANBUS network
 * \li the message has been transmitted
 * \li the message has been received and its data has not been unpacked yet
 * \li the message has been received and its data has been unpacked
 *
 * Moreover, the list of events that can be associated to each signal are:
 *
 * \li the signal is going to be packed into a message which will be transmitted on the CANBUS network
 * \li the signal has been packed into a message which will be transmitted on the CANBUS network
 * \li the signal is going to be updated with a new received value
 * \li the signal has been updated with a new received value
 *
 * For each message and signal a callback can be associated to the desired event and it will be called 
 * automatically from the callback manager. See \link FrameworkCanBus::CANCallbackManager 
 * CANCallbackManager\endlink for implementation details.
 *
 * \section Architecture_manager Architecture manager
 * This is the most important module, it manages the whole architecture ensuring that coherence is mantained 
 * in the relationships between signals, messages and ECUs. This manager allows to do the higher-level 
 * operations on the CANBUS architecture like:
 *
 * \li mark a signal as packed into a message checking for collisions with other signals
 * \li start an ECU i.e. start transmitting all the messages for which the ECU is marked as transmitter
 * 
 * One of the most important features is the <b>DBC compatibility</b> of the architecture management. 
 * This means that the architecture manager can build the CANBUS architecture loading the architecture 
 * definition from a <b>.dbc</b> file generated with the free tool <b>CANAlyzer DBC Editor</b>. See
 * \link FrameworkCanBus::CANArchitectureManager CANArchitectureManager\endlink for implementation details.
 */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \page examples Examples
 *
 * Available examples are: \n \n
 * \subpage example1 : print the list of available CANBUS devices \n \n
 * \subpage example2 : create a message and send it over a device every 500ms for 10 sec \n \n
 * \subpage example3 : uses the <b>architecture manager</b> to load the demo.dbc architecture file and 
                       starts an ECU transmission \n \n
 * \subpage example4 : uses the <b>architecture manager</b>, uses the virtual device in loopback 
                       configuration, installs receiving callback \n \n
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \page example1 Example 1
 *
 * This example instantiates the hardware manager object and perform a hardware scan of the available
 * CANBUS devices.
 *
 * \code
int main(int argc, char* argv[])
{
    // harware management
    CANHardwareManager *hwm;
    CANHardwareManager::CANHardwareNameList_type list;

    cout << endl << "---------- HARDWARE LIST" << endl;
    // create the hardware manager
    hwm = new CANHardwareManager();
    // get the list of available devices
    hwm->GetHardwareNameList(list);
    // print the list on screen
    CANHardwareManager::CANHardwareNameList_type::iterator it = list.begin();
    while(it != list.end()) {
        cout << (*it) << ", deviceID = " << hwm->GetHardwareInterface(*it)->getDeviceID() << endl;
        it++;
        }
    cout << "list END" << endl;
    // wait user to give some input
    char c;
    cin >> c;
    // deallocate the hardware manager and exit and exit    delete hwm;
    return 0;
    }
 * \endcode
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \page example2 Example 2
 *
 * This example manually initialize the hardware manager, get the hardware interface of the last available
 * device, creates a message with some properties and sends the message using the opened device every
 * 500ms for 10sec. At each send the data of the message is changed. \n
 * This is an example of how it is possible to manually send messages.
 *
 * \code
int main(int argc, char* argv[])
{
    // harware management
    CANHardwareManager *hwm = NULL;
    CANHardwareManager::CANHardwareNameList_type list;
    list.clear();

    // hardware interface
    CANHardwareInterface *HIF = NULL;
    CANHardwareConfiguration config;

    // local messages
    CANMessage *message = NULL;
    CANMessageProperties msg_prop;

    cout << endl << "---------- SINGLE MESSAGE TEST" << endl;
    // initialize the hardware manager and get the list of available devices
    hwm = new CANHardwareManager();
    hwm->GetHardwareNameList(list);
    if (list.empty()) {
        cout << "no hardware devices found.";
    } else {
        // get the first available hardware
        HIF = hwm->GetHardwareInterface(*(list.rbegin()));
        // set the device configuration
        config.baudrate = CANHardwareConfiguration::BAUDRATE_500K;
        config.loopback = false;
        // open the device with the assigned configuration
        if (!HIF->Open(config)) {
            // error opening the device, show the error message
            cout << "FAILED: " << HIF->PopErrorStack() << endl;
        } else {
            // set message properties
            msg_prop.set_cycle_time(500);
            msg_prop.set_dlc(8);
            msg_prop.set_id(0x100);
            msg_prop.set_message_type(CANMessageTypes::MSG_STANDARD);
            msg_prop.set_name("TEST MESSAGE");
            // create the message with the assigned properties
            message = new CANMessage(&msg_prop);
            // transmit the message every 500ms with low-level hardware interface method
            for (unsigned char i=0;i<20;i++) {
                unsigned char data[8] = {0,0,0,0,0,0,0,0};
                // sleep a while
                Sleep(500);
                // generate progressive data
                data[0] = i;
                // fill message data
                message->set_data((void *)data);
                // transmit the message: low-level device function 
                HIF->Transmit(message);
                }
            // close the device
            HIF->Close();
            // deallocate objects
            delete message;
            }
        }
    // deallocate hardware manager
    delete hwm;
    return 0;
    }
 * \endcode
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \page example3 Example 3
 *
 * This example loads the demo.dbc architecture description and starts transmitting the messages from the 
 * TEST ECU using the last available device. \n
 * In this example the <b>architecture manager</b> class is used to do most of the work without the need
 * to manually create messages and manage send and receiving timing.
 *
 * \code
int main(int argc, char* argv[])
{
    
    // create an instance of architecture manager
    CANArchitectureManager *am = new CANArchitectureManager();
    // check for creation errors
    if (am->HasError()) {
        // print error message
        cout << am->PopErrorStack();
        delete am;
        return 0;
        }
    // get the available devices list
    CANHardwareManager::CANHardwareNameList_type hw_list;
    if (!am->get_hardware_manager()->GetHardwareNameList(hw_list)) {
        cout << "error retriving the device list.";
        delete am;
        return 0;
        }
    // set the last device as the default
    if (!am->SetDefaultHardwareDevice(*(hw_list.rbegin()))) {
        cout << "failed to set the default device";
        delete am;
        return 0;
        }
    // configure the hardware for 500K baud, no loopback
    CANHardwareConfiguration HW_config;
    HW_config.baudrate = CANHardwareConfiguration::BAUDRATE_500K;
    HW_config.loopback = false;
    if (!am->GetDefaultHardwareInterface()->Open(HW_config)) {
        cout << "error initializing the device";
        delete am;
        return 0;
        }
    
    if (!am->LoadDBC("demo.dbc")) {
        cout << am->PopErrorStack();
        delete am;
        return 0;
        }

    // now start transmitting the message on the default hardware
    am->StartECU("TestNode");
    // retrive some signal references
    CANSignal *sine1 = am->get_signal_manager()->get_signal("Sine1");
    CANSignal *sine2 = am->get_signal_manager()->get_signal("Sine2");
    CANSignal *state = am->get_signal_manager()->get_signal("State");
    CANSignal *curve = am->get_signal_manager()->get_signal("Curve");

    // update signals for 10 seconds every 10ms
    for (int i=0; i<1000; i++) {
        // update signal values
        sine1->set_value(sin(double(i)/100));
        sine2->set_value(sin(double(i+50)/100));
        state->set_value((i / 50) % 2);
        curve->set_value(i);
        // sleep 10ms
        Sleep(10);
        }
    // stop the ECU, close the device and exit
    am->StopECU("TestNode");
    am->GetDefaultHardwareInterface()->Close();

    // print architecture description
    cout << am->GetDescription() << endl;
    
    char c;
    cin >> c;

    delete am;
    return 0;
    }
 * \endcode
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \page example4 Example 4
 *
 * This example loads the demo.dbc architecture description and starts transmitting the messages from the 
 * TEST ECU using the virtual device with the loopback active. The Sine1 signal defined in the demo.dbc file
 * is updated every second. The messages reception is enabled on the virtual device and a callback associated
 * to the receiving of new values of the Sine1 signal is installed in order to trigger this event on screen. 
 * After 10 seconds the application is terminated.
 *
 * \code

// CTime class for computing elapsed time.
CTime time;

// Signal transmitting callback
void transmit_callback(void *p) {
    // print signal name and value
    CANSignal *signal = (CANSignal *)p;
    cout << "transmit " << signal->get_properties()->get_name() << " = " << signal->get_value() << " at t=" << time.TOC() << endl;
    }

// Signal receiving callback
void rcv_callback(void *p) {
    // print signal name and value
    CANSignal *signal = (CANSignal *)p;
    cout << "receive " << signal->get_properties()->get_name() << " = " << signal->get_value() << " at t=" << time.TOC() << endl;
    }

int main(int argc, char* argv[])
{

    // architecture manager
    CANArchitectureManager *am;

    // create an instance of architecture manager
    am = new CANArchitectureManager();
    // check for creation errors
    if (am->HasError()) {
        // print error message
        cout << am->PopErrorStack();
        delete am;
        return 0;
        }
    // set the virtual device as the default
    if (!am->SetDefaultHardwareDevice("CANBUS_Vitual_Device")) {
        cout << "failed to set the default device";
        delete am;
        return 0;
        }
    // configure the hardware for 500K baud, no loopback
    CANHardwareConfiguration HW_config;
    HW_config.baudrate = CANHardwareConfiguration::BAUDRATE_500K;
    HW_config.loopback = true;
    if (!am->GetDefaultHardwareInterface()->Open(HW_config)) {
        cout << "error initializing the device";
        delete am;
        return 0;
        }
    
    if (!am->LoadDBC("demo.dbc")) {
        cout << am->PopErrorStack();
        delete am;
        return 0;
        }

    // now start transmitting the message on the default hardware
    am->StartECU("TestNode");

    // retrive some signal references
    CANSignal *sine1 = am->get_signal_manager()->get_signal("Sine1");

    // install signal receiving callback 
    am->get_callback_manager()->InstallSignalReceivingCallback( "Sine1",
                                                                rcv_callback, 
                                                                CANCallbackManager::SIGNAL_AFTER_UNPACKING);

    // install signal transmitting callback 
    am->get_callback_manager()->InstallSignalTransmissionCallback( "Sine1",
                                                                transmit_callback, 
                                                                CANCallbackManager::SIGNAL_AFTER_PACKING);
    // start receiving messages
    am->StartReceivingFromDefaultHardwareDevice();

    // update signals for 10 seconds every 1 second
    for (int i=0; i<10; i++) {
        // update signal value 
        sine1->set_value(sin(double(i)/100));
        // sleep 10ms
        Sleep(1000);
        }
    // stop all ECU transmission
    //am->StopECU("TestNode");
    am->StopAllECU();
    // Stop receiving messages
    am->StopReceivingFromDefaultHardwareDevice();
    // remove signal receiving callback
    am->get_callback_manager()->RemoveSignalReceivingCallback("Sine1");
    // close the virtual device
    am->GetDefaultHardwareInterface()->Close();
    // deallocate can architecture and exit
    delete am;
    return 0;
}
 * \endcode
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXTRA DOCUMENTATION OF NAMESPACES

/** \brief CAN classes container
 *
 * This namespace contains all the classes of the CANBUS manager. User can
 * initialize and interact with the canbus architecture accessing to any information about signals, 
 * messages and ECUs and registering callbacks for some specific events through these classes. \n
 */
namespace FrameworkCanBus {}
