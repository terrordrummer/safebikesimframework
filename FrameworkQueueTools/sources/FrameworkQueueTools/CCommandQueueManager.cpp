#include "FrameworkQueueTools/CCommandQueueManager.h"

using namespace FrameworkCore;
using namespace FrameworkQueueTools;
using namespace std;


//////////////////////////////////
//      LOCAL DEFINES
//////////////////////////////////
#define DEFAULT_COMMAND_CLASS_NAME "__DEFAULT"

/*
  ____ ____                                          _  ___
 / ___/ ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___
| |  | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \
| |__| |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/
 \____\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|
*/
CCommandQueueManager::CCommandQueueManager() : _logCommandsEnabled(false) {

    };

/*
 /\/|___ ____                                          _  ___
|/\/ ___/ ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___
  | |  | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \
  | |__| |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/
   \____\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|
*/
CCommandQueueManager::~CCommandQueueManager() {
    EmptyCommandQueue();
    };

/*
 ____            _      ____                                          _
|  _ \ _   _ ___| |__  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
| |_) | | | / __| '_ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
|  __/| |_| \__ \ | | | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|_|    \__,_|___/_| |_|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
*/
/**
 *  \param class_name name of the class that has to manage the command
 *  \param command_name name of the command to be executed
 *  \param params parameters passed to the command
 *  \return \b TRUE if command is syccesfully pushed, \b FALSE otherwise
 *
 * When a user pushes a command this data is stored into a process queue and the method exits immediatly,
 * the commands will be executed later by calling the ProcessCommandQueue method.
 */
bool CCommandQueueManager::PushCommand(std::string &class_name, std::string &command_name, CGenericType &params) {
    _synch.InstanceLock("CCommandQueueManagerMnager_QueueLock");
    _tCommandParameters element;
    element.class_name   = class_name;
    element.command_name = command_name;
    element.params       = params;
    _commandQueue.push(element);
    _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
    return true;
    }

/*
 ____            _      ____                                          _
|  _ \ _   _ ___| |__  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
| |_) | | | / __| '_ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
|  __/| |_| \__ \ | | | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|_|    \__,_|___/_| |_|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
*/
/**
 *  \param command_name name of the command to be executed
 *  \param params parameters passed to the command
 *  \return \b TRUE if command is syccesfully pushed, \b FALSE otherwise
 *
 * When a user pushes a command this data is stored into a process queue and the method exits immediatly,
 * the commands will be executed later by calling the ProcessCommandQueue method. In this case the class
 * is not specified, it means that the command will have a global scope name and the user will have to check
 * that no conflits with same command name of different classes are registered.
 */
bool CCommandQueueManager::PushCommand(std::string &command_name, CGenericType &params) {
    _synch.InstanceLock("CCommandQueueManagerMnager_QueueLock");
    _tCommandParameters element;
    element.class_name   = DEFAULT_COMMAND_CLASS_NAME;
    element.command_name = command_name;
    element.params       = params;
    _commandQueue.push(element);
    _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
    return true;
    }

/*
 ____                              ____                                          _  ___
|  _ \ _ __ ___   ___ ___ ___ ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___
| |_) | '__/ _ \ / __/ _ Y __/ __| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \
|  __/| | | (_) | (_|  __|__ \__ \ |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/
|_|   |_|  \___/ \___\___|___/___/\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|
*/
/**
 *  Process the pushed commands one by one in a FIFO order.
 */
void CCommandQueueManager::ProcessCommandQueue() {
    // pop all pending commands and resolve the class that has to execute them
    string class_name;
    string command_name;
    CGenericType params;
    _synch.InstanceLock("CCommandQueueManager_Process");
    // pop commands from the queue
    while ( PopCommand(class_name, command_name, params) ) 
    {
        // check is a class is specified fot the command
        if ( class_name.compare(DEFAULT_COMMAND_CLASS_NAME)==0 ) 
        {
            // command class is not specified, it is a default command (i.e. no class is defined)
            // search the class into the default command map
            _defaultCommandToRegisteredClassType::iterator it = _defaultCommandToRegisteredClass.find(command_name);
            if ( it != _defaultCommandToRegisteredClass.end() ) 
                {
                // class fo the default command has been founded, execute the command
                CCommandQueueableClass *selected_class = (CCommandQueueableClass *)_registeredClass[it->second];
                selected_class->ExecuteQueueableCommand(command_name, params);
                if (_logCommandsEnabled)
                    _logCommands << command_name << " : " << params.Dump() << "\n";
                }
            //continue;
        } 
        else 
        {
            // a class for the command is specified, check if the class has been registered
            _registeredClassType::iterator it = _registeredClass.find(class_name);
            if ( it != _registeredClass.end() ) 
            {
                // class registered, execute the command
                ((CCommandQueueableClass *)it->second)->ExecuteQueueableCommand(command_name, params);
            }
        }
    }
    _synch.InstanceUnlock("CCommandQueueManager_Process");
    }

