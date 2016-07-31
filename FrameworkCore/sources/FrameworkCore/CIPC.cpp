#include "FrameworkCore/CIPC.h"

// namespaces
using namespace FrameworkCore;
using namespace std;

// defines
#define IP_BROADCAST 0xffffffff

/*
  ____ __  __       _ _   _               _   ____             _        _
 / ___|  \/  |_   _| | |_(_) ___ __ _ ___| |_/ ___|  ___   ___| | _____| |_
| |   | |\/| | | | | | __| |/ __/ _` / __| __\___ \ / _ \ / __| |/ / _ \ __|
| |___| |  | | |_| | | |_| | (_| (_| \__ \ |_ ___) | (_) | (__|   <  __/ |_
 \____|_|  |_|\__,_|_|\__|_|\___\__,_|___/\__|____/ \___/ \___|_|\_\___|\__|
*/
CMulticastSocket::CMulticastSocket() {
    SetErrorPrefix("<CMulticastSocket>");
}

/*
     _       _        ____
    | | ___ (_)_ __  / ___|_ __ ___  _   _ _ __
 _  | |/ _ \| | '_ \| |  _| '__/ _ \| | | | '_ \
| |_| | (_) | | | | | |_| | | | (_) | |_| | |_) |
 \___/ \___/|_|_| |_|\____|_|  \___/ \__,_| .__/
                                          |_|
*/
void CMulticastSocket::JoinGroup(quint32 aIP, quint16 aPort) {
    int results = 0;
 
    // Bind this socket to the requested multicast address.
    //    - Use the platform default binding mode.
    QUdpSocket::bind( aPort );

    //--< Winsock2 specific code >------------------------------------
    SOCKET      WinSocket = socketDescriptor();
    SOCKADDR_IN localIF;
    struct ip_mreq mreq;

    // query the system for the local network interface IP address.
    hostent  *pHost = gethostbyname("");   // gets the localhost data.

    if(0 == pHost) {
        PushError("[JoinGroup] Unknown socket error getting local host data.");
        return;
    }
    else if(AF_INET == pHost->h_addrtype)
    {
      // for simplicity, assume that the test systems only have one network
      //    interface, therefore select the first IPv4 address in the list and
      //    store in host byte order.
      mLocalIP = ntohl( * reinterpret_cast<quint32*>( *pHost->h_addr_list ) );
    }

    // bind to this socket to the provided port number.
    localIF.sin_family   = AF_INET;
    localIF.sin_port     = htons( aPort );
    localIF.sin_addr.s_addr = htonl( INADDR_ANY );
    results = ::bind( WinSocket, (SOCKADDR*)&localIF, sizeof(localIF) );
    /*---< NOTE >---------------------------------
     Since QUdpSocket overloaded the a number of
     bind() method implementations, the global scope
     call {::bind(...)} was required to force the
     compiler to use the Winsock2 bind function.
    --------------------------------------------*/
    if(SOCKET_ERROR == results) {
        // get the error string
        LPTSTR message;
        results = WSAGetLastError();
        FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                        , 0
                        , results
                        , 0
                        , message
                        , 0
                        , NULL);
        PushError("[JoinGroup] Error binding the socket: <" + string(message) + ">");
        LocalFree(message);
        return;
    }
 
    // subscripe to the multicast group
    mreq.imr_interface.s_addr = htonl( mLocalIP );
    mreq.imr_multiaddr.s_addr = htonl( aIP );
    results = setsockopt(WinSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq) );

    if(SOCKET_ERROR == results) {
        // get the error string
        LPTSTR message;
        results = WSAGetLastError();
        FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                        , 0
                        , results
                        , 0
                        , message
                        , 0
                        ,NULL);
        PushError("[JoinGroup] Error adding socket to multicast group: <" + string(message) + ">");
        LocalFree(message);
        return;
        }
    }

