// System
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

// Framework
#include "FrameworkIPC/CIPC.h"
#include "FrameworkCore/CTime.h"
#include <sstream>

//#define COUT_TRACE

// namespaces
using namespace FrameworkCore;
using namespace FrameworkIPC;
using namespace std;

// Channel defines
#define CHANNEL_FIELD_NAME      "0x_channel_name"
#define CHANNEL_FIELD_DATA      "0x_data"
#define CHANNEL_FIELD_TIME      "0x_timestamp"
#define CHANNEL_LOCALHOST_NAME  "0x_localhost"

// default socket options
#define DEFAULT_PORT 4096
#define IP_MULTICAST 0xE0000105
#define MULTICAST_ADDR "224.5.6.7"
#define MAPSIZE     1024*1024

// default receiving datagram buffer and timeout
#define BUFFER_SIZE 8192
#define DEFAULT_TIMEOUT_ms 1000


int CIPC::_counter = 0;

/*
  ____ ___ ____   ____
 / ___|_ _|  _ \ / ___|
| |    | || |_) | |
| |___ | ||  __/| |___
 \____|___|_|    \____|
*/
CIPC::CIPC() : _joined(false) {
    _ID = _counter++;
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | CIPC() : " <<  endl;
#endif
    SetErrorClassPrefix("CIPC");
    // clear the channels map
    _channelsData.clear();
    // create the mapped file name
    char aux[1024];
    GetTempPath(1024, aux);
    _localIP = "";
}

/*
 /\/|___ ___ ____   ____
|/\/ ___|_ _|  _ \ / ___|
  | |    | || |_) | |
  | |___ | ||  __/| |___
   \____|___|_|    \____|
*/
CIPC::~CIPC() {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | ~CIPC() : _joined = " << _joined << endl;
#endif
    if (_joined) Leave();
}

/*
            _   _                    _ ___ ____
  __ _  ___| |_| |    ___   ___ __ _| |_ _|  _ \
 / _` |/ _ \ __| |   / _ \ / __/ _` | || || |_) |
| (_| |  __/ |_| |__| (_) | (_| (_| | || ||  __/
 \__, |\___|\__|_____\___/ \___\__,_|_|___|_|
 |___/
*/
void CIPC::getLocalIP(string &localIP) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | getLocalIP... " << endl;
#endif
    // GET THE LOCAL IP
    char szHostName[128] = "";
    ::gethostname(szHostName, sizeof(szHostName));
    struct sockaddr_in localAddress;
    struct hostent     *pHost        = 0;
    pHost = ::gethostbyname(szHostName);
    char aszIPAddresses[10][16]; // maximum of ten IP addresses

    for(int iCnt = 0; ((pHost->h_addr_list[iCnt]) && (iCnt < 10)); ++iCnt)
    {
      memcpy(&localAddress.sin_addr, pHost->h_addr_list[iCnt], pHost->h_length);
      strcpy_s(aszIPAddresses[iCnt], 16, inet_ntoa(localAddress.sin_addr));
    }
    localIP.assign(aszIPAddresses[0]);
}

