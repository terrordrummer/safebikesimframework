#pragma once

#include <set>
#include <map>
#include <string>
#include "FrameworkCore/CGenericType.h"
#include "FrameworkQueueTools/common_defines.h"

/** \brief Tools for queue-based architectures.
 *
 * Command queue tools make possible to 
 * queue some commands and let them to be processed in a later moment.
 * The commands are executed by special class instances which inherits 
 * from the CCommandQueueableClass class. Each of these objects has to register 
 * to a command-queue manager which creates a map between the 
 * command name and the class instance that will be engaged for that command.
 *
 * In the common usage, the user has to:
 * \li instantiate the command-queue manager
 * \li register some command-queueable class instances to the manager
 * \li push some commands to the manager
 * \li ask the manager to process the command queue 
 *
 * The CCommandQueueableClass provides the mechanism to define a map between
 * command names and methods. The manager will be able to rerive this map
 * when this class will register.\n
 * The CCommandQueueManager provides a mechanism to redirect each queued command to the 
 * right registered command-queueable class instance.
 */
namespace FrameworkQueueTools {
    
    /**
     * The following macro is used by a CCommandQueueable class in order to register
     * a method in the available command set. Tipically this macro has to be used
     * inside the constructor class.
     */
    #define REGISTER_QUEUEABLE_COMMAND(command_name, class_name, method_name) {                                     \
        RegisterQueueableCommand(##command_name, &execute_command_##class_name##_##method_name, (void *)this);      \
        }

    /**
     * The following macro is used by a CCommandQueueable class in order to register
     * a method in the available command set. Tipically this macro has to be used
     * inside the constructor class.
     */
    #define UNREGISTER_QUEUEABLE_COMMAND(command_name) {    \
        UnregisterQueueableCommand(##command_name);         \
        }

    /**
     * The following macro is used by a CCommandQueueable class in order to register
     * a method in the available command set. Tipically this macro has to be used
     * inside the constructor class.
     */
    #define UNREGISTER_ALL_QUEUEABLE_COMMANDS() {   \
        UnregisterAllQueueableCommands();           \
        }

    /**
     * The following macro is used by a CCommandQueueable class in order to define
     * which methods are available as queueable commands. It has to be defined at
     * the beginning of the implementation file of the class.
     */
    #define DEFINE_QUEUEABLE_COMMAND(class_name, method_name)                                                   \
        bool execute_command_##class_name##_##method_name (void *p, FrameworkCore::CGenericType &params) {      \
        return ((##class_name *)p)->##method_name(params);                                                      \
        }


    /** \brief Class implementing queueable methods mechanism.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class is used to implement a mechanism of queueable commands. Any inhering class must implement the
     * commands as public methods and it must define and register them using some specific macros.
     * Then when a command will be sent to the command queueable manager it will dispatch the command to the right 
     * class that has registered. Class registration is done by the manager. \n
     *
     * \b USAGE \n
     * A class that wants to implement a queueable command must inherit CCommandQueueableClass, then it must:
     * \li define and implement the queueable commands as public methods 
     * \li declare the queueable commands inside its source code using the \b DEFINE_QUEUEABLE_COMMAND macro
     * \li register the commands inside the constructor code using the \b REGISTER_QUEUEABLE_COMMAND macro
     * 
     * Once these steps are done all the remaining work is done by this class and the command-queue 
     * manager one. As an example, the following class is defines to have a queueable command called "print" that
     * puts on the cout stream the content of the "output" field of the parameter object:
     *
     * \code
     * // - - - - - - - - HEADER FILE .h - - - - - - - -
     * #include <FrameworkQueueTools/CCommandQueueableClass.h>
     *
     * class myOutput : public CCommandQueueableClass {
     * public:
     *     myOutput();                          // default constructor
     *     bool output(CGenericType &param);    // queueable command prototype
     *  }
     * 
     * // - - - - - - - - SOURCE FILE .cpp  - - - - - - - -
     *
     * // declare the queueable command
     * DEFINE_QUEUEABLE_COMMAND(myOutput, output);
     *
     * myOutput::myOutput() {
     *     // register the command
     *     REGISTER_QUEUEABLE_COMMAND("output", myOutput, output);
     *     }
     *
     * // implement the command
     * bool myOutput::output(CGenericType &param) {
     *     cout << (param % "output").toString();
     *     }
     * \endcode
     *
     *
     */

    class framework_queue_tools_class CCommandQueueableClass {
    public:
        /** \brief Default header of the command method.
         *
         * Each command method prototype has to be defined with this header.                */
        typedef bool (*CommandCall_type)(void *, FrameworkCore::CGenericType& params);

    private:
        /** Command name to instance method container type.                                 */
        typedef std::map<std::string, CommandCall_type> _commandNameToFunctionType;

        /** \brief Command name to method map.
         *
         * This container maps each command name to an instance of the method that will be 
         * executed.                                                                        */
        _commandNameToFunctionType _commandNameToFunction;

        /**\brief Command execution data environment.
         *
         * Since C++ does not allow to manage method pointers with the flexibility required each
         * command should use an environment as workspace for exchanging variables between standard
         * methods and command methods.
         */
        FrameworkCore::CGenericType _commandEnvironment;

        /** \brief This pointer to the instantiation of the registering class (no casting). */
        void *_registeringClassBaseAddress;

    public:
        /** \brief Default constructor.                                                     */
        CCommandQueueableClass();

        /** \brief Default destructor.                                                      */
        ~CCommandQueueableClass();

        /** \brief Register a new queueable command and associate it to the given 
         *         instance method.                                                         */
        bool RegisterQueueableCommand(std::string command_name, CommandCall_type method, void *_registeringClassBaseAddress);

        /** \brief Unregister a queueable command.                                          */
        void UnregisterQueueableCommand(std::string command_name);

        /** \brief Unregister all queueable commands.                                       */
        void UnregisterAllQueueableCommands();

        /** \brief Retrive the list of available commands.                                  */
        void getQueueableCommandSet(std::set<std::string> &command_set);

        /** \brief Execute the given command with the given parameters.                     */
        bool ExecuteQueueableCommand(std::string &command_name, FrameworkCore::CGenericType &params);

        };

    }