/*
 _                          ____
| |    ___  __ ___   _____ / ___|_ __ ___  _   _ _ __
| |   / _ \/ _` \ \ / / _ \ |  _| '__/ _ \| | | | '_ \
| |__|  __/ (_| |\ V /  __/ |_| | | | (_) | |_| | |_) |
|_____\___|\__,_| \_/ \___|\____|_|  \___/ \__,_| .__/
                                                |_|
*/
void CMulticastSocket::LeaveGroup() {
    //--< Winsock2 specific code >------------------------------------
   SOCKET socket = socketDescriptor();
 
   // leave the currently connected multicast group
   if((IP_BROADCAST == mMcastIP) && (INVALID_SOCKET != socket))
   {
      int results = 0;
      struct ip_mreq mreq;
 
      mreq.imr_interface.s_addr = htonl( mLocalIP );
      mreq.imr_multiaddr.s_addr = htonl( mMcastIP );
 
      results = setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq) );
 
      // catch and log the error, if any
      if(SOCKET_ERROR == results)
      {
         results = WSAGetLastError();
         error( QAbstractSocket::UnknownSocketError );
      }
   }
   //--< Winsock2 specific code >------------------------------------
    }



/*
  ____ ___ ____   ____
 / ___|_ _|  _ \ / ___|
| |    | || |_) | |
| |___ | ||  __/| |___
 \____|___|_|    \____|
*/
CIPC::CIPC() : _joined(false) {
    SetErrorPrefix("<CIPC>");
    // clear the channels map
    _channels.clear();
}

/*
     _       _        ____
    | | ___ (_)_ __  / ___|_ __ ___  _   _ _ __
 _  | |/ _ \| | '_ \| |  _| '__/ _ \| | | | '_ \
| |_| | (_) | | | | | |_| | | | (_) | |_| | |_) |
 \___/ \___/|_|_| |_|\____|_|  \___/ \__,_| .__/
                                          |_|
*/
bool CIPC::JoinGroup() {
    // avoid to reconnect again
    if (!_joined) {
        // join the group
        _socket.JoinGroup(IP_BROADCAST, 12012);
        // check erors
        if (_socket.HasError())
            PushError("[JoinGroup] " + _socket.PopErrorStack());
        else
            _joined = true;
    }
    // done
    return _joined;
}

/*
 ____                _  ____ _                            _
|  _ \ ___  __ _  __| |/ ___| |__   __ _ _ __  _ __   ___| |
| |_) / _ \/ _` |/ _` | |   | '_ \ / _` | '_ \| '_ \ / _ \ |
|  _ <  __/ (_| | (_| | |___| | | | (_| | | | | | | |  __/ |
|_| \_\___|\__,_|\__,_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/
bool CIPC::ReadChannel(std::string channel_name, FrameworkCore::CGenericType &data) {

    return true;
}

/*
 ____                _  ____ _                            _
|  _ \ ___  __ _  __| |/ ___| |__   __ _ _ __  _ __   ___| |
| |_) / _ \/ _` |/ _` | |   | '_ \ / _` | '_ \| '_ \ / _ \ |
|  _ <  __/ (_| | (_| | |___| | | | (_| | | | | | | |  __/ |
|_| \_\___|\__,_|\__,_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/ 
bool ReadChannel(std::string channel_name, FrameworkCore::CGenericType &data, double timeout = 1000.0) {
    return true;
};

/*
__        __    _ _        ____ _                            _
\ \      / / __(_) |_ ___ / ___| |__   __ _ _ __  _ __   ___| |
 \ \ /\ / / '__| | __/ _ \ |   | '_ \ / _` | '_ \| '_ \ / _ \ |
  \ V  V /| |  | | ||  __/ |___| | | | (_| | | | | | | |  __/ |
   \_/\_/ |_|  |_|\__\___|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/
bool CIPC::WriteChannel(std::string channel_name, FrameworkCore::CGenericType &data) {
    // check that channel has been already opened
    if (_channels.find(channel_name) == _channels.end()) {
        PushError("[WriteChannel] Attempt to write data to the non-initialized channel <" + channel_name + ">");
        return false;
    }
    std::string dump = data.Dump();
    tChannelInfo channel = _channels.find(channel_name)->second;

    // write data to the channel!!!!
    qint64 res = _socket.writeDatagram(dump.c_str(), dump.length(), channel.IP_address, 12012);
    
    return true;
}

/*
 _                          ____
| |    ___  __ ___   _____ / ___|_ __ ___  _   _ _ __
| |   / _ \/ _` \ \ / / _ \ |  _| '__/ _ \| | | | '_ \
| |__|  __/ (_| |\ V /  __/ |_| | | | (_) | |_| | |_) |
|_____\___|\__,_| \_/ \___|\____|_|  \___/ \__,_| .__/
                                                |_|
*/
bool CIPC::LeaveGroup() {
    _socket.LeaveGroup();
    _joined = false;
    return true;
}