/*
 __  __             ____        _         ___  _     _           _
|  \/  | __ _ _ __ |  _ \  __ _| |_ __ _ / _ \| |__ (_) ___  ___| |_
| |\/| |/ _` | '_ \| | | |/ _` | __/ _` | | | | '_ \| |/ _ \/ __| __|
| |  | | (_| | |_) | |_| | (_| | || (_| | |_| | |_) | |  __/ (__| |_
|_|  |_|\__,_| .__/|____/ \__,_|\__\__,_|\___/|_.__// |\___|\___|\__|
             |_|                                  |__/
*/
void CIPC::MapDataObject(std::string objectName) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | MapDataObject: " << objectName << endl;
#endif
    //// ---------------------------
    ////     CREATE MAPPED FILE
    //// ---------------------------
    //HANDLE hFile, hMapFile;
    //LPVOID lpMapAddress;

    //// Create the test file. Open it "Create Always" to overwrite any
    //// existing file. The data is re-created below
    //hFile = CreateFile( (_tempPath + objectName).c_str(),
    //                     GENERIC_READ | GENERIC_WRITE,
    //                     0,
    //                     NULL,
    //                     CREATE_ALWAYS,
    //                     FILE_ATTRIBUTE_NORMAL,
    //                     NULL);
    //// check result
    //if (hFile == INVALID_HANDLE_VALUE)
    //    return;

    //// Create a file mapping object for the file
    //// Note that it is a good idea to ensure the file size is not zero
    //hMapFile = CreateFileMapping( hFile,          // current file handle
    //            NULL,           // default security
    //            PAGE_READWRITE, // read/write permission
    //            0,              // size of mapping object, high
    //            MAPSIZE,        // size of mapping object, low
    //            NULL);          // name of mapping object
    //// check result
    //if (hMapFile == NULL)
    //    return;

    //// get the mapping pointer
    //lpMapAddress = MapViewOfFile(hMapFile,            // handle to
    //                                    // mapping object
    //               FILE_MAP_ALL_ACCESS, // read/write
    //               0,                   // high-order 32
    //                                    // bits of file
    //                                    // offset
    //               0,                   // low-order 32
    //                                    // bits of file
    //                                    // offset
    //               MAPSIZE);            // number of bytes
    //// check result
    //if (lpMapAddress == NULL)
    //    return;

    //// mapping done, store mapping data
    //_mappedFile[objectName] = hFile;
    //_mappedView[objectName] = hMapFile;
    //_mappedData[objectName] = lpMapAddress;

}
/*
     _       _
    | | ___ (_)_ __
 _  | |/ _ \| | '_ \
| |_| | (_) | | | | |
 \___/ \___/|_|_| |_|

*/
bool CIPC::Join() {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | Join" << endl;
#endif
    //// avoid to reconnect again
    if (!_joined) {
        synch.InstanceLock("CIPC::Joining");
        // aux
        int res;
        
        // Initialized WSA
        WORD wVersionRequested;
        WSADATA wsaData;
        wVersionRequested = MAKEWORD(2, 2);
        res = WSAStartup(wVersionRequested, &wsaData);
        if (res != 0) {
            PushError("Join", "Error in WSAStartup()");
            synch.InstanceUnlock("CIPC::Joining");
            return false;
        }

        //// GET THE LOCAL IP
        char szHostName[128] = "";
        ::gethostname(szHostName, sizeof(szHostName));
        _localHost.assign(szHostName);
        //struct hostent     *pHost        = 0;
        //pHost = ::gethostbyname(szHostName);
        //char aszIPAddresses[10][16]; // maximum of ten IP addresses

        //for(int iCnt = 0; ((pHost->h_addr_list[iCnt]) && (iCnt < 10)); ++iCnt)
        //{
        //  memcpy(&localAddress.sin_addr, pHost->h_addr_list[iCnt], pHost->h_length);
        //  strcpy_s(aszIPAddresses[iCnt], 16, inet_ntoa(localAddress.sin_addr));
        //}
        getLocalIP(_localIP);
        const char *localIP = _localIP.c_str();

        // ----------------------------------------------
        // ---------------- BUILD SOCKET ----------------
        // ----------------------------------------------
        _st = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        _sr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        // ---------------------------------------------
        // client and server can be on the same computer
        // ---------------------------------------------
        int bTrue = 1;
        res = setsockopt( _st, SOL_SOCKET, SO_REUSEADDR, (const char *)&bTrue, sizeof(bTrue));
        if ( res == -1) {
            stringstream out;
            out.str("");
            out << "SO_REUSEADDR transmission error : " << WSAGetLastError();
            PushError("Join", out.str());
        }

        res = setsockopt( _sr, SOL_SOCKET, SO_REUSEADDR, (const char *)&bTrue, sizeof(bTrue));
        if ( res == -1) {
            stringstream out;
            out.str("");
            out << "SO_REUSEADDR reception error : " << WSAGetLastError() << endl;
            PushError("Join", out.str());
        }

        // -------------------------
        // SET THE RECEIVING TIMEOUT
        // -------------------------
        timeval tv;
	    tv.tv_sec = DEFAULT_TIMEOUT_ms / 1000 ;
	    tv.tv_usec = ( DEFAULT_TIMEOUT_ms % 1000) ; //* 1000  ;
        res = setsockopt (_sr, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof timeval);
        if ( res == -1) {
            stringstream out;
            out.str("");
            out << "Error setting receiving socket timeout: " << WSAGetLastError() << endl;
            PushError("Join", out.str());
        }   

        // ---------------------------------------
        //      BIND THE TRANSMISSION SOCKET
        // ---------------------------------------
        SOCKADDR_IN             localif;
        localif.sin_family      = AF_INET;
        localif.sin_port        = htons(4096);
        localif.sin_addr.s_addr = inet_addr(localIP);
        
        bool binded = false;
        for(unsigned int i=0; i<1024; i++) {
            localif.sin_port = 4096 + i;
            if (SOCKET_ERROR != bind(_st, (SOCKADDR *) &localif, sizeof (localif))) {
                binded = true;
                break;
                }
            }
        if (!binded) {
            stringstream out;
            out.str("");
            out << "error binding socket at address " << localIP << ", error: " << WSAGetLastError() << endl;
            PushError("Join", out.str());
            synch.InstanceUnlock("CIPC::Joining");
            return false;
        }
        _transmitPort = localif.sin_port;

        // ---------------------------------------
        //       BIND THE RECEPTION SOCKET
        // ---------------------------------------
        localif.sin_family      = AF_INET;
        localif.sin_port        = htons(4096);
        localif.sin_addr.s_addr = inet_addr(localIP);

        binded = false;
        for(unsigned int i=0; i<1024; i++) {
            localif.sin_port = 4096 + i;
            if (SOCKET_ERROR != bind(_sr, (SOCKADDR *) &localif, sizeof (localif))) {
                binded = true;
                break;
                }
            }
        if (!binded) {
            stringstream out;
            out.str("");
            out << "error binding socket at address " << localIP << ", error: " << WSAGetLastError() << endl;
            PushError("Join", out.str());
            synch.InstanceUnlock("CIPC::Joining");
            return false;
        }
        _receivePort = localif.sin_port;

        // -----------------------------------
        // SET THE TTL FOR TRANSMITTION SOCKET
        // -----------------------------------
        char TTL = 32 ; // Restrict to our school network, for example
        res = setsockopt(_st, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&TTL, sizeof(TTL));
        if ( res == -1) {
            stringstream out;
            out.str("");
            out << "Error setting multicast TTL : " << WSAGetLastError() << endl;
            PushError("Join", out.str());
        }

        // -----------------------------------
        // SET THE TTL FOR TRANSMITTION SOCKET
        // -----------------------------------
        unsigned long addr = inet_addr(localIP);
        res = setsockopt(_st , IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr, sizeof(addr));
        if ( res == -1) {
            stringstream out;
            out.str("");
            out << "Error setting IP_MULTICAST_IF: " << WSAGetLastError() << endl;
            PushError("Join", out.str());
        }

        // ----------------------------------------
        // ENABLE MULTICAST TO THE RECEIVING SOCKET
        // ----------------------------------------
        ip_mreq_source   mreq;
        mreq.imr_multiaddr.s_addr   = inet_addr(MULTICAST_ADDR);
        mreq.imr_interface.s_addr   = inet_addr(localIP);
        mreq.imr_sourceaddr.s_addr  = inet_addr(localIP);

        res = setsockopt(_sr, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) (& mreq), sizeof(ip_mreq_source));
        if ( res == -1) {
            stringstream out;
            out.str("");
            out << "Error setting multicast reception: " << WSAGetLastError() << endl;
            PushError("Join", out.str());
        }    

    }

    if (!HasError()) {
        // ---------------------------
        //     GROUP JOINED FLAG
        // ---------------------------
        _joined = true;

        // ---------------------------
        //   LAUNCH RECEIVING THREAD
        // ---------------------------
        ThreadRun("MULTICAST_RECEIVING", NULL);
        //ThreadRun("FILEMAPPING_RECEIVING", NULL);
    }

    synch.InstanceUnlock("CIPC::Joining");
    // done
    return _joined;
}

