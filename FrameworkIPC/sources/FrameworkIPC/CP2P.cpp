#include "FrameworkIPC/CP2P.h"
#include <sstream>
#include <iostream>


// -------------------------------------------------------
// alias to the Windows system GetLastError API functions
DWORD (WINAPI *winGetLastError)(void) = GetLastError;
// -------------------------------------------------------

using namespace FrameworkIPC;

CP2P::CP2P() {
    SetErrorClassPrefix("CP2P");
    _processId = GetCurrentProcessId();
    _channelMap.clear();
    };

/*
  ____                _        ____ _                            _
 / ___|_ __ ___  __ _| |_ ___ / ___| |__   __ _ _ __  _ __   ___| |
| |   | '__/ _ \/ _` | __/ _ \ |   | '_ \ / _` | '_ \| '_ \ / _ \ |
| |___| | |  __/ (_| | ||  __/ |___| | | | (_| | | | | | | |  __/ |
 \____|_|  \___|\__,_|\__\___|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
*/
/**
 *  \param channel_name name identifier of the comunication channel
 *  \return TRUE if channel is created succesfully
 *
 * This method creates an instance of an inter-process comunication channel. Once this channel is
 * created the process will have to listen to this channel for upcoming connections from some other
 * process. The comunication protocol works as a Server-Client connections: first the Server must create the channel and
 * must wait for connections of the clients, once the Server is waiting the client can connect to the channel. \n
 * The comunication have to be synchronized i.e. the process that reads the pipe is locked until some data is really
 * written by the other process. Moreover
 */
bool CP2P::CreateChannel(std::string channel_name) {
    _channelMapType::iterator it = _channelMap.find(channel_name);
    if (it != _channelMap.end()) {
        // channel already exist
        return true;
        }
    std::string pipe_name = std::string("\\\\.\\pipe\\") + channel_name;
    HANDLE pipe_handle = CreateNamedPipe( pipe_name.c_str(), 
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 
        PIPE_UNLIMITED_INSTANCES,
        1024*10,
        1024*10,
        0,
        NULL);
    if (pipe_handle == INVALID_HANDLE_VALUE) {
        // error creating while the channel, get the last error
        char msg[1024];
        DWORD last_error = winGetLastError();
        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 
            NULL, 
            last_error, 
            LANG_SYSTEM_DEFAULT,
            (LPSTR)msg,
            1024,
            NULL);
        PushError("CreateChannel", std::string("System error crating the channel: <") + std::string(msg) + std::string(">"));
        return false;
        }
    // pipe created
    _channelMap[channel_name] = pipe_handle;
    return true;
    };

/*
__        __    _ _   _____           ____                            _   _
\ \      / /_ _(_) |_|  ___|__  _ __ / ___|___  _ __  _ __   ___  ___| |_(_) ___  _ __  ___
 \ \ /\ / / _` | | __| |_ / _ \| '__| |   / _ \| '_ \| '_ \ / _ \/ __| __| |/ _ \| '_ \/ __|
  \ V  V / (_| | | |_|  _| (_) | |  | |__| (_) | | | | | | |  __/ (__| |_| | (_) | | | \__ \
   \_/\_/ \__,_|_|\__|_|  \___/|_|   \____\___/|_| |_|_| |_|\___|\___|\__|_|\___/|_| |_|___/
*/
/**
 *  \param channel_name channel's name on which the connection has to be waited
 *  \return TRUE if another process has connected
 *
 * This method waits for an existing channel to be connected by another process. This method is locking until
 * a connection is established or until the channel is closed by another thread.
 */
bool CP2P::WaitForConnections(std::string channel_name) {
    // channel must exist
    _channelMapType::iterator it = _channelMap.find(channel_name);
    if (it == _channelMap.end()) {
        // channel not found, create a new one
        std::stringstream s;
        s << "channel -" << channel_name << "- not found" ;
        PushError("WaitForConnections", s.str());
        return false;
        }
    // CHECK ERRORS
    ConnectNamedPipe(_channelMap[channel_name],NULL);
    return true;
    };

/*
  ____                            _
 / ___|___  _ __  _ __   ___  ___| |_
| |   / _ \| '_ \| '_ \ / _ \/ __| __|
| |__| (_) | | | | | | |  __/ (__| |_
 \____\___/|_| |_|_| |_|\___|\___|\__|
*/
/**
 *  \param channel_name channel's name to which the connection should be established
 *  \return TRUE if the connection has been established
 *
 * If this method has success then the connection over the given channel is ready for transmission. To have
 * success this method requires another process to have already created the channel and being waiting for
 * connections over it.
 *
 */