/*
 _____                 _          ____                                          _  ___
| ____|_ __ ___  _ __ | |_ _   _ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___
|  _| | '_ ` _ \| '_ \| __| | | | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \
| |___| | | | | | |_) | |_| |_| | |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/
|_____|_| |_| |_| .__/ \__|\__, |\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|
                |_|        |___/
*/
/**
 * Empty the command queue discarding the whole command queue content.
 */
void CCommandQueueManager::EmptyCommandQueue() {
    _synch.InstanceLock("CCommandQueueManagerMnager_QueueLock");
    // pop all commands in the queue
    while (!_commandQueue.empty())
        _commandQueue.pop();
    _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
    }

/*
 ____            _     _             ____                                          _  ___                              _     _       ____ _
|  _ \ ___  __ _(_)___| |_ ___ _ __ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___| | __ _ ___ ___
| |_) / _ \/ _` | / __| __/ _ \ '__| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   | |/ _` / __/ __|
|  _ <  __/ (_| | \__ \ ||  __/ |  | |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |___| | (_| \__ \__ \
|_| \_\___|\__, |_|___/\__\___|_|   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____|_|\__,_|___/___/
           |___/
*/
/**
 *
 */
bool CCommandQueueManager::RegisterCommandQueueableClass(std::string &class_name, CCommandQueueableClass *class_instance) {
    bool retval;
    _synch.InstanceLock("CCommandQueueManagerMnager_QueueLock");
    // avoid duplicated names
    if (_registeredClass.find(class_name) != _registeredClass.end()) {
        // class is already registered. Discard this second one
        retval =  false;
        }
    else {
        // store the class pointer
        _registeredClass[class_name] = class_instance;
        // get the list of available commands of this class
        set<string> commandSet;
        class_instance->getQueueableCommandSet(commandSet);
        // if a command is already available then ALL commands of the class are disabled by default in order to avoid
        // the mixing of several default commands between different classes.
        bool register_default_commands = true;
        set<string>::iterator it = commandSet.begin();
        while (it != commandSet.end()) {
            if ( _defaultCommandToRegisteredClass.find(*it) != _defaultCommandToRegisteredClass.end()) {
                // a command with the same name has already been registered by another queueable class. All the 
                // commands of the actual class will not be registered as available without specifying the class name.
                register_default_commands = false;
                continue;
                }
            it++;
            }
        // check if commands should be registered as available without the class name
        if (register_default_commands) {
            // register commands as default for this class
            it = commandSet.begin();
            while (it != commandSet.end()) {
                _defaultCommandToRegisteredClass[*it] = class_name;
                it++;
                }
            }
        // registration done
        retval = true;
        }
    // release mutex and return
    _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
    return retval;
    }

/*
 _   _                      _     _             ____                                          _  ___                              _     _       ____ _
| | | |_ __  _ __ ___  __ _(_)___| |_ ___ _ __ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___| | __ _ ___ ___
| | | | '_ \| '__/ _ \/ _` | / __| __/ _ \ '__| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   | |/ _` / __/ __|
| |_| | | | | | |  __/ (_| | \__ \ ||  __/ |  | |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |___| | (_| \__ \__ \
 \___/|_| |_|_|  \___|\__, |_|___/\__\___|_|   \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____|_|\__,_|___/___/
                      |___/
*/
void CCommandQueueManager::UnregisterCommandQueueableClass(std::string &class_name) {
    _synch.InstanceLock("CCommandQueueManagerMnager_QueueLock");
    // delete the class pointer
    _registeredClass.erase(class_name);
    // remove all default commands associated to the class
    _defaultCommandToRegisteredClassType::iterator it = _defaultCommandToRegisteredClass.begin();
    while(it != _defaultCommandToRegisteredClass.end()) {
        // erase the default command if it is associated to the unregistering class
        if ( it->second.compare(class_name) == 0 ) {
            _defaultCommandToRegisteredClass.erase(it->first);
            // restart from the first command again
            it = _defaultCommandToRegisteredClass.begin();
            }
        else
            // move to the next command
            it++;
        }
    // release mutex and return
    _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
    }

