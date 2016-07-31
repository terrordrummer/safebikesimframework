#pragma once

#include <queue>
#include <map>
#include <string>
#include "FrameworkCore/CGenericType.h"
#include "FrameworkCore/CSynch.h"
#include "FrameworkCore/CTrace.h"
#include "FrameworkQueueTools/CCommandQueueableClass.h"

namespace FrameworkQueueTools {


    /** \brief Command queue manager.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * The command queue manager manages the command queue and allows
     * command-queueable class instances to register on that queue.\n
     * Registration/unregistration is done by calling 
     * RegisterCommandQueueableClass(), UnregisterCommandQueueableClass()
     * or UnregisterAllRegisteredQueueableCommands() methods.\n
     *
     * Commands are managed in a queue and they can be pushed calling
     * PushCommand().\n
     *
     * The command queue can be processed calling ProcessCommandQueue() or can be
     * cleared calling EmptyCommandQueue().\n
     *
     * Log feature is enabled by calling StartLoggingQueuableCommands() and
     * disabled by calling StopLoggingQueueableCommands().
     */

    class framework_queue_tools_class CCommandQueueManager {
    private:
        /** \brief Struct of queued commands.                                                   */
        typedef struct {
            std::string                 class_name;     /**< name of the class that has to execute the command. */
            std::string                 command_name;   /**< name of the command that has to be executed.       */
            FrameworkCore::CGenericType params;         /**< command parameters.                                */
            } _tCommandParameters;

        /** \brief Command queue map container.                                                 */
        typedef std::queue<_tCommandParameters> _commandQueueType;

        /** \brief Registered class class container.                                            */
        typedef std::map<std::string, CCommandQueueableClass *> _registeredClassType;

        /** \brief Default command to registered class map type.                                */
        typedef std::map<std::string, std::string>  _defaultCommandToRegisteredClassType;

        /** \brief Command queue list.                                                          */
        _commandQueueType _commandQueue;

        /** \brief Registered class set.                                                        */
        _registeredClassType _registeredClass;

        /** \brief Map of default commands.
         *
         * A default command is a command which can be executed without specifying the registered class name. Each
         * class that registers itself will be asked if the command list it can respond to. If all these command
         * names are not yet defined as default then the while set of commands are added to the default command
         * set and the registering class is marker as associated to these commands.
         * This mechanism is useful in order to let some class to register and let the user to push the commands
         * without specifying all the time the class name too but only the command name.
         */
        _defaultCommandToRegisteredClassType    _defaultCommandToRegisteredClass;

        /** \Internal synchronization variable.                                                 */
        FrameworkCore::CSynch                   _synch;

        /** \brief Logging commands class.                                                      */
        FrameworkCore::CTrace            _logCommands;

        /** \brief Log commands flag.                                                           */
        bool                                    _logCommandsEnabled;

    public:
        /** \brief Default constructor.                                                         */
        CCommandQueueManager();

        /** \brief Default destructor.                                                          */
        ~CCommandQueueManager();

        /////////////////////////////////////////
        //          QUEUE MANAGEMENT
        /////////////////////////////////////////
        /** \brief Push a command that has to be executed from the specified class.
         *
         * Each command has to be exxecuted by a specified class. If the specified class does not exist or does
         * not respond to the given command then a FALSE value is returned.
         * The parameters has to be stored into a record set.
         */
        bool PushCommand(std::string &class_name, std::string &command_name, FrameworkCore::CGenericType &params);
        
        /** \brief Push a generic command into the list.
         *
         * A generic command will be executed from the first class that has been registered making avvailavle the
         * given command. If no available classes exist then a FALSE value is returned.
         * The parameters has to be stored into a record set.
         */
        bool PushCommand(std::string &command_name, FrameworkCore::CGenericType &params);

        /** \brief Push a generic command into the list.
         *
         * A generic command will be executed from the first class that has been registered making avvailavle the
         * given command. If no available classes exist then a FALSE value is returned.
         * The parameters has to be stored into a record set.
         */
        __inline bool PushCommand(char *command_name, FrameworkCore::CGenericType &params) {
            return PushCommand(std::string(command_name), params);
            }

        /** \brief Process the command queue.
         *
         * The manager pops the command one-by-one and ask the right command-queueable class to run it.
         */
        void ProcessCommandQueue();

        /** brief Empty the queue.                  */
        void EmptyCommandQueue();

        ////////////////////////////////////////////////////////
        //          COMMAND-QUEUEABLE CLASS MANAGEMENT
        ////////////////////////////////////////////////////////
        /** \brief Register a new command-queueable class instance.     */
        bool RegisterCommandQueueableClass(std::string &class_name, CCommandQueueableClass *class_instance);
        
        /** \brief Unregister a new command-queueable class instance.   */
        void UnregisterCommandQueueableClass(std::string &class_name);

        /** \brief Unregister all queueable commands.                   */
        void UnregisterAllRegisteredQueueableCommands();

        ////////////////////////////////////////
        //          LOGGING MANAGEMENT
        ////////////////////////////////////////
        /** \brief Start logging commands.                              
         *
         * This method enables the logging of processed commands. The output means the kind of stream used
         * for logging assuming the same convention of CTrace class.
         */
        void StartLoggingQueuableCommands(std::string &output);

        /** \brief Stop logging queueable commands.                     */
        void StopLoggingQueueableCommands();

    private:
        /** \brief Get the next command.            */
        bool PopCommand(std::string &class_name, std::string &command_name, FrameworkCore::CGenericType &params);

        };

    }