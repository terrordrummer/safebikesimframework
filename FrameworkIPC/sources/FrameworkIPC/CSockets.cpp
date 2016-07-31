/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#define _CRT_SECURE_NO_WARNINGS

#include "FrameworkIPC/CSockets.h"
#include <sstream>
#include <iostream>

#ifdef WIN32
  #include <winsock2.h>        // For socket(), connect(), send(), and recv()
  #include <ws2tcpip.h>

  // Need to link with Ws2_32.lib
  #pragma comment (lib, "Ws2_32.lib")

  typedef int socklen_t;
  typedef char raw_type;       // Type used for raw data on this platform
#else
  #include <sys/types.h>       // For data types
  #include <sys/socket.h>      // For socket(), connect(), send(), and recv()
  #include <netdb.h>           // For gethostbyname()
  #include <arpa/inet.h>       // For inet_addr()
  #include <unistd.h>          // For close()
  #include <netinet/in.h>      // For sockaddr_in
  typedef void raw_type;       // Type used for raw data on this platform
#endif

#include <errno.h>             // For errno

using namespace std;

#ifdef WIN32
static bool initialized = false;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SocketException Code
/*
 ____             _        _   _____                    _   _
/ ___|  ___   ___| | _____| |_| ____|_  _____ ___ _ __ | |_(_) ___  _ __
\___ \ / _ \ / __| |/ / _ \ __|  _| \ \/ / __/ _ \ '_ \| __| |/ _ \| '_ \
 ___) | (_) | (__|   <  __/ |_| |___ >  < (_|  __/ |_) | |_| | (_) | | | |
|____/ \___/ \___|_|\_\___|\__|_____/_/\_\___\___| .__/ \__|_|\___/|_| |_|
                                                 |_|
*/
SocketException::SocketException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) {
  if (inclSysMsg) {
    userMessage.append(": ");
    userMessage.append(strerror(errno));
  }
}

SocketException::~SocketException() throw() {
}

/*
          _           _
__      _| |__   __ _| |_
\ \ /\ / / '_ \ / _` | __|
 \ V  V /| | | | (_| | |_
  \_/\_/ |_| |_|\__,_|\__|
*/
const char *SocketException::what() const throw() {
  return userMessage.c_str();
}

