#include "FrameworkCanBus/CANHardwareInterface.h"

using namespace FrameworkCanBus;

CANHardwareInterface::CANHardwareInterface() {
    _hwInterfaceName.clear();
    _hwInterfaceName = "CAN Hardware Interface - virtual";
    while(!_loopbackQueue.empty()) _loopbackQueue.pop();
    _loopbackMaxSize = 1024;
    _loopback = false;
    }
/*
*/
CANHardwareInterface::~CANHardwareInterface() {
    while(!_loopbackQueue.empty()) _loopbackQueue.pop();
    };

/*
*/
bool CANHardwareInterface::Test() {
    return true;
    }

/*
*/
bool CANHardwareInterface::isOpen() {
    return _open;
    }

/*
*/
bool CANHardwareInterface::Open(const CANHardwareConfiguration config) {
    _loopback = config.loopback;
    return true;
    }

/*
*/
bool CANHardwareInterface::Close() {
    return true;
    }

/*
*/
bool CANHardwareInterface::Transmit(CANMessage *msg) {
    return true;
    }

/*
*/
bool CANHardwareInterface::Receive(CANMessage *msg) {
    return true;
    }

bool CANHardwareInterface::Receive(CANMessage *msg, unsigned int timeout_ms) {
    return true;
}

/*
*/
void CANHardwareInterface::EmptyReceivingBuffer() {
    _synch.InstanceLock("_loopback");
    while(_loopbackQueue.size()!=0)
        _loopbackQueue.pop();
    _synch.InstanceUnlock("_loopback");
    return;
    }


/*
*/
unsigned int CANHardwareInterface::getDeviceID() {
    return 0;
    }

/*
*/
void CANHardwareInterface::EnableLoopback() {
    _synch.InstanceLock("_loopback");
    _loopback = true;
    _synch.InstanceUnlock("_loopback");
    EmptyReceivingBuffer();
    }

/*
*/
void CANHardwareInterface::DisableLoopback() {
    _synch.InstanceLock("_loopback");
    _loopback = false;
    _synch.InstanceUnlock("_loopback");
    EmptyReceivingBuffer();
    }

/*
*/
bool CANHardwareInterface::LoopbackTransmit(CANMessage *msg) {
    if (!_loopback) return false;
    _synch.InstanceLock("_loopback");
    // discard last element if queue is full
    if (_loopbackQueue.size() >= _loopbackMaxSize) {
        delete _loopbackQueue.front();
        _loopbackQueue.pop();
        }
    // add message to _loopback buffer
    CANMessage *new_msg = new CANMessage(msg->get_properties());
    new_msg->set_data(msg->get_data());
    _loopbackQueue.push(new_msg);
    _synch.InstanceUnlock("_loopback");
    return true;
    }

/*
*/
bool CANHardwareInterface::LoopbackReceive(CANMessage *p_msg) {
    if (!_loopback) return false;
    _synch.InstanceLock("_loopback");
    // return lase message if present
    if (_loopbackQueue.size()>0) {
        // get the last message
        CANMessage *msg = _loopbackQueue.front();
        p_msg->set_properties(msg->get_properties());
        p_msg->set_data(msg->get_data());
        // remove the last message
        _loopbackQueue.pop();
        delete msg;
        _synch.InstanceUnlock("_loopback");
        return true;
    } else {
        _synch.InstanceUnlock("_loopback");
        return false;
        }
    }


/*
*/
void CANHardwareInterface::SetLoopbackBufferSize(CANBasicTypes::type_uinteger size) {
    _synch.InstanceLock("_loopback");
    _loopbackMaxSize = size;
    while (_loopbackQueue.size() > _loopbackMaxSize)
        _loopbackQueue.pop();
    _synch.InstanceUnlock("_loopback");
    }