/*
__        __    _ _   ____        _        _____                     ____ _                            _
\ \      / /_ _(_) |_|  _ \  __ _| |_ __ _|  ___| __ ___  _ __ ___  / ___| |__   __ _ _ __  _ __   ___| |
 \ \ /\ / / _` | | __| | | |/ _` | __/ _` | |_ | '__/ _ \| '_ ` _ \| |   | '_ \ / _` | '_ \| '_ \ / _ \ |
  \ V  V / (_| | | |_| |_| | (_| | || (_| |  _|| | | (_) | | | | | | |___| | | | (_| | | | | | | |  __/ |
   \_/\_/ \__,_|_|\__|____/ \__,_|\__\__,_|_|  |_|  \___/|_| |_| |_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/
bool CIPC::WaitDataFromChannel(std::string channel_name, FrameworkCore::CGenericType &data, unsigned int timeout_ms) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | WaitDataFromChannel : " << channel_name << endl;
#endif
    // check that channel has been already opened
    if (!_joined) {
        PushError("ReadChannel", "Channel not yet joined");
        return false;
    }

    // wait the receiving event
    if (_receptionEvents.WaitEvent(channel_name, timeout_ms)) {
        synch.InstanceLock("ChannelDataSynch");
        data = _channelsData[channel_name];
        synch.InstanceUnlock("ChannelDataSynch");
        return true;
    } else
        return false;
    
}

/*
__        __    _ _       ____        _       _____      ____ _                            _
\ \      / / __(_) |_ ___|  _ \  __ _| |_ __ |_   _|__  / ___| |__   __ _ _ __  _ __   ___| |
 \ \ /\ / / '__| | __/ _ \ | | |/ _` | __/ _` || |/ _ \| |   | '_ \ / _` | '_ \| '_ \ / _ \ |
  \ V  V /| |  | | ||  __/ |_| | (_| | || (_| || | (_) | |___| | | | (_| | | | | | | |  __/ |
   \_/\_/ |_|  |_|\__\___|____/ \__,_|\__\__,_||_|\___/ \____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/
bool CIPC::WriteDataToChannel(std::string channel_name, FrameworkCore::CGenericType &data) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | WriteDataToChannel " << channel_name << endl;
#endif
    synch.InstanceLock("CIPC::WriteChannel");

    // check that channel has been already opened
    if (!_joined) {
        
        PushError("WriteChannel", "Channel not yet joined");
        synch.InstanceUnlock("CIPC::WriteChannel");
        return false;
    }

    // create data string
    //static CGenericType sendobj;
    CGenericType sendobj;
    sendobj = 0;
    sendobj % CHANNEL_LOCALHOST_NAME    = _localHost;
    sendobj % CHANNEL_FIELD_NAME        = channel_name;
    sendobj % CHANNEL_FIELD_DATA        = data;
    std::string dump = sendobj.Dump();

    // transmit the data to the multicast group
    sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_port = _transmitPort;
    to.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    int res = sendto(_st, dump.c_str(), (int)dump.length(), 0, (sockaddr *)&to, sizeof(sockaddr_in));

    synch.InstanceUnlock("CIPC::WriteChannel");

    // write data into filemapping memory

    return res != -1;
}

/*
  ____ _                   ____ _                            _ ____         __  __
 / ___| | ___  __ _ _ __  / ___| |__   __ _ _ __  _ __   ___| | __ ) _   _ / _|/ _| ___ _ __
| |   | |/ _ \/ _` | '_ \| |   | '_ \ / _` | '_ \| '_ \ / _ \ |  _ \| | | | |_| |_ / _ \ '__|
| |___| |  __/ (_| | | | | |___| | | | (_| | | | | | | |  __/ | |_) | |_| |  _|  _|  __/ |
 \____|_|\___|\__,_|_| |_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|____/ \__,_|_| |_|  \___|_|
*/
void CIPC::CleanChannelBuffer(std::string channel_name) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | CleanChannelBuffer" << endl;
#endif
    synch.InstanceLock("ChannelDataSynch");
    // check if channel isn't already synchronized
    _tChannelData::iterator it = _channelsData.find(channel_name);
    if (it != _channelsData.end()) {
        // clean buffered data if present
        it->second = 0;
    }
    synch.InstanceUnlock("ChannelDataSynch");
}
/*
 _
| |    ___  __ ___   _____
| |   / _ \/ _` \ \ / / _ \
| |__|  __/ (_| |\ V /  __/
|_____\___|\__,_| \_/ \___|
*/
bool CIPC::Leave() {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | Leave" << endl;
#endif

    //ThreadTerminate("FILEMAPPING_RECEIVING");
    DisableAllDataSynchronization();

    if (!_joined) return true;
    
    // ---------------------------
    //  stop the receiving thread
    // ---------------------------
    ThreadTerminate("MULTICAST_RECEIVING");
    // ---------------------------
    //    leave multicast group
    // ---------------------------
    ip_mreq_source   mreq;
    mreq.imr_multiaddr.s_addr   = inet_addr(MULTICAST_ADDR);
    mreq.imr_interface.s_addr   = inet_addr(_localIP.c_str());
    mreq.imr_sourceaddr.s_addr  = inet_addr(_localIP.c_str());
    int res = setsockopt(_sr, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, (char *) (& mreq), sizeof(ip_mreq_source));
    
    if (res == -1) {
        stringstream msg;
        msg.str("");
        msg << "Error leaving multicast group. Error code : " << WSAGetLastError();
        PushError("Leave", msg.str());
    }

    // ---------------------------
    //    close all sockets
    // ---------------------------
    closesocket(_st);
    closesocket(_sr);

    // -------------------------------
    //    close mapping view and file
    // -------------------------------
    //std::map<std::string, LPVOID>::iterator it_map;
    //for (it_map = _mappedData.begin(); it_map != _mappedData.end(); it_map++) {
    //    UnmapViewOfFile(_mappedData[it_map->first]);
    //    CloseHandle(_mappedView[it_map->first]);
    //    CloseHandle(_mappedFile[it_map->first]);
    //}
    //_mappedData.clear();
    //_mappedView.clear();
    //_mappedFile.clear();

    // DONE
    _joined = false;
    return true;
}

