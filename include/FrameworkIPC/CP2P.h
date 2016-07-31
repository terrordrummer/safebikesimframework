#pragma once

// STL
#include <map>

// Framework
#include "FrameworkIPC/common_defines.h"
#include "FrameworkCore/CError.h"
#include "FrameworkQueueTools/CCommandQueueManager.h"

// System
#include "windows.h"

namespace FrameworkIPC {
    /** \brief Process-To-Process Comunication class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements some basic methods for process-to-process comunication. The protocol is very simple
     * and  no broadcast comunication is possibile using this class. Moreover, synchronization must be provided
     * by actors to avoid deadlocks (see next). \n
     * The comunication starts with a <b>server process</b> which creates a named channel and starts waiting
     * for connections, then a <b>client process</b> can connect to the named channel (same name) to start the
     * comunication with the server process. \n
     * It is worth to highlight that the following rules must be gauranteeded to have the comunication working:
     *
     * \li there always should be a process writing data into the channel and the other process reading this data 
     *     from the channel (synchronous protocol transmission)
     * \li it must be avoided the situations in which both processes are reading data from the channel. This 
     *     situation reveals to be a <b>deadlock</b> because both processes are expecting the other process to write 
     *     some data
     * \li no result is gauranteed if both processes are concurrently writing into the channel
     *
     * Following this rules will result in a correct comunication. During a comunication session bot server and
     * client are allowed to write data i.e. comunication can be bi-directional but concurrent write/read is not possible. \n
     * Closing the channel has the effect of terminating the comunication between processes. If the comunication terminates
     * in an unexpected way a process will get write/read errors and will be able to reset the status of the CP2P instance.
     */
    class framework_ipc_class CP2P : public FrameworkCore::CError {
    private:
        /** \brief Type of map from name to channels.               */
        typedef std::map<std::string, HANDLE> _channelMapType;

        /** \brief Map between file names and file mapping.         */
        _channelMapType _channelMap;

        /** \brief Process ID.                                      */
        DWORD _processId;

    public:
        /** \brief Class constructor.                               */
        CP2P();

        /** \brief Create an IPC channel.                           */
        bool CreateChannel(std::string channel_name);

        /** \brief Waits for a process to connect to the channel.   */
        bool WaitForConnections(std::string channel_name);

        /** \brief Connect to the channel.                          */
        bool Connect(std::string channel_name);

        /** \brief Read data from an IPC channel.                   */
        bool Read(std::string channel_name, unsigned int data_to_read, void *data_content, unsigned int *data_readed);

        /** \brief Write data to an IPC channel.                    */
        bool Write(std::string channel_name, unsigned int size_to_write, void *data_content);

        /** \brief Write a text to an IPC channel.                  */
        bool Write(std::string channel_name, std::string text);

        /** \brief Close IPC channel.                               */
        bool Close(std::string channel_name);

        };

    }