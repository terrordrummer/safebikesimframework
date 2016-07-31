#pragma once

#include <string>
#include <stack>
#include "FrameworkCore/common_defines.h"

namespace FrameworkCore {

    /** \brief Error management class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements a basic set of methods in order to manage the occdurrence of an error event.
     * The error management is done by using a stack of error messages: when an error occurs its description 
     * has to be pushed into this stack. At any time, the error stack contains the messages describing the 
     * list of errors occurred in the past.\n
     * An instance is considered to be in an error state if its description stack is not empty.\n
     * The error stack have to be cleared in order to reset the error state.
     *
     * This class should be inherited by the classes that want to approach the error management using 
     * this strategy. Inheriting this class allows to:
     * \li check the error state
     * \li push a new error description into the stack
     * \li retrive the last error message
     * \li retrive the entire list of error messages
     * \li pop the error message at the top of the stack
     * \li empty the error stack
     * \n
     *
     * \b USAGE \n
     * There are few rules that each inheriting class should follw:
     * \li <b>Class constructor should call SetErrorPrefix</b> method with the name of the class as parameter. 
     *     In such way any error message will be prefixed by the class name. The standard format for the class 
     *     name is <b>[<em>class name</em>]</b>. \n
     *     The call will be:
     *     \code
     *     SetErrorPrefix("[myClass]");
     *     \endcode
     * \li <b>Each error message should be pushed along with the name of the method that is pushing it</b> in order 
     *     to trace the error call stack.\n
     *     The call will be:
     *     \code
     *     PushError("MethodName" , "This is an error message");
     *     \endcode
     * \li <b>A class that detects an error for an object X and wants to propagate the error instead of manage
     *     it must call the PropagateError method. \n
     *     As an example, suppose that myObj inherits the CErrorManagement class, then the following:
     *     \code
     *     if (myObj.HasError())
     *      PropagateError("MethodName", &myObj);
     *     \endcode
     *     Notice that this operation will reset the error inside \c myObj and move the error state
     *     inside the calling object.
     *
     *     Simple class example:
     *      \code
     *      class FileRead : CError {
     *      private:
     *          FILE *f;
     *
     *      public:
     *          FileRead();                     // Constructor
     *          void OpenFile(char *);          // Open method
     *          void CloseFile();               // Close method
     *          void WriteData(char *data);     // Data write method
     *      }
     *
     *      // Constructor
     *      FileRead::FileRead() : f(NULL) {
     *          SetErrorClassPrefix("FileRead");
     *      }
     *
     *      // Method OpenFile
     *      FileRead::OpenFile(char *fileName) {
     *          f = fopen(fileName, "w");
     *          if (!f)
     *              // Error occurred
     *              PushError("OpenFile", "Unable to open the data file");
     *      }
     *
     *      // Method CloseFile
     *      FileRead::CloseFile() {
     *          if (!f)
     *              // Error occurred
     *              PushError("CloseFile", "File not opened");
     *          else
     *              fclose(f);
     *      }
     *
     *      ...
     *
     *      // -----------------------------------------------
     *      // USAGE OF THE CLASS
     *      void main() {
     *          FileRead F;
     *          F.OpenFile("myfile");
     *          if (F.HasError()) {
     *              // show the last error
     *              cout << F.GetLastError() << endl;
     *          }
     *
     *          F.CloseFile();
     *
     *          if (F.HasError())
     *              cout << F.PopErrorStack() << endl;
     *      }
     *      \endcode
     */
    class framework_core_class CError {
    private:
        /** \brief Error description prefix.
         *
         * This prefix is inserted at the beginning of any error description pushed into the stack. Tipically,
         * this prefix contains the name of the class pushing the description.
         */
        std::string             _errorClassPrefix;
        /** \brief Error description stack.                                                                 
         *
         * This is the stack containing the list of error mesasges pushed in the object.
         */
        std::stack<std::string> _errorDescriptionStack;    

    protected:
        /**  \brief Sets the prefix of error description.
         *   \param prefix class name
         */
        void SetErrorClassPrefix(std::string prefix);

        /**  \brief Pushes an error message into the stack.
         *   \param methodName name of the calling method
         *   \param description description of the error occurred
         */
        void PushError(std::string methodName, std::string description);
        /** \brief Error propagation method.
         *  \param methodName name of the calling method
         *  \param error_obj pointer to the object that raised has the error
         *
         *  This method allows to propagate the error state of an object by moving its error stack content
         *  into the locak error stack. The object error will be resetted.
         */
        void PropagateError(std::string methodName, CError *error_obj);
        /** \brief Error propagation mechanism inside the same object
         *  \param methodName name of the calling method
         *
         *  This method allows to propagate the error state from inner methods to outer methods of the same object.
         */
        void PropagateError(std::string methodName);

    public:

        CError();                /**< \brief Default constructor.                                    */
        virtual ~CError();       /**< \brief Default destructor.                                     */
        
        bool HasError();                /**< \brief Checks the error state of a class instance.             */
        void ResetError();              /**< \brief Resets the error state.                                 */

        std::string GetLastError();     /**< \brief Gets the last error description string.                 */
        std::string PopLastError();     /**< \brief Pops from the stack the last error description string.  */
        std::string PopErrorStack();    /**< \brief Pops the entire error stack's descriptions and put it 
                                                    into a string.                                          */
        };

    }