/*
  __ _ _ _    _       _     _
 / _(_) | |  / \   __| | __| |_ __
| |_| | | | / _ \ / _` |/ _` | '__|
|  _| | | |/ ___ \ (_| | (_| | |
|_| |_|_|_/_/   \_\__,_|\__,_|_|
*/
// Function to fill in address structure given an address and port
static void fillAddr(const string &address, unsigned short port, 
                     sockaddr_in &addr) {
  memset(&addr, 0, sizeof(addr));  // Zero out address structure
  addr.sin_family = AF_INET;       // Internet address

  hostent *host;  // Resolve name
  if ((host = gethostbyname(address.c_str())) == NULL) {
    // strerror() will not work for gethostbyname() and hstrerror() 
    // is supposedly obsolete
    throw SocketException("Failed to resolve name (gethostbyname())");
  }
  addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

  addr.sin_port = htons(port);     // Assign port in network byte order
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Socket Code
/*
 ____             _        _
/ ___|  ___   ___| | _____| |_
\___ \ / _ \ / __| |/ / _ \ __|
 ___) | (_) | (__|   <  __/ |_
|____/ \___/ \___|_|\_\___|\__|
*/
Socket::Socket(int type, int protocol) throw(SocketException) {
  #ifdef WIN32
    if (!initialized) {
      WORD wVersionRequested;
      WSADATA wsaData;

      wVersionRequested = MAKEWORD(2, 0);              // Request WinSock v2.0
      if (WSAStartup(wVersionRequested, &wsaData) != 0) {  // Load WinSock DLL
        throw SocketException("Unable to load WinSock DLL");
      }
      initialized = true;
    }
  #endif

  // Make a new socket
  if ((sockDesc = (int)socket(PF_INET, type, protocol)) < 0) {
    throw SocketException("Socket creation failed (socket())", true);
  }
}
/*
 ____             _        _
/ ___|  ___   ___| | _____| |_
\___ \ / _ \ / __| |/ / _ \ __|
 ___) | (_) | (__|   <  __/ |_
|____/ \___/ \___|_|\_\___|\__|
*/
Socket::Socket(int sockDesc) {
  this->sockDesc = sockDesc;
}

/*
 /\/|___             _        _
|/\/ ___|  ___   ___| | _____| |_
   \___ \ / _ \ / __| |/ / _ \ __|
    ___) | (_) | (__|   <  __/ |_
   |____/ \___/ \___|_|\_\___|\__|
*/
Socket::~Socket() {
  #ifdef WIN32
    ::closesocket(sockDesc);
  #else
    ::close(sockDesc);
  #endif
  sockDesc = -1;
}

/*
            _   _                    _    _       _     _
  __ _  ___| |_| |    ___   ___ __ _| |  / \   __| | __| |_ __ ___ ___ ___
 / _` |/ _ \ __| |   / _ \ / __/ _` | | / _ \ / _` |/ _` | '__/ _ Y __/ __|
| (_| |  __/ |_| |__| (_) | (_| (_| | |/ ___ \ (_| | (_| | | |  __|__ \__ \
 \__, |\___|\__|_____\___/ \___\__,_|_/_/   \_\__,_|\__,_|_|  \___|___/___/
 |___/
*/
string Socket::getLocalAddress() throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of local address failed (getsockname())", true);
  }
  return inet_ntoa(addr.sin_addr);
}

/*
            _   _                    _ ____            _
  __ _  ___| |_| |    ___   ___ __ _| |  _ \ ___  _ __| |_
 / _` |/ _ \ __| |   / _ \ / __/ _` | | |_) / _ \| '__| __|
| (_| |  __/ |_| |__| (_) | (_| (_| | |  __/ (_) | |  | |_
 \__, |\___|\__|_____\___/ \___\__,_|_|_|   \___/|_|   \__|
 |___/
*/
unsigned short Socket::getLocalPort() throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of local port failed (getsockname())", true);
  }
  return ntohs(addr.sin_port);
}

/*
          _   _                    _ ____            _
 ___  ___| |_| |    ___   ___ __ _| |  _ \ ___  _ __| |_
/ __|/ _ \ __| |   / _ \ / __/ _` | | |_) / _ \| '__| __|
\__ \  __/ |_| |__| (_) | (_| (_| | |  __/ (_) | |  | |_
|___/\___|\__|_____\___/ \___\__,_|_|_|   \___/|_|   \__|
*/
void Socket::setLocalPort(unsigned short localPort) throw(SocketException) {
  // Bind the socket to its port
  sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(localPort);

  if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) {
    throw SocketException("Set of local port failed (bind())", true);
  }
}

/*
          _   _                    _    _       _     _                     _              _ ____            _
 ___  ___| |_| |    ___   ___ __ _| |  / \   __| | __| |_ __ ___ ___ ___   / \   _ __   __| |  _ \ ___  _ __| |_
/ __|/ _ \ __| |   / _ \ / __/ _` | | / _ \ / _` |/ _` | '__/ _ Y __/ __| / _ \ | '_ \ / _` | |_) / _ \| '__| __|
\__ \  __/ |_| |__| (_) | (_| (_| | |/ ___ \ (_| | (_| | | |  __|__ \__ \/ ___ \| | | | (_| |  __/ (_) | |  | |_
|___/\___|\__|_____\___/ \___\__,_|_/_/   \_\__,_|\__,_|_|  \___|___/___/_/   \_\_| |_|\__,_|_|   \___/|_|   \__|
*/
void Socket::setLocalAddressAndPort(const string &localAddress,
    unsigned short localPort) throw(SocketException) {
  // Get the address of the requested host
  sockaddr_in localAddr;
  fillAddr(localAddress, localPort, localAddr);

  if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) {
    throw SocketException("Set of local address and port failed (bind())", true);
  }
}

