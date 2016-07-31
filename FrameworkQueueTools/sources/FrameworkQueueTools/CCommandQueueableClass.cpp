#include "FrameworkQueueTools/CCommandQueueableClass.h"

using namespace FrameworkCore;
using namespace FrameworkQueueTools;


/*
  ____ ____                                          _  ___                              _     _       ____ _
 / ___/ ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___| | __ _ ___ ___
| |  | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   | |/ _` / __/ __|
| |__| |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |___| | (_| \__ \__ \
 \____\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____|_|\__,_|___/___/
*/
CCommandQueueableClass::CCommandQueueableClass() : _registeringClassBaseAddress(NULL) {
    _commandNameToFunction.clear();
    }

/*
 /\/|___ ____                                          _  ___                              _     _       ____ _
|/\/ ___/ ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |/ _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___| | __ _ ___ ___
  | |  | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   | |/ _` / __/ __|
  | |__| |__| (_) | | | | | | | | | | | (_| | | | | (_| | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |___| | (_| \__ \__ \
   \____\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____|_|\__,_|___/___/
*/
CCommandQueueableClass::~CCommandQueueableClass() {
    _commandNameToFunction.clear();
    }

/*
 ____            _     _             ___                              _     _       ____                                          _
|  _ \ ___  __ _(_)___| |_ ___ _ __ / _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
| |_) / _ \/ _` | / __| __/ _ \ '__| | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
|  _ <  __/ (_| | \__ \ ||  __/ |  | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|_| \_\___|\__, |_|___/\__\___|_|   \__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
           |___/
*/
/**
 *  \param command_name command name registered for the method
 *  \param method function pointer to the c function that implements the call to the method
 *  \param p_registering_class_base_address pointer to the object instance that will run the registered method
 *  \return \b TRUE on success, \b FALSE if command name is already registered
 *
 * This private method stores the data needed to call the method registered with the \c command_name name.
 * It is automatically called by the \c REGISTER_QUEUEABLE_COMMAND macro.
 */
bool CCommandQueueableClass::RegisterQueueableCommand(std::string command_name, CommandCall_type method, void *p_registering_class_base_address) {
    // store the registering class base address
    _registeringClassBaseAddress = p_registering_class_base_address;
    // command name has not to be already registered
    _commandNameToFunctionType::iterator it = _commandNameToFunction.find(command_name);
    if (it != _commandNameToFunction.end())
        // command name already registered
        return false;
    _commandNameToFunction[command_name] = method;
    return true;
    }

/*
 _   _                      _     _             ___                              _     _       ____                                          _
| | | |_ __  _ __ ___  __ _(_)___| |_ ___ _ __ / _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
| | | | '_ \| '__/ _ \/ _` | / __| __/ _ \ '__| | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
| |_| | | | | | |  __/ (_| | \__ \ ||  __/ |  | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| |
 \___/|_| |_|_|  \___|\__, |_|___/\__\___|_|   \__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
                      |___/
*/
/**
 *  \param command_name name of the command to unregister
 *
 * This method is called automatically by \c UNREGISTER_QUEUEABLE_COMMAND macro.
 */
void CCommandQueueableClass::UnregisterQueueableCommand(std::string command_name) {
    // command name has not to be already registered
    _commandNameToFunction.erase(command_name);
    }

/*
 _   _                      _     _             _    _ _  ___                              _     _       ____                                          _
| | | |_ __  _ __ ___  __ _(_)___| |_ ___ _ __ / \  | | |/ _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |___
| | | | '_ \| '__/ _ \/ _` | / __| __/ _ \ '__/ _ \ | | | | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
| |_| | | | | | |  __/ (_| | \__ \ ||  __/ | / ___ \| | | |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| \__ \
 \___/|_| |_|_|  \___|\__, |_|___/\__\___|_|/_/   \_\_|_|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
                      |___/
*/
/**
 *
 * This method is called automatically by \c UNREGISTER_ALL_QUEUEABLE_COMMANDS macro.
 */
void CCommandQueueableClass::UnregisterAllQueueableCommands() {
    _commandNameToFunction.clear();
    }

/*
            _    ___                              _     _       ____                                          _ ____       _
  __ _  ___| |_ / _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| / ___|  ___| |_
 / _` |/ _ \ __| | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` \___ \ / _ \ __|
| (_| |  __/ |_| |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| |___) |  __/ |_
 \__, |\___|\__|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|____/ \___|\__|
 |___/
*/
/**
 *  \param command_set string set filled with the registered commands
 *
 * This method returns the list of command names registered in the constructor.
 */
void CCommandQueueableClass::getQueueableCommandSet(std::set<std::string> &command_set) {
    command_set.clear();
    // cycle through all the registered commands and insert the name into the set
    _commandNameToFunctionType::iterator it = _commandNameToFunction.begin();
    while ( it != _commandNameToFunction.end() ) {
        command_set.insert(it->first);
        it++;
        }
    }

/*
 _____                     _        ___                              _     _       ____                                          _
| ____|_  _____  ___ _   _| |_ ___ / _ \ _   _  ___ _   _  ___  __ _| |__ | | ___ / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
|  _| \ \/ / _ \/ __| | | | __/ _ \ | | | | | |/ _ \ | | |/ _ \/ _` | '_ \| |/ _ \ |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
| |___ >  <  __/ (__| |_| | ||  __/ |_| | |_| |  __/ |_| |  __/ (_| | |_) | |  __/ |__| (_) | | | | | | | | | | | (_| | | | | (_| |
|_____/_/\_\___|\___|\__,_|\__\___|\__\_\\__,_|\___|\__,_|\___|\__,_|_.__/|_|\___|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
*/
/**
 *  \param command_name name of the command to execute
 *  \param params CGenericType parameters container
 *
 * This method calls the method that is registered to be a queueable command associated to the \c commandname name.
 */
bool CCommandQueueableClass::ExecuteQueueableCommand(std::string &command_name, CGenericType &params) {
    // efficiently fond the method to be executed
    _commandNameToFunctionType::iterator it = _commandNameToFunction.find(command_name);
    if (it == _commandNameToFunction.end())
        // command not founded
        return false;
    else {
        // execute the command
        CommandCall_type f = it->second;
        return f(_registeringClassBaseAddress, params);
        }
    return false;
    }
