// ClientTest.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "FrameworkCanBus/CANHardwareInterface.h"
#include "FrameworkCanBus/CANHardwareManager.h"
#include "FrameworkCanBus/CANArchitectureManager.h"
#include "math.h"

using namespace FrameworkCanBus;
using namespace std;

// SELECT THE DEMO TO BUILD - ONLY ONE OPTION AT TIME
//#define HARDWARE_SCAN           // print the list of available hardware devices
//#define SINGLE_MESSAGE_TEST   // create a message and transmit 10 messages every 500ms changing the first byte data content 
#define DEMO_DBC              // loads a DBC architecture file and starts to send some data
//#define SIGNAL_CALLBACK       // receiving callback installation

#ifdef SIGNAL_CALLBACK

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

#endif
/** local macro definitions */
#define SAFEDELETE(ptr) {if (ptr!=NULL) {delete ptr; ptr=NULL;}}

#ifdef HARDWARE_SCAN
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
#endif


#ifdef SINGLE_MESSAGE_TEST
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
#endif

    
#ifdef DEMO_DBC
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
#endif


#ifdef SIGNAL_CALLBACK
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
#endif