/*
      _                  _   _
  ___| | ___  __ _ _ __ | | | |_ __
 / __| |/ _ \/ _` | '_ \| | | | '_ \
| (__| |  __/ (_| | | | | |_| | |_) |
 \___|_|\___|\__,_|_| |_|\___/| .__/
                              |_|
*/
void Socket::cleanUp() throw(SocketException) {
  #ifdef WIN32
    if (WSACleanup() != 0) {
      throw SocketException("WSACleanup() failed");
    }
  #endif
}

/*
                    _           ____                  _
 _ __ ___ ___  ___ | |_   _____/ ___|  ___ _ ____   _(_) ___ ___
| '__/ _ Y __|/ _ \| \ \ / / _ \___ \ / _ \ '__\ \ / / |/ __/ _ \
| | |  __|__ \ (_) | |\ V /  __/___) |  __/ |   \ V /| | (_|  __/
|_|  \___|___/\___/|_| \_/ \___|____/ \___|_|    \_/ |_|\___\___|
*/
unsigned short Socket::resolveService(const string &service,
                                      const string &protocol) {
  struct servent *serv;        /* Structure containing service information */

  if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
    return atoi(service.c_str());  /* Service is port number */
  else 
    return ntohs(serv->s_port);    /* Found port (network byte order) by name */
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CommunicatingSocket Code
/*
  ____                                      _           _   _             ____             _        _
 / ___|___  _ __ ___  _ __ ___  _   _ _ __ (_) ___ __ _| |_(_)_ __   __ _/ ___|  ___   ___| | _____| |_
| |   / _ \| '_ ` _ \| '_ ` _ \| | | | '_ \| |/ __/ _` | __| | '_ \ / _` \___ \ / _ \ / __| |/ / _ \ __|
| |__| (_) | | | | | | | | | | | |_| | | | | | (_| (_| | |_| | | | | (_| |___) | (_) | (__|   <  __/ |_
 \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|_|\___\__,_|\__|_|_| |_|\__, |____/ \___/ \___|_|\_\___|\__|
                                                                    |___/
*/
CommunicatingSocket::CommunicatingSocket(int type, int protocol)  
    throw(SocketException) : Socket(type, protocol) {
}

/*
  ____                                      _           _   _             ____             _        _
 / ___|___  _ __ ___  _ __ ___  _   _ _ __ (_) ___ __ _| |_(_)_ __   __ _/ ___|  ___   ___| | _____| |_
| |   / _ \| '_ ` _ \| '_ ` _ \| | | | '_ \| |/ __/ _` | __| | '_ \ / _` \___ \ / _ \ / __| |/ / _ \ __|
| |__| (_) | | | | | | | | | | | |_| | | | | | (_| (_| | |_| | | | | (_| |___) | (_) | (__|   <  __/ |_
 \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|_|\___\__,_|\__|_|_| |_|\__, |____/ \___/ \___|_|\_\___|\__|
                                                                    |___/
*/
CommunicatingSocket::CommunicatingSocket(int newConnSD) : Socket(newConnSD) {
}

/*
                                 _
  ___ ___  _ __  _ __   ___  ___| |_
 / __/ _ \| '_ \| '_ \ / _ \/ __| __|
| (_| (_) | | | | | | |  __/ (__| |_
 \___\___/|_| |_|_| |_|\___|\___|\__|
*/
void CommunicatingSocket::connect(const string &foreignAddress,
    unsigned short foreignPort) throw(SocketException) {
  // Get the address of the requested host
  sockaddr_in destAddr;
  fillAddr(foreignAddress, foreignPort, destAddr);

  // Try to connect to the given port
  if (::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr)) < 0) {
    throw SocketException("Connect failed (connect())", true);
  }
}