/*
 _____             _     _      ____        _        ____                   _                     _          _   _
| ____|_ __   __ _| |__ | | ___|  _ \  __ _| |_ __ _/ ___| _   _ _ __   ___| |__  _ __ ___  _ __ (_)______ _| |_(_) ___  _ __
|  _| | '_ \ / _` | '_ \| |/ _ \ | | |/ _` | __/ _` \___ \| | | | '_ \ / __| '_ \| '__/ _ \| '_ \| |_  / _` | __| |/ _ \| '_ \
| |___| | | | (_| | |_) | |  __/ |_| | (_| | || (_| |___) | |_| | | | | (__| | | | | | (_) | | | | |/ / (_| | |_| | (_) | | | |
|_____|_| |_|\__,_|_.__/|_|\___|____/ \__,_|\__\__,_|____/ \__, |_| |_|\___|_| |_|_|  \___/|_| |_|_/___\__,_|\__|_|\___/|_| |_|
                                                           |___/
*/
void CIPC::EnableDataSynchronization(std::string channel_name) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | EnableDataSynchronization - " << channel_name << endl;
#endif
    synch.InstanceLock("ChannelDataSynch");
    // check if channel is already synchronized
    if (_channelsData.find(channel_name) != _channelsData.end()) {
        synch.InstanceUnlock("ChannelDataSynch");     
        return;
    }
    // add channel name to the synchronization list
    _channelsData[channel_name] = CGenericType();
    synch.InstanceUnlock("ChannelDataSynch");
}