/*
 _   _                      _     _             _    _ _ ____            _     _                    _  ___                              _     _       ____                                          _
| | | |_ __  _ __ ___  __ _(_)___| |_ ___ _ __ / \  | | |  _ \ ___  __ _(_)___| |_ ___ _ __ ___  __| |/ _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |___
| | | | '_ \| '__/ _ \/ _` | / __| __/ _ \ '__/ _ \ | | | |_) / _ \/ _` | / __| __/ _ \ '__/ _ \/ _` | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
| |_| | | | | | |  __/ (_| | \__ \ ||  __/ | / ___ \| | |  _ <  __/ (_| | \__ \ ||  __/ | |  __/ (_| | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| \__ \
 \___/|_| |_|_|  \___|\__, |_|___/\__\___|_|/_/   \_\_|_|_| \_\___|\__, |_|___/\__\___|_|  \___|\__,_|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
                      |___/                                        |___/
*/
void CCommandQueueManager::UnregisterAllRegisteredQueueableCommands() {
    // unregister all classes
    _registeredClassType::iterator it = _registeredClass.begin();
    while (it != _registeredClass.end()) {
        UnregisterCommandQueueableClass( (string)*(&(it->first)) );
        it = _registeredClass.begin();
        }
    }

/*
 ____              ____                                          _
|  _ \ ___  _ __  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
| |_) / _ \| '_ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
|  __/ (_) | |_) | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|_|   \___/| .__/ \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
           |_|
*/
bool CCommandQueueManager::PopCommand(std::string &class_name, std::string &command_name, CGenericType &params) {
    bool retval;
    _synch.InstanceLock("CCommandQueueManagerMnager_QueueLock");
    if (_commandQueue.empty()) {
        _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
        retval = false;
    }
    else {
        // get the command name and parameters
        class_name   = _commandQueue.front().class_name;
        command_name = _commandQueue.front().command_name;
        params       = _commandQueue.front().params;
        // remove the command from the queue
        _commandQueue.pop();
        _synch.InstanceUnlock("CCommandQueueManagerMnager_QueueLock");
        retval = true;
        }
    return retval;
    }

/*
 ____  _             _   _                      _              ___                         _     _       ____                                          _
/ ___|| |_ __ _ _ __| |_| |    ___   __ _  __ _(_)_ __   __ _ / _ \ _   _  ___ _   _  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |___
\___ \| __/ _` | '__| __| |   / _ \ / _` |/ _` | | '_ \ / _` | | | | | | |/ _ \ | | |/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
 ___) | || (_| | |  | |_| |__| (_) | (_| | (_| | | | | | (_| | |_| | |_| |  __/ |_| | (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| \__ \
|____/ \__\__,_|_|   \__|_____\___/ \__, |\__, |_|_| |_|\__, |\__\_\\__,_|\___|\__,_|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
                                    |___/ |___/         |___/
*/
void CCommandQueueManager::StartLoggingQueuableCommands(std::string &output) {
    // set the log stream
    _logCommands.SetLogStream(output);
    // enabe command logging
    _logCommandsEnabled = true;
    }

/*
 ____  _              _                      _              ___                              _     _       ____                                          _
/ ___|| |_ ___  _ __ | |    ___   __ _  __ _(_)_ __   __ _ / _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |___
\___ \| __/ _ \| '_ \| |   / _ \ / _` |/ _` | | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
 ___) | || (_) | |_) | |__| (_) | (_| | (_| | | | | | (_| | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| \__ \
|____/ \__\___/| .__/|_____\___/ \__, |\__, |_|_| |_|\__, |\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
               |_|               |___/ |___/         |___/
*/
void CCommandQueueManager::StopLoggingQueueableCommands() {
    // restore log stream
    _logCommands.SetLogStream("cout");
    // disable command logging
    _logCommandsEnabled = false;
    }