/*
                    _
 ___  ___ _ __   __| |
/ __|/ _ \ '_ \ / _` |
\__ \  __/ | | | (_| |
|___/\___|_| |_|\__,_|
*/
void CommunicatingSocket::send(const void *buffer, int bufferLen) 
    throw(SocketException) {
  if (::send(sockDesc, (raw_type *) buffer, bufferLen, 0) < 0) {
    throw SocketException("Send failed (send())", true);
  }
}

/*
 _ __ ___  _____   __
| '__/ _ \/ __\ \ / /
| | |  __/ (__ \ V /
|_|  \___|\___| \_/
*/
int CommunicatingSocket::recv(void *buffer, int bufferLen) 
    throw(SocketException) {
  int rtn;
  if ((rtn = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0)) < 0) {
    throw SocketException("Received failed (recv())", true);
  }

  return rtn;
}

/*
            _   _____              _                _       _     _
  __ _  ___| |_|  ___|__  _ __ ___(_) __ _ _ __    / \   __| | __| |_ __ ___ ___ ___
 / _` |/ _ \ __| |_ / _ \| '__/ _ \ |/ _` | '_ \  / _ \ / _` |/ _` | '__/ _ Y __/ __|
| (_| |  __/ |_|  _| (_) | | |  __/ | (_| | | | |/ ___ \ (_| | (_| | | |  __|__ \__ \
 \__, |\___|\__|_|  \___/|_|  \___|_|\__, |_| |_/_/   \_\__,_|\__,_|_|  \___|___/___/
 |___/                               |___/
*/
string CommunicatingSocket::getForeignAddress() 
    throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of foreign address failed (getpeername())", true);
  }
  return inet_ntoa(addr.sin_addr);
}

/*
            _   _____              _             ____            _
  __ _  ___| |_|  ___|__  _ __ ___(_) __ _ _ __ |  _ \ ___  _ __| |_
 / _` |/ _ \ __| |_ / _ \| '__/ _ \ |/ _` | '_ \| |_) / _ \| '__| __|
| (_| |  __/ |_|  _| (_) | | |  __/ | (_| | | | |  __/ (_) | |  | |_
 \__, |\___|\__|_|  \___/|_|  \___|_|\__, |_| |_|_|   \___/|_|   \__|
 |___/                               |___/
*/
unsigned short CommunicatingSocket::getForeignPort() throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of foreign port failed (getpeername())", true);
  }
  return ntohs(addr.sin_port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TCPSocket Code
/*
 _____ ____ ____  ____             _        _
|_   _/ ___|  _ \/ ___|  ___   ___| | _____| |_
  | || |   | |_) \___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/ ___) | (_) | (__|   <  __/ |_
  |_| \____|_|   |____/ \___/ \___|_|\_\___|\__|
*/
TCPSocket::TCPSocket() 
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, 
    IPPROTO_TCP) {
}

/*
 _____ ____ ____  ____             _        _
|_   _/ ___|  _ \/ ___|  ___   ___| | _____| |_
  | || |   | |_) \___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/ ___) | (_) | (__|   <  __/ |_
  |_| \____|_|   |____/ \___/ \___|_|\_\___|\__|
*/
TCPSocket::TCPSocket(const string &foreignAddress, unsigned short foreignPort)
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) {
  connect(foreignAddress, foreignPort);
}

/*
 _____ ____ ____  ____             _        _
|_   _/ ___|  _ \/ ___|  ___   ___| | _____| |_
  | || |   | |_) \___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/ ___) | (_) | (__|   <  __/ |_
  |_| \____|_|   |____/ \___/ \___|_|\_\___|\__|
*/
TCPSocket::TCPSocket(int newConnSD) : CommunicatingSocket(newConnSD) {
}