/*
 ____  _           _     _      ____        _        ____                   _                     _          _   _
|  _ \(_)___  __ _| |__ | | ___|  _ \  __ _| |_ __ _/ ___| _   _ _ __   ___| |__  _ __ ___  _ __ (_)______ _| |_(_) ___  _ __
| | | | / __|/ _` | '_ \| |/ _ \ | | |/ _` | __/ _` \___ \| | | | '_ \ / __| '_ \| '__/ _ \| '_ \| |_  / _` | __| |/ _ \| '_ \
| |_| | \__ \ (_| | |_) | |  __/ |_| | (_| | || (_| |___) | |_| | | | | (__| | | | | | (_) | | | | |/ / (_| | |_| | (_) | | | |
|____/|_|___/\__,_|_.__/|_|\___|____/ \__,_|\__\__,_|____/ \__, |_| |_|\___|_| |_|_|  \___/|_| |_|_/___\__,_|\__|_|\___/|_| |_|
                                                           |___/
*/
void CIPC::DisableDataSynchronization(std::string channel_name) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | DisableDataSynchronization" << endl;
#endif
    synch.InstanceLock("ChannelDataSynch");
    // check if channel isn't already synchronized
    if (_channelsData.find(channel_name) == _channelsData.end()) {
        synch.InstanceUnlock("ChannelDataSynch");
        return;
    }
    // remove channel name from the synchronization list
    _channelsData.erase(channel_name);
    synch.InstanceUnlock("ChannelDataSynch");
}

