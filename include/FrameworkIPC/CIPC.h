#pragma once

// STL
#include <map>

// Framework
#include "FrameworkIPC/common_defines.h"
#include "FrameworkCore/CError.h"
#include "FrameworkCore/CSynch.h"
#include "FrameworkCore/CThread.h"
#include "FrameworkQueueTools/CCommandQueueManager.h"

/** \brief Inter-Process Comunication namespace.                    */
namespace FrameworkIPC {

    /** \brief Inter-Process Comunication class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements the methods needed for a full inter-process comunication. The protocol lets a process to
     * comunicate to another process running on the same machine or on another PC connected to the network.
     * The core data adopted by the comunication is the CGenericType data.
     *
     * 
     */
    class framework_ipc_class CIPC :   public    FrameworkCore::CError
                                     , protected FrameworkCore::CThread
                                     , protected FrameworkQueueTools::CCommandQueueManager {
    private:

        /**< \brief Transmission port.
         *
         * The socket connects to the multicast group and binds to this transmission port.
         */
        unsigned int _transmitPort; 
        
        /**< \brief Reception port.
         *
         * The socket connects to the multicast group and binds to this reception port.
         */
        unsigned int _receivePort;  

        /**< \brief Transmission socket.
         *
         * This socket is used to transmit data to the multicast group.
         */
        __w64 unsigned int  _st;

        /**< \brief Reception socket.
         *
         * This socket is used to get the data coming from the multicast group.
         */
        __w64 unsigned int  _sr;

        /** \brief Container of channel's data.                                                                             */
        typedef std::map<std::string, FrameworkCore::CGenericType> _tChannelData;

        /** \brief Channel data reception signal event container.                                                           */
        FrameworkCore::CSynch _receptionEvents;

        /** \brief Synchronized data received from the multicast group.
         *
         * This member stores the last data received from a channel. Before store this data synchronization of the channel 
         * has to be enabled.
         */
        _tChannelData _channelsData;

        /** \brief Group joined flag.
         *
         * This flag stores the state of the connection to the multicast group.
         */
        bool _joined;

        /** \brief Localhost machine name.
         *
         * This member stores the localhost name that will be attached to the comunication data in order
         * to detect if a data comes from a process on the same machine or from a machine on another PC
         * on the LAN.
         */
        std::string _localHost;
        
        /** \brief Local IP address.                                                                                        */
        std::string _localIP;

        /** \brief Temporary mapped file name.                                                                              */
        //std::map<std::string, HANDLE> _mappedFile;

        /** \brief Temporary mapped view.                                                                                   */
        //std::map<std::string, HANDLE> _mappedView;

        /** \brief Temporary mapped data.                                                                                   */
        //std::map<std::string, LPVOID> _mappedData;

        /** \brief Temporary path.                                                                                          */
        //std::string _tempPath;

        /** Synchronization object.                                                                                         */
        FrameworkCore::CSynch synch;

        static int  _counter;
        int         _ID;

    public:
        /** \brief Class constructor.                                                                                       */
        CIPC();

        /** \brief Virtual destructor.                                                                                      */
        virtual ~CIPC();
        
        /** \brief Join the IPC group.                                                                                      */
        bool Join();

        /** \brief Leave the IPC group.                             */
        bool Leave();

        /** \brief Wait incomingfrom data from a channel.
         *
         * This method waits some data coming from the given IPC channel.
         */
        bool WaitDataFromChannel(std::string channel_name, FrameworkCore::CGenericType &data, unsigned int timeout_ms = 10000);

        /** \brief Write data to an IPC channel.
         *
         * This method writes a data to the given IPC channel.
         */
        bool WriteDataToChannel(std::string channel_name, FrameworkCore::CGenericType &data);

        /** \brief Clean the content of the bufferend data channel. */
        void CleanChannelBuffer(std::string channel_name);

        /** \brief Enable a channel's data synchronization.
         *
         * This method keeps synchronized the channel data content coming from the IPC data exchange.
         */
        void EnableDataSynchronization(std::string channel_name);

        /** \brief Disable a channel's data synchronization.
         *
         * This method stops the synchronization of the given channel's data.
         */
        void DisableDataSynchronization(std::string channel_name);

        /** \brief Disable all channel's data synchronization.                                                              */
        void DisableAllDataSynchronization();

        /** \brief Gets the last channel data content.                                                                      */
        void GetChannelData(std::string channel_name, FrameworkCore::CGenericType &data);

        /** \brief Get the next data received from the group, no matter the channel.                                        */
        void WaitNextReceivedData(FrameworkCore::CGenericType &data);

        /** \brief Get the next data received from the group with a maximum timeout.                                        */
        bool WaitNextReceivedData(FrameworkCore::CGenericType &data, unsigned int timeout_ms);

        /** \brief Register receiver command queueable method.                                                              */
        bool BindChannelReceiverCommand(std::string channel, std::string command_name);

        /** \brief Register receiver command queueable method.                                                              */
        void UnbindChannelReceiverCommand(std::string channel, std::string command_name);

        /** \brief Register receiver command queueable method.                                                              */
        void UnbindallChannelReceiverCommands(std::string channel);

        /** \brief Get channel info.                                                                                        */
        std::string GetChannelInfo();

        /** \brief Get processes connected to the channel.                                                                  */
        std::vector<std::string> GetConnectedProcesses();

    protected:

        /** \brief Virtual threaded method from CThread class                                                               */
        virtual void ThreadedMethod(std::string thread_name, void *thread_params);

    private:
        /** \brief Retrive the locap IP assigned to the first network adapter.                                              */
        void getLocalIP(std::string &localIP);

        /** \brief Connects to the file mapping associated to the data object with the given name.                          */
        void MapDataObject(std::string);
        };

    }