// TCPServerSocket Code
/*
 _____ ____ ____  ____                           ____             _        _
|_   _/ ___|  _ \/ ___|  ___ _ ____   _____ _ __/ ___|  ___   ___| | _____| |_
  | || |   | |_) \___ \ / _ \ '__\ \ / / _ \ '__\___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/ ___) |  __/ |   \ V /  __/ |   ___) | (_) | (__|   <  __/ |_
  |_| \____|_|   |____/ \___|_|    \_/ \___|_|  |____/ \___/ \___|_|\_\___|\__|
*/
TCPServerSocket::TCPServerSocket(unsigned short localPort, int queueLen) 
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP) {
  setLocalPort(localPort);
  setListen(queueLen);
}

/*
 _____ ____ ____  ____                           ____             _        _
|_   _/ ___|  _ \/ ___|  ___ _ ____   _____ _ __/ ___|  ___   ___| | _____| |_
  | || |   | |_) \___ \ / _ \ '__\ \ / / _ \ '__\___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/ ___) |  __/ |   \ V /  __/ |   ___) | (_) | (__|   <  __/ |_
  |_| \____|_|   |____/ \___|_|    \_/ \___|_|  |____/ \___/ \___|_|\_\___|\__|
*/
TCPServerSocket::TCPServerSocket(const string &localAddress, 
    unsigned short localPort, int queueLen) 
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP) {
  setLocalAddressAndPort(localAddress, localPort);
  setListen(queueLen);
}

/*
                          _
  __ _  ___ ___ ___ _ __ | |_
 / _` |/ __/ __/ _ \ '_ \| __|
| (_| | (_| (_|  __/ |_) | |_
 \__,_|\___\___\___| .__/ \__|
                   |_|
*/
TCPSocket *TCPServerSocket::accept() throw(SocketException) {
  int newConnSD;
  if ((newConnSD = (int)::accept(sockDesc, NULL, 0)) < 0) {
    throw SocketException("Accept failed (accept())", true);
  }

  return new TCPSocket(newConnSD);
}

/*
          _   _     _     _
 ___  ___| |_| |   (_)___| |_ ___ _ __
/ __|/ _ \ __| |   | / __| __/ _ \ '_ \
\__ \  __/ |_| |___| \__ \ ||  __/ | | |
|___/\___|\__|_____|_|___/\__\___|_| |_|
*/
void TCPServerSocket::setListen(int queueLen) throw(SocketException) {
  if (listen(sockDesc, queueLen) < 0) {
    throw SocketException("Set listening socket failed (listen())", true);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// UDPSocket Code
/*
 _   _ ____  ____  ____             _        _
| | | |  _ \|  _ \/ ___|  ___   ___| | _____| |_
| | | | | | | |_) \___ \ / _ \ / __| |/ / _ \ __|
| |_| | |_| |  __/ ___) | (_) | (__|   <  __/ |_
 \___/|____/|_|   |____/ \___/ \___|_|\_\___|\__|
*/
UDPSocket::UDPSocket() throw(SocketException) : CommunicatingSocket(SOCK_DGRAM,
    IPPROTO_UDP) {
  setBroadcast();
}

/*
 _   _ ____  ____  ____             _        _
| | | |  _ \|  _ \/ ___|  ___   ___| | _____| |_
| | | | | | | |_) \___ \ / _ \ / __| |/ / _ \ __|
| |_| | |_| |  __/ ___) | (_) | (__|   <  __/ |_
 \___/|____/|_|   |____/ \___/ \___|_|\_\___|\__|
*/
UDPSocket::UDPSocket(unsigned short localPort)  throw(SocketException) : 
    CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
  setLocalPort(localPort);
  setBroadcast();
}

    /*
 _   _ ____  ____  ____             _        _
| | | |  _ \|  _ \/ ___|  ___   ___| | _____| |_
| | | | | | | |_) \___ \ / _ \ / __| |/ / _ \ __|
| |_| | |_| |  __/ ___) | (_) | (__|   <  __/ |_
 \___/|____/|_|   |____/ \___/ \___|_|\_\___|\__|
*/
UDPSocket::UDPSocket(const string &localAddress, unsigned short localPort) 
     throw(SocketException) : CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
  setLocalAddressAndPort(localAddress, localPort);
  setBroadcast();
}