/*
 ____  _           _     _         _    _ _ ____        _        ____                   _                     _          _   _
|  _ \(_)___  __ _| |__ | | ___   / \  | | |  _ \  __ _| |_ __ _/ ___| _   _ _ __   ___| |__  _ __ ___  _ __ (_)______ _| |_(_) ___  _ __
| | | | / __|/ _` | '_ \| |/ _ \ / _ \ | | | | | |/ _` | __/ _` \___ \| | | | '_ \ / __| '_ \| '__/ _ \| '_ \| |_  / _` | __| |/ _ \| '_ \
| |_| | \__ \ (_| | |_) | |  __// ___ \| | | |_| | (_| | || (_| |___) | |_| | | | | (__| | | | | | (_) | | | | |/ / (_| | |_| | (_) | | | |
|____/|_|___/\__,_|_.__/|_|\___/_/   \_\_|_|____/ \__,_|\__\__,_|____/ \__, |_| |_|\___|_| |_|_|  \___/|_| |_|_/___\__,_|\__|_|\___/|_| |_|
                                                                       |___/
*/
void CIPC::DisableAllDataSynchronization() {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | DisableAllDataSynchronization" << endl;
#endif
    synch.InstanceLock("ChannelDataSynch");
    _channelsData.clear();
    synch.InstanceUnlock("ChannelDataSynch");
}

/*
  ____      _    ____ _                            _ ____        _
 / ___| ___| |_ / ___| |__   __ _ _ __  _ __   ___| |  _ \  __ _| |_ __ _
| |  _ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ | | | |/ _` | __/ _` |
| |_| |  __/ |_| |___| | | | (_| | | | | | | |  __/ | |_| | (_| | || (_| |
 \____|\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|____/ \__,_|\__\__,_|
*/
void CIPC::GetChannelData(std::string channel_name, CGenericType &data) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | GetChannelData : " << channel_name << endl;
#endif
    synch.InstanceLock("ChannelDataSynch");
    // check if channel isn't already synchronized
    _tChannelData::iterator it = _channelsData.find(channel_name);
    if (it == _channelsData.end()) {
        data = 0;
        synch.InstanceUnlock("ChannelDataSynch");
        return;
    }
    // get synchronized object
    data = it->second;
    synch.InstanceUnlock("ChannelDataSynch");
}

