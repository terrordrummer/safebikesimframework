#pragma once 
#pragma warning (disable : 4996)    // Remove strcpy deprecated warning

#include "FrameworkLUA/LUA/lua.h"
#include "FrameworkLUA/LUA/lualib.h"
#include "FrameworkLUA/LUA/lauxlib.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <stdexcept>
#include "FrameworkLUA/common_defines.h"

/** \brief LUA script language.
 *
 * This namespace contains the classes useful to integrate the LUA scripting language.
 */
namespace FrameworkLUA {

    #define ID_DOSTRING             0
    #define ID_PARSEFILE            1
    #define ID_GETSTRUCT            2
    #define ID_NOTATABLE            3
    #define ID_NOTAVALIDTABLEMEMBER 4
    #define ID_NOTASTRING           5
    #define ID_STRINGNOTFOUND       6
    #define ID_VALUENOTFOUND        7
    #define ID_NOTAVALIDVALUE       8
    #define ID_PARSERALREADYOPEN    9


    /** \brief Wrapper lua class.
     *
     * This class implements an easy-to-use interface to the LUA language. A wrapper is an environment which
     * can parse lua commands and files and from which it is possible to get values as c basic types.
     * \n
     * Tipical usage is as explained in the following example:
     * \code
     * CWrapperLua WL;              // declare the wrapper
     * WL.ParseFile("test.lua");    // parse a lua script file, suppose that in the file there is the instruction "a=5";
     * WL.SetValue("b",10);         // assign b = 10 inside the lua environment
     * WL.DoString("c=a+b");        // compute c variabile as a+b
     * int c;
     * WL.GetInt("c",c);            // extract c value
     * \endcode
     * It is in general possible to execute commands by parsing text files containing lua scripts by calling
     * \b ParseFile method or parsing command strings using the \b DoString method. When necessary any kind of value
     * can be extracted by using the getter methods.
     */
    class framework_lua_class CWrapperLua {
    public:
        typedef int                                   _indexType ;      /**< \brief Index type.             */
        typedef double                                _valueType ;      /**< \brief Value type.             */
        typedef std::map<std::string,double>          _mapValues ;      /**< \brief Map of values type.     */
        typedef std::map<std::string,std::string>     _mapStrings ;     /**< \brief Map of strings type.    */
        
        /** \brief List of error management behaviours.     */
        typedef enum {
            DontLog=0,          /**< \brief Ignore all errors and avoid to log.                             */
            Ignore=1,           /**< \brief Errors are only logged to file.                                 */
            CatchAndContinue=2, /**< \brief Error messages are appended and return code can signal error.   */
            CatchAndShow=3      /**< \brief Error messages are displayed in a modal window.                 */
        } EMan;

    private:
        lua_State *L ;

        CWrapperLua( CWrapperLua const & );
        CWrapperLua const & operator = ( CWrapperLua const & );

        void PushValue( char const var[], _valueType const value );
        void PushValue( char const a[], char const b[] );
        void PushValue( _valueType const idx, _valueType const value );
        void Error(int ID_Call, std::string const &var);

        EMan                _errorLevel;
        bool                _hasError;
        bool                _firstLogWrite;
        std::ostringstream  _errorMsg;
        std::fstream        _logFile;
        int                 _errCounter;
        char                _logFileName[1024];

    public:

        /** \brief Default constructor.                                                                 */
        explicit CWrapperLua() ;
        /** \brief Constructor from a lua script file.                                                  */
        explicit CWrapperLua(const char *pLogFileName);
        /** \brief Default destructor.                                                                  */
        virtual ~CWrapperLua() ;

        /** \brief Assign a numeric value into the environment.                                         */
        void SetValue   ( std::string const & var, _valueType const     value ) ;
        /** \brief Assign a string variable into the environment.                                       */
        void SetString  ( std::string const & var, std::string const &  value ) ;

        /** \brief Get the value of a numeric variable.                                                 */
        bool GetValue   ( std::string const & var, _valueType &         value ) ;
        /** \brief Get a double value of a numeric variable.                                            */
        bool GetDouble  ( std::string const & var, double &             value ) ;
        /** \brief Get a float value of a numeric variable.                                             */
        bool GetFloat   ( std::string const & var, float &              value ) ;
        /** \brief Get an int value of a numeric variable.                                              */
        bool GetInt     ( std::string const & var, int  &               value ) ;
        /** \brief Get a bool value of a variable (if numeric, 0=false, true otherwise)                 */
        bool GetBool    ( std::string const & var, bool  &              value ) ;
        /** \brief Get the string content of a string variable.                                         */
        bool GetString ( std::string const & var, std::string &         value ) ;
        /** \brief Extract a struct as a map of strings.                                                */
        void GetStruct ( std::string const & var,
                         _mapValues   & vars_value, 
                         _mapStrings  & vars_string ) ;

        /** \brief Parse a lua script file.                                                             */
        bool ParseFile ( std::string const & project_name ) ;
        /** \brief Parse a lua script string.                                                           */
        void DoString  ( std::string const & cmd ) ;

        /** \brief Sets the error detection level.                                                      */
        void SetErrorLevel ( EMan Code ) ;
        /** \brief Check the error status of the wrapper.                                               */
        bool ErrorOccurred();

        /** \brief Get the error description.                                                           */
        void GetErrorMsg(std::string &Res);
        /** \brief Get the error description.                                                           */
        void GetErrorMsg(char Res[]);
        /** \brief Reset the error state of the wrapper.                                                */
        void ErrorReset();

    };

}