/*
          _   ____                      _               _
 ___  ___| |_| __ ) _ __ ___   __ _  __| | ___ __ _ ___| |_
/ __|/ _ \ __|  _ \| '__/ _ \ / _` |/ _` |/ __/ _` / __| __|
\__ \  __/ |_| |_) | | | (_) | (_| | (_| | (_| (_| \__ \ |_
|___/\___|\__|____/|_|  \___/ \__,_|\__,_|\___\__,_|___/\__|
*/
void UDPSocket::setBroadcast() {
  // If this fails, we'll hear about it when we try to send.  This will allow 
  // system that cannot broadcast to continue if they don't plan to broadcast
  int broadcastPermission = 1;
  setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST, 
             (raw_type *) &broadcastPermission, sizeof(broadcastPermission));
}

/*
     _ _                                     _
  __| (_)___  ___ ___  _ __  _ __   ___  ___| |_
 / _` | / __|/ __/ _ \| '_ \| '_ \ / _ \/ __| __|
| (_| | \__ \ (_| (_) | | | | | | |  __/ (__| |_
 \__,_|_|___/\___\___/|_| |_|_| |_|\___|\___|\__|
*/
void UDPSocket::disconnect() throw(SocketException) {
  sockaddr_in nullAddr;
  memset(&nullAddr, 0, sizeof(nullAddr));
  nullAddr.sin_family = AF_UNSPEC;

  // Try to disconnect
  if (::connect(sockDesc, (sockaddr *) &nullAddr, sizeof(nullAddr)) < 0) {
   #ifdef WIN32
    if (errno != WSAEAFNOSUPPORT) {
   #else
    if (errno != EAFNOSUPPORT) {
   #endif
      throw SocketException("Disconnect failed (connect())", true);
    }
  }
}

/*
                    _ _____
 ___  ___ _ __   __| |_   _|__
/ __|/ _ \ '_ \ / _` | | |/ _ \
\__ \  __/ | | | (_| | | | (_) |
|___/\___|_| |_|\__,_| |_|\___/
*/
void UDPSocket::sendTo(const void *buffer, int bufferLen, 
    const string &foreignAddress, unsigned short foreignPort) 
    throw(SocketException) {
  sockaddr_in destAddr;
  fillAddr(foreignAddress, foreignPort, destAddr);

  // Write out the whole buffer as a single message.
  if (sendto(sockDesc, (raw_type *) buffer, bufferLen, 0,
             (sockaddr *) &destAddr, sizeof(destAddr)) != bufferLen) {
    throw SocketException("Send failed (sendto())", true);
  }
}

void UDPSocket::sendBroadCast(const void *buffer, int bufferLen, unsigned short foreignPort) throw(SocketException) {

    sockaddr_in destAddr;
    fillAddr("255.255.255.255", foreignPort, destAddr);

    // Write out the whole buffer as a single message.
    if (sendto(sockDesc, (raw_type *) buffer, bufferLen, 0, (sockaddr *) &destAddr, sizeof(destAddr)) != bufferLen) {
        throw SocketException("Send failed (sendto())", true);
    }
}

/*
                     _____
 _ __ ___  _____   _|  ___| __ ___  _ __ ___
| '__/ _ \/ __\ \ / / |_ | '__/ _ \| '_ ` _ \
| | |  __/ (__ \ V /|  _|| | | (_) | | | | | |
|_|  \___|\___| \_/ |_|  |_|  \___/|_| |_| |_|
*/
int UDPSocket::recvFrom(void *buffer, int bufferLen, string &sourceAddress,
    unsigned short &sourcePort) throw(SocketException) {
  sockaddr_in clntAddr;
  socklen_t addrLen = sizeof(clntAddr);
  int rtn;
  if ((rtn = recvfrom(sockDesc, (raw_type *) buffer, bufferLen, 0, 
                      (sockaddr *) &clntAddr, (socklen_t *) &addrLen)) < 0) {
    stringstream s;
    s.str("");
    s << "Receive failed (recvfrom(" << WSAGetLastError() << "))";
    throw SocketException(s.str(), true);
  }
  sourceAddress = inet_ntoa(clntAddr.sin_addr);
  sourcePort = ntohs(clntAddr.sin_port);

  return rtn;
}

/*
          _   __  __       _ _   _               _  _____ _____ _
 ___  ___| |_|  \/  |_   _| | |_(_) ___ __ _ ___| ||_   _|_   _| |
/ __|/ _ \ __| |\/| | | | | | __| |/ __/ _` / __| __|| |   | | | |
\__ \  __/ |_| |  | | |_| | | |_| | (_| (_| \__ \ |_ | |   | | | |___
|___/\___|\__|_|  |_|\__,_|_|\__|_|\___\__,_|___/\__||_|   |_| |_____|
*/
void UDPSocket::setMulticastTTL(unsigned char multicastTTL) throw(SocketException) {
  if (setsockopt(sockDesc, IPPROTO_IP, IP_MULTICAST_TTL, 
                 (raw_type *) &multicastTTL, sizeof(multicastTTL)) < 0) {
    throw SocketException("Multicast TTL set failed (setsockopt())", true);
  }
}

/*
   _       _        ____
  (_) ___ (_)_ __  / ___|_ __ ___  _   _ _ __
  | |/ _ \| | '_ \| |  _| '__/ _ \| | | | '_ \
  | | (_) | | | | | |_| | | | (_) | |_| | |_) |
 _/ |\___/|_|_| |_|\____|_|  \___/ \__,_| .__/
|__/                                    |_|
*/
void UDPSocket::joinGroup(unsigned int &port) throw(SocketException) {

    // bind the socket
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    bool binded = false;
    
    for(unsigned int i=0; i<1024; i++) {
        service.sin_port = port + i;
        if (SOCKET_ERROR != bind(sockDesc, (SOCKADDR *) &service, sizeof (service))) {
            binded = true;
            break;
        }
    }

    if (!binded) {
        stringstream s;
        s.str("");
        s << "Error binding socket (" << WSAGetLastError() << "))";
        throw SocketException(s.str(), true);
    }
    else {
        
        port = service.sin_port;

        //multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
        //multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
        ////int res = setsockopt(sockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP, (raw_type *) &multicastRequest,  sizeof(multicastRequest));
        //int res = setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST, (raw_type *) &multicastRequest,  sizeof(multicastRequest));
        //if (res < 0) {
        //    stringstream s;
        //    s.str("");
        //    s << "Multicast group join failed (setsockopt(" << WSAGetLastError() << "))";
        //    throw SocketException(s.str(), true);
        //} else
        //    std::cout << "MULTICAST group joined" << endl;
    }
}


/*
 _                       ____
| | ___  __ ___   _____ / ___|_ __ ___  _   _ _ __
| |/ _ \/ _` \ \ / / _ \ |  _| '__/ _ \| | | | '_ \
| |  __/ (_| |\ V /  __/ |_| | | | (_) | |_| | |_) |
|_|\___|\__,_| \_/ \___|\____|_|  \___/ \__,_| .__/
                                             |_|
*/
void UDPSocket::leaveGroup(const string &multicastGroup) throw(SocketException) {
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
                 (raw_type *) &multicastRequest, 
                 sizeof(multicastRequest)) < 0) {
    throw SocketException("Multicast group leave failed (setsockopt())", true);
  }
}

#undef _CRT_SECURE_NO_WARNINGS