/*
__        __    _ _   _   _           _   ____               _               _ ____        _
\ \      / /_ _(_) |_| \ | | _____  _| |_|  _ \ ___  ___ ___(_)_   _____  __| |  _ \  __ _| |_ __ _
 \ \ /\ / / _` | | __|  \| |/ _ \ \/ / __| |_) / _ \/ __/ _ \ \ \ / / _ \/ _` | | | |/ _` | __/ _` |
  \ V  V / (_| | | |_| |\  |  __/>  <| |_|  _ <  __/ (_|  __/ |\ V /  __/ (_| | |_| | (_| | || (_| |
   \_/\_/ \__,_|_|\__|_| \_|\___/_/\_\\__|_| \_\___|\___\___|_| \_/ \___|\__,_|____/ \__,_|\__\__,_|
*/
void CIPC::WaitNextReceivedData(FrameworkCore::CGenericType &data) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | WaitNextReceivedData" << endl;
#endif

    _receptionEvents.WaitEvent("CIPC::ThreadedMethod::data_received");
    synch.InstanceLock("ChannelDataSynch");
    data = _channelsData["LAST"];
    synch.InstanceUnlock("ChannelDataSynch");
}

/*
__        __    _ _   _   _           _   ____               _               _ ____        _
\ \      / /_ _(_) |_| \ | | _____  _| |_|  _ \ ___  ___ ___(_)_   _____  __| |  _ \  __ _| |_ __ _
 \ \ /\ / / _` | | __|  \| |/ _ \ \/ / __| |_) / _ \/ __/ _ \ \ \ / / _ \/ _` | | | |/ _` | __/ _` |
  \ V  V / (_| | | |_| |\  |  __/>  <| |_|  _ <  __/ (_|  __/ |\ V /  __/ (_| | |_| | (_| | || (_| |
   \_/\_/ \__,_|_|\__|_| \_|\___/_/\_\\__|_| \_\___|\___\___|_| \_/ \___|\__,_|____/ \__,_|\__\__,_|
   */
bool CIPC::WaitNextReceivedData(FrameworkCore::CGenericType &data, unsigned int timeout_ms) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | WaitNextReceivedData" << endl;
#endif
    if (_receptionEvents.WaitEvent("CIPC::ThreadedMethod::data_received", timeout_ms)) {
        synch.InstanceLock("ChannelDataSynch");
        data = _channelsData["LAST"];
        synch.InstanceUnlock("ChannelDataSynch");
        return true;
    }
    else
        return false;
}

/*
 ____  _           _  ____ _                            _ ____               _                 ____                                          _
| __ )(_)_ __   __| |/ ___| |__   __ _ _ __  _ __   ___| |  _ \ ___  ___ ___(_)_   _____ _ __ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
|  _ \| | '_ \ / _` | |   | '_ \ / _` | '_ \| '_ \ / _ \ | |_) / _ \/ __/ _ \ \ \ / / _ \ '__| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
| |_) | | | | | (_| | |___| | | | (_| | | | | | | |  __/ |  _ <  __/ (_|  __/ |\ V /  __/ |  | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|____/|_|_| |_|\__,_|\____|_| |_|\__,_|_| |_|_| |_|\___|_|_| \_\___|\___\___|_| \_/ \___|_|   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
*/
bool CIPC::BindChannelReceiverCommand(std::string channel, std::string command_name) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | BindChannelReceiverCommand" << endl;
#endif
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
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | GetChannelInfo" << endl;
#endif
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
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | GetConnectedProcesses" << endl;
#endif
    vector<std::string> ret;
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int timeout_recvfrom(__w64 unsigned int sock, sockaddr_in * sockfrom, int *fromlen, char *data, int length, int timeout_ms) {
    fd_set socks;
    struct timeval t;
    FD_ZERO(&socks);
    FD_SET(sock, &socks);
    t.tv_sec = timeout_ms / 1000;
    t.tv_usec = (timeout_ms % 1000) * 1000;
    if (select((int)sock + 1, &socks, NULL, NULL, &t)) {
        int res = recvfrom((SOCKET)sock, data, length, 0, (sockaddr *)sockfrom, fromlen);
        if (res == WSAEMSGSIZE) {
            MessageBox(NULL,"WSAEMSGSIZE","ERROR",MB_OK);
            return res;
        }
        else
            return res;
    }
    else
        return 0;
}

