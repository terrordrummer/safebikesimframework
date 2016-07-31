#include "FrameworkCanBus/CVirtualCANBUSDevice.h"

using namespace FrameworkCanBus;
using namespace std;

CVirtualCANBUSDevice::CVirtualCANBUSDevice() {
    _hwInterfaceName.clear();
    _hwInterfaceName = "CANBUS_Vitual_Device";
    _open = false;
    _loopback = true;
    }

/** \brief Test a CANBUS hardware.                      */
bool CVirtualCANBUSDevice::Test() {
    return true;
    }

/** \brief Open a CANBUS hardware.                      */
bool CVirtualCANBUSDevice::Open(const CANHardwareConfiguration config) {
    _open = true;
    return true;
    }

/** \brief Closes the connection to the hardware.       */
bool CVirtualCANBUSDevice::Close() {
    _open = false;
    return true;
    }

/** \brief Transmit a message to the hardware.          */
bool CVirtualCANBUSDevice::Transmit(CANMessage *msg){
    return true;
    }

/** \brief Wait for a message from the hardware.        */
bool CVirtualCANBUSDevice::Receive(CANMessage *msg){
    // FALSE because the virtual device receives data only through the loopback queue
    return false;
    }

bool CVirtualCANBUSDevice::Receive(CANMessage *msg, unsigned int timeout_ms) {
    return false;
}

/** \brief Empty the receiving buffer.                  */
void CVirtualCANBUSDevice::EmptyReceivingBuffer(){
    return;
    }

/** \brief Hardware data of the device. */
string CVirtualCANBUSDevice::GetDescription() {
    return string("Virtual CANBUS device");
    }