bool CP2P::Connect(std::string channel_name) {
    // channel must exist
    _channelMapType::iterator it = _channelMap.find(channel_name);
    if (it != _channelMap.end()) {
        return true;
        }
    std::string pipe_name = std::string("\\\\.\\pipe\\") + channel_name;
    HANDLE pipe_handle = CreateFile( 
         pipe_name.c_str(),   // pipe name 
         GENERIC_READ |  // read and write access 
         GENERIC_WRITE, 
         0,              // no sharing 
         NULL,           // default security attributes
         OPEN_EXISTING,  // opens existing pipe 
         0,              // default attributes 
         NULL);          // no template file

    if (pipe_handle == INVALID_HANDLE_VALUE) {
        // error creating while the channel, get the last error
        char msg[1024];
        DWORD last_error = winGetLastError();
        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 
            NULL, 
            last_error, 
            LANG_SYSTEM_DEFAULT,
            (LPSTR)msg,
            1024,
            NULL);
        PushError("Connect", std::string("System error connecting the channel: -") + std::string(msg) + std::string("-"));
        return false;
        }

    // channel created!
    _channelMap[channel_name] = pipe_handle;
    return true;
    }

/*
 ____                _
|  _ \ ___  __ _  __| |
| |_) / _ \/ _` |/ _` |
|  _ <  __/ (_| | (_| |
|_| \_\___|\__,_|\__,_|
*/
/**
 *  \param channel_name channel from which read data
 *  \param data_to_read maximum size of the data expected to be readed
 *  \param data_content pointer to the memory which will receive the data
 *  \param data_readed size of data received
 *  \return TRUE id read operation had success
 *
 * This method waits for some data to be written on the channel. It will returns when data will be received
 * in the buffer. Return value can be FALSE if channel is not ready for transmission.
 */
bool CP2P::Read(std::string channel_name, unsigned int size_to_read, void *data_content, unsigned int *data_readed) {
    // wait from the channel
    unsigned int read_pos = 0;
    while (1) {
        BOOL res = ReadFile(_channelMap[channel_name], (void *)&(((char *)data_content)[read_pos]), size_to_read - read_pos, (LPDWORD)data_readed,NULL);
        if (res == FALSE) {
            // check the reason of the error
            if (winGetLastError() == ERROR_MORE_DATA) {
                // message is not completely received, read next data
                read_pos += *data_readed;
                } else {
                    // error creating while the channel, get the last error
                    char msg[1024];
                    DWORD last_error = winGetLastError();
                    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 
                        NULL, 
                        last_error, 
                        LANG_SYSTEM_DEFAULT,
                        (LPSTR)msg,
                        1024,
                        NULL);
                    PushError("Read", std::string("System error reading the channel: -") + std::string(msg) + std::string("-"));
                    *data_readed = 0;
                    return false;
                    }
            } else
                break;
        }
    // data readed otherwise
    return true;
    };

/*
__        __    _ _
\ \      / / __(_) |_ ___
 \ \ /\ / / '__| | __/ _ \
  \ V  V /| |  | | ||  __/
   \_/\_/ |_|  |_|\__\___|
*/
/**
 *  \param channel_name channel on which write data
 *  \param size_to_write size of data to write on the channel
 *  \param data_content pointer to the data to be written
 *  \return TRUE if data has been written correctly
 *
 * This method writes data to the given channel. It can return FALSE if channel is not yet 
 * ready for comunication.
 */
bool CP2P::Write(std::string channel_name, unsigned int size_to_write, void *data_content) {
    DWORD data_written;
    if (WriteFile(_channelMap[channel_name], data_content, size_to_write, (LPDWORD)&data_written, NULL) == NULL) {
        // error creating while the channel, get the last error
        char msg[1024];
        DWORD last_error = winGetLastError();
        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 
            NULL, 
            last_error, 
            LANG_SYSTEM_DEFAULT,
            (LPSTR)msg,
            1024,
            NULL);
        PushError("Write", std::string("System error writing the channel: -") + std::string(msg) + std::string("-"));
        return false;
        }
    // data written
    return true;
    };

/*
__        __    _ _
\ \      / / __(_) |_ ___
 \ \ /\ / / '__| | __/ _ \
  \ V  V /| |  | | ||  __/
   \_/\_/ |_|  |_|\__\___|
*/
/**
 *  \param channel_name channel on which transmit the text
 *  \param text text to transmit over the channel
 *  \return TRUE if text has been written correctly
 *
 * This method writes a text sentence to the given channel. It can return FALSE if channel is not yet
 * ready for comunication.
 */
bool CP2P::Write(std::string channel_name, std::string text) {
    return Write(channel_name,(unsigned int)text.length()+1, (void *)(text.c_str()));
}

/*
  ____ _
 / ___| | ___  ___  ___
| |   | |/ _ \/ __|/ _ \
| |___| | (_) \__ \  __/
 \____|_|\___/|___/\___|
*/
/**
 *  \param channel_name name of che channel to close
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This method closes the given channel. Comunication will not be enabled anymnore until che channel 
 * will be connected again.
 */
bool CP2P::Close(std::string channel_name) {
     // channel must exist
    _channelMapType::iterator it = _channelMap.find(channel_name);
    if (it == _channelMap.end()) {
        // no channel to close
        std::stringstream s;
        s << "Attemting to close a non-existent channel -" << channel_name << "-";
        PushError("Close", s.str());
        return false;
        }
    // close handle, remove handle from the map and return
    CloseHandle(it->second);
    _channelMap.erase(it);
    return true;
    };