/*
 _____ _                        _          _ __  __      _   _               _
|_   _| |__  _ __ ___  __ _  __| | ___  __| |  \/  | ___| |_| |__   ___   __| |
  | | | '_ \| '__/ _ \/ _` |/ _` |/ _ \/ _` | |\/| |/ _ \ __| '_ \ / _ \ / _` |
  | | | | | | | |  __/ (_| | (_| |  __/ (_| | |  | |  __/ |_| | | | (_) | (_| |
  |_| |_| |_|_|  \___|\__,_|\__,_|\___|\__,_|_|  |_|\___|\__|_| |_|\___/ \__,_|
*/
void CIPC::ThreadedMethod(std::string thread_name, void *thread_params) {
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | ThreadedMethod - " << thread_name << endl;
#endif
    if (thread_name == string("MULTICAST_RECEIVING")) {
        // data buffer
        char datagram_content[BUFFER_SIZE+1];

        // receiving thread
        sockaddr_in localif;
        int         fromlen = sizeof(sockaddr_in);
        int         bytesRcvd = 0;
#ifdef COUT_TRACE
        cout << this << " " << _ID << " | timeout_recvfrom" << endl;
#endif
        bytesRcvd = timeout_recvfrom(_sr, &localif, &fromlen, datagram_content, BUFFER_SIZE, DEFAULT_TIMEOUT_ms);
        
        if (bytesRcvd > 0) {
#ifdef COUT_TRACE
            cout << this << " " << _ID << " | bytesRcvd : " << bytesRcvd << endl;
#endif
            // data received, check the channel name
            CGenericType data;
            datagram_content[bytesRcvd] = 0;
            // undump object
            string S = string(datagram_content);
#ifdef COUT_TRACE
            cout << this << " " << _ID << " | undump " << S << endl;
#endif
            data.Undump(S);
#ifdef COUT_TRACE
            cout << this << " " << _ID << " | undump DONE" << endl;
#endif
            // updata channel's data
            string channel_name = (data % CHANNEL_FIELD_NAME) .toString();
#ifdef COUT_TRACE
            cout << this << " " << _ID << " | signal event - " << channel_name << endl;
#endif
            synch.InstanceLock("ChannelDataSynch");
            if (_channelsData.find(channel_name) != _channelsData.end()) {
                _channelsData[channel_name] = data % CHANNEL_FIELD_DATA;
                _receptionEvents.NotifyEvent(channel_name);
            }

#ifdef COUT_TRACE
            cout << this << " " << _ID << " | signal event - LAST" << endl;
#endif
            data % CHANNEL_FIELD_DATA % "channelfield" = data % CHANNEL_FIELD_NAME;
            _channelsData["LAST"] = data % CHANNEL_FIELD_DATA;
            _receptionEvents.NotifyEvent("CIPC::ThreadedMethod::data_received");
#ifdef COUT_TRACE
            cout << this << " " << _ID << " | DONE" << endl;
#endif

            // if data
            synch.InstanceUnlock("ChannelDataSynch");
        }
    }
    else if (thread_name == string("FILEMAPPING_RECEIVING")) {
    
    }
#ifdef COUT_TRACE
    cout << this << " " << _ID << " | EXIT ThreadedMethod - " << thread_name << endl;
#endif
}