/*
 ____  _           _  ____ _                            _ ____               _                 ____                                          _
| __ )(_)_ __   __| |/ ___| |__   __ _ _ __  _ __   ___| |  _ \ ___  ___ ___(_)_   _____ _ __ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
|  _ \| | '_ \ / _` | |   | '_ \ / _` | '_ \| '_ \ / _ \ | |_) / _ \/ __/ _ \ \ \ / / _ \ '__| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
| |_) | | | | | (_| | |___| | | | (_| | | | | | | |  __/ |  _ <  __/ (_|  __/ |\ V /  __/ |  | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|____/|_|_| |_|\__,_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|_| \_\___|\___\___|_| \_/ \___|_|   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
*/
bool CIPC::BindChannelReceiverCommand(std::string channel, std::string command_name) {
    return true;
}

/*
 _   _       _     _           _  ____ _                            _ ____               _                 ____                                          _
| | | |_ __ | |__ (_)_ __   __| |/ ___| |__   __ _ _ __  _ __   ___| |  _ \ ___  ___ ___(_)_   _____ _ __ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
| | | | '_ \| '_ \| | '_ \ / _` | |   | '_ \ / _` | '_ \| '_ \ / _ \ | |_) / _ \/ __/ _ \ \ \ / / _ \ '__| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
| |_| | | | | |_) | | | | | (_| | |___| | | | (_| | | | | | | |  __/ |  _ <  __/ (_|  __/ |\ V /  __/ |  | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
 \___/|_| |_|_.__/|_|_| |_|\__,_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|_| \_\___|\___\___|_| \_/ \___|_|   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
*/
void CIPC::UnbindChannelReceiverCommand(std::string channel, std::string command_name) {

}

/*
 _   _       _     _           _       _ _  ____ _                            _ ____               _                 ____                                          _
| | | |_ __ | |__ (_)_ __   __| | __ _| | |/ ___| |__   __ _ _ __  _ __   ___| |  _ \ ___  ___ ___(_)_   _____ _ __ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |___
| | | | '_ \| '_ \| | '_ \ / _` |/ _` | | | |   | '_ \ / _` | '_ \| '_ \ / _ \ | |_) / _ \/ __/ _ \ \ \ / / _ \ '__| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
| |_| | | | | |_) | | | | | (_| | (_| | | | |___| | | | (_| | | | | | | |  __/ |  _ <  __/ (_|  __/ |\ V /  __/ |  | |__| (_) | | | | | | | | | | | (_| | | | | (_| \__ \
 \___/|_| |_|_.__/|_|_| |_|\__,_|\__,_|_|_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|_| \_\___|\___\___|_| \_/ \___|_|   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
*/
void CIPC::UnbindallChannelReceiverCommands(std::string channel) {
}



/*
  ____      _    ____ _                            _ ___        __
 / ___| ___| |_ / ___| |__   __ _ _ __  _ __   ___| |_ _|_ __  / _| ___
| |  _ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ || || '_ \| |_ / _ \
| |_| |  __/ |_| |___| | | | (_| | | | | | | |  __/ || || | | |  _| (_) |
 \____|\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|___|_| |_|_|  \___/
*/
std::string CIPC::GetChannelInfo() {
    return "";
}

/*
  ____      _    ____                            _           _ ____
 / ___| ___| |_ / ___|___  _ __  _ __   ___  ___| |_ ___  __| |  _ \ _ __ ___   ___ ___ ___ ___  ___ ___
| |  _ / _ \ __| |   / _ \| '_ \| '_ \ / _ \/ __| __/ _ \/ _` | |_) | '__/ _ \ / __/ _ Y __/ __|/ _ Y __|
| |_| |  __/ |_| |__| (_) | | | | | | |  __/ (__| ||  __/ (_| |  __/| | | (_) | (_|  __|__ \__ \  __|__ \
 \____|\___|\__|\____\___/|_| |_|_| |_|\___|\___|\__\___|\__,_|_|   |_|  \___/ \___\___|___/___/\___|___/
*/
vector<std::string> CIPC::GetConnectedProcesses() {
    vector<std::string> ret;
    return ret;
}