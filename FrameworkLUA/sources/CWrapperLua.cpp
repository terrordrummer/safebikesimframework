#pragma warning(disable: 4786)

#include "Windows.h"
#include "FrameworkLUA/CWrapperLua.h"
#include <iostream>
#include <sstream>


using namespace std;
using namespace FrameworkLUA;



/* ____            _  __     __    _
  |  _ \ _   _ ___| |_\ \   / /_ _| |_   _  ___
  | |_) | | | / __| '_ \ \ / / _` | | | | |/ _ \
  |  __/| |_| \__ \ | | \ V / (_| | | |_| |  __/
  |_|    \__,_|___/_| |_|\_/ \__,_|_|\__,_|\___|
*/
void CWrapperLua::PushValue( char const var[], _valueType const value ) {
    lua_pushstring(L,var);
    lua_pushnumber(L,value);
    lua_settable(L,-3);
}

void CWrapperLua::PushValue( char const a[], char const b[] ) {
    lua_pushstring(L,a);
    lua_pushstring(L,b);
    lua_settable(L,-3);
}

void CWrapperLua::PushValue( _valueType const idx, _valueType const value ) {
    lua_pushnumber(L,idx);
    lua_pushnumber(L,value);
    lua_settable(L,-3);
}

/*
   _____
  | ____|_ __ _ __ ___  _ __
  |  _| | '__| '__/ _ \| '__|
  | |___| |  | | | (_) | |
  |_____|_|  |_|  \___/|_|
*/
void CWrapperLua::Error(int ID_Call, string const &var) {

    ostringstream Intest;
    ostringstream OldErrMsg;
    ostringstream Tmp;

    if (_errorLevel==DontLog)
        return;

    OldErrMsg.str("");
    Tmp.str("");

    OldErrMsg << _errorMsg.str();
    _errorMsg.str("");
    _errCounter++;

    // Open Log file
    if (_firstLogWrite)
    {
        _logFile.open(_logFileName,fstream::out | fstream::ate | fstream::trunc);
        _firstLogWrite=false;
    }
    else
        _logFile.open(_logFileName,fstream::out | fstream::app);

    switch (ID_Call) {
    case ID_DOSTRING:
        Intest << "Cannot run command '" << var << "'. ";
        break;
    case ID_PARSEFILE:
        Intest << "Cannot parse file '" << var << "'. ";
        break;
    case ID_GETSTRUCT:
        Intest << "Cannot get struct '" << var << "'. ";
        break;
    case ID_NOTATABLE:
        Intest << var << " is not a table. ";
        break;
    case ID_NOTAVALIDTABLEMEMBER:
        _errorMsg << "Struct '" << var << "' contains a non-(string,boolean,number) element.\n";
        goto write_log;
    case ID_NOTASTRING:
        Intest << "'" << var << "' is not a string. ";
        break;
    case ID_STRINGNOTFOUND:
        Intest << "String '" << var << "' not found. ";
        break;
    case ID_VALUENOTFOUND:
        Intest << "Value '" << var << "' not found. ";
        break;
    case ID_NOTAVALIDVALUE:
        _errorMsg << "Value '" << var << "' contains a non-(integer,boolean) data.\n";
        break;
    case ID_PARSERALREADYOPEN:
        _errorMsg << "lua_open_parser() : Parser already open";
        goto write_log;
    }
    // Append Lua error message
    try {
          _errorMsg << Intest.str();
        const char *LuaErr;
        LuaErr=lua_tostring(L, -1);
        if (LuaErr!=NULL)
        {
            Tmp << LuaErr << "\n" ;
            _errorMsg << Tmp.str();
        }
        else
            _errorMsg << "\n";
    } catch (...){ _errorMsg << "\n"; }

write_log:
    // Write LogError
    _logFile << _errCounter << " -- " << _errorMsg.str();
    _logFile.close();

    if (_errorLevel!=Ignore)
    {
        // Error occurred
        _hasError=true;
        // If CatchAndShow then show error message
        if (_errorLevel==CatchAndShow)
             try{MessageBox(NULL,_errorMsg.str().c_str(),TEXT("Error"),MB_OK);}catch(...){}
        else
        {
            // CatchAndContinue : append the error message to the previous
            OldErrMsg << _errorMsg.str();
            _errorMsg.str("");
            _errorMsg << OldErrMsg.str();
        }
    }
}

/*
   ____        _  __     __     _            
  / ___|  ___ | |_\ \   / /__ _| |_   _  ___ 
  \___ \ / _ \| __|\ \ / // _` | | | | |/ _ \
   ___) |  __/| |_  \ V /| (_| | | |_| |  __/
  |____/ \___| \__|  \_/  \__,_|_|\__,_|\___|
*/
/**
 *  \param var variable name
 *  \param value value of the variable to assign
 *
 * This function assigns a numeric value to a variable inside the wrapper environment.
 */
void CWrapperLua::SetValue( const std::string & var, const _valueType value ) {
    ostringstream cmd ;
    cmd << var << '=' << value << '\n';
    DoString( cmd . str() . c_str() ) ;
}

/*
   ____       _   ____  _        _             
  / ___|  ___| |_/ ___|| |_ _ __(_)_ __   __ _ 
  \___ \ / _ \ __\___ \| __| '__| | '_ \ / _` |
   ___) |  __/ |_ ___) | |_| |  | | | | | (_| |
  |____/ \___|\__|____/ \__|_|  |_|_| |_|\__, |
                                         |___/ 
*/
/**
 *  \param var variable name
 *  \param value text of the string variable to assign
 *
 * This function assigns a string text to a variable inside the wrapper environment.
 */  
void CWrapperLua::SetString( const std::string & var, const std::string & value ) {
    ostringstream cmd ;
    cmd << var << "='" << value << "'" ;
    DoString( cmd . str() . c_str() ) ;
}

/*                                     
      ____       _  __     __     _            
     / ___| ___ | |_\ \   / /__ _| |_   _  ___ 
    | |  _ / _ \| __|\ \ / // _` | | | | |/ _ \
    | |_| |  __/| |_  \ V /| (_| | | |_| |  __/
     \____|\___| \__|  \_/  \__,_|_|\__,_|\___|
*/
/**
 *  \param var variable name
 *  \param value variable that will contain the value
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This function gets a numeric value from a variable inside the wrapper environment. If the operation has success
 * the value is assigned to the \c value param, otherwise \c value is unchanged.
 */
bool CWrapperLua::GetValue( const std::string & var, _valueType & value ) {
    ostringstream cmd ;
    cmd << "XOPTIMA_WORK_INTERVAL_VALUE=" << var ;
    DoString( cmd . str() . c_str() ) ;
    lua_pushstring(L, "XOPTIMA_WORK_INTERVAL_VALUE");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if ( lua_isnil(L, -1) ) 
    {
        Error(ID_VALUENOTFOUND,var);
        return false ;
    }
    switch( lua_type(L,-1) ) {
        case LUA_TNUMBER:
            value = lua_tonumber(L, -1) ;
            break;
        case LUA_TBOOLEAN:
            value = lua_toboolean(L, -1) ;
            break;
        default:
            Error(ID_NOTAVALIDVALUE,var);
            lua_pop(L, 1);
            return false ;
    }
    lua_pop(L, 1);
    return true ;
}

/*
    ____      _   ____              _     _
   / ___| ___| |_|  _ \  ___  _   _| |__ | | ___
  | |  _ / _ \ __| | | |/ _ \| | | | '_ \| |/ _ \
  | |_| |  __/ |_| |_| | (_) | |_| | |_) | |  __/
   \____|\___|\__|____/ \___/ \__,_|_.__/|_|\___|
*/
/**
 *  \param var variable name
 *  \param value double variable that will contain the value
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This function gets a numeric value from a variable inside the wrapper environment. If the operation has success
 * the value is assigned to the \c value param, otherwise \c value is unchanged.
 */
bool CWrapperLua::GetDouble( const std::string & var, double & value ) {
    ostringstream cmd ;
    cmd << "XOPTIMA_WORK_INTERVAL_VALUE=" << var ;
    DoString( cmd . str() . c_str() ) ;
    lua_pushstring(L, "XOPTIMA_WORK_INTERVAL_VALUE");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if ( lua_isnil(L, -1) ) 
    {
        Error(ID_VALUENOTFOUND,var);
        return false ;
    }
    switch( lua_type(L,-1) ) {
        case LUA_TNUMBER:
            value = (double)lua_tonumber(L, -1) ;
            break;
        default:
            Error(ID_NOTAVALIDVALUE,var);
            lua_pop(L, 1);
            return false ;
    }
    lua_pop(L, 1);
    return true ;
}
/*
      ____      _   _____ _             _
     / ___| ___| |_|  ___| | ___   __ _| |_
    | |  _ / _ \ __| |_  | |/ _ \ / _` | __|
    | |_| |  __/ |_|  _| | | (_) | (_| | |_
     \____|\___|\__|_|   |_|\___/ \__,_|\__|
*/
/**
 *  \param var variable name
 *  \param value float variable that will contain the value
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This function gets a numeric value from a variable inside the wrapper environment. If the operation has success
 * the value is assigned to the \c value param, otherwise \c value is unchanged.
 */
bool CWrapperLua::GetFloat( const std::string & var, float & value ) {
    ostringstream cmd ;
    cmd << "XOPTIMA_WORK_INTERVAL_VALUE=" << var ;
    DoString( cmd . str() . c_str() ) ;
    lua_pushstring(L, "XOPTIMA_WORK_INTERVAL_VALUE");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if ( lua_isnil(L, -1) ) 
    {
        Error(ID_VALUENOTFOUND,var);
        return false ;
    }
    switch( lua_type(L,-1) ) {
        case LUA_TNUMBER:
            value = (float)lua_tonumber(L, -1) ;
            break;
        default:
            Error(ID_NOTAVALIDVALUE,var);
            lua_pop(L, 1);
            return false ;
    }
    lua_pop(L, 1);
    return true ;
}

/*
      ____      _   ___       _
     / ___| ___| |_|_ _|_ __ | |_
    | |  _ / _ \ __|| || '_ \| __|
    | |_| |  __/ |_ | || | | | |_
     \____|\___|\__|___|_| |_|\__|
*/
/**
 *  \param var variable name
 *  \param value int variable that will contain the value
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This function gets a numeric value from a variable inside the wrapper environment. If the operation has success
 * the value is assigned to the \c value param, otherwise \c value is unchanged.
 */
bool
CWrapperLua::GetInt( const std::string & var, int & value ) {
    ostringstream cmd ;
    cmd << "XOPTIMA_WORK_INTERVAL_VALUE=" << var ;
    DoString( cmd . str() . c_str() ) ;
    lua_pushstring(L, "XOPTIMA_WORK_INTERVAL_VALUE");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if ( lua_isnil(L, -1) ) 
    {
        Error(ID_VALUENOTFOUND,var);
        return false ;
    }
    switch( lua_type(L,-1) ) {
        case LUA_TNUMBER:
            value = (int)lua_tonumber(L, -1) ;
            break;
        default:
            Error(ID_NOTAVALIDVALUE,var);
            lua_pop(L, 1);
            return false ;
    }
    lua_pop(L, 1);
    return true ;
}

/*
      ____      _   ____              _
     / ___| ___| |_| __ )  ___   ___ | |
    | |  _ / _ \ __|  _ \ / _ \ / _ \| |
    | |_| |  __/ |_| |_) | (_) | (_) | |
     \____|\___|\__|____/ \___/ \___/|_|
*/
/**
 *  \param var variable name
 *  \param value bool variable that will contain the value
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This function gets a numeric value from a variable inside the wrapper environment. If the operation has success
 * the value is assigned to the \c value param, otherwise \c value is unchanged. \n
 * Lua will consider \b FALSE a numeric value of zero, \b TRUE any other numeric value.
 */
bool
CWrapperLua::GetBool( const std::string & var, bool & value ) {
    ostringstream cmd ;
    cmd << "XOPTIMA_WORK_INTERVAL_VALUE=" << var ;
    DoString( cmd . str() . c_str() ) ;
    lua_pushstring(L, "XOPTIMA_WORK_INTERVAL_VALUE");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if ( lua_isnil(L, -1) ) 
    {
        Error(ID_VALUENOTFOUND,var);
        return false ;
    }
    switch( lua_type(L,-1) ) {
        case LUA_TBOOLEAN:
            value = (bool)lua_toboolean(L, -1) ;
            break;
        default:
            Error(ID_NOTAVALIDVALUE,var);
            lua_pop(L, 1);
            return false ;
    }
    lua_pop(L, 1);
    return true ;
}

/*    ____      _   ____  _        _             
     / ___| ___| |_/ ___|| |_ _ __(_)_ __   __ _ 
    | |  _ / _ \ __\___ \| __| '__| | '_ \ / _` |
    | |_| |  __/ |_ ___) | |_| |  | | | | | (_| |
     \____|\___|\__|____/ \__|_|  |_|_| |_|\__, |
                                           |___/ 
*/
/**
 *  \param var variable name
 *  \param value string variable that will contain the value
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This function gets a string from a variable inside the wrapper environment. If the operation has success
 * the value is assigned to the \c value param, otherwise \c value is unchanged.
 */
bool
CWrapperLua::GetString( const std::string & var, std::string & value ) {
    ostringstream cmd ;
    cmd << "XOPTIMA_WORK_INTERVAL_VALUE=" << var ;
    DoString( cmd . str() . c_str() ) ;
    lua_pushstring(L, "XOPTIMA_WORK_INTERVAL_VALUE");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if ( lua_isnil(L, -1) ) 
    {
        Error(ID_STRINGNOTFOUND,var);
        return false ;
    }
    switch( lua_type(L,-1) ) {
        case LUA_TSTRING:    
            value = lua_tostring(L, -1) ;
            lua_pop(L, 1);
            return true ;
        default:
            Error(ID_NOTASTRING,var);
            lua_pop(L, 1);
            return false ;
    }
}


/*    ____      _   ____  _                   _   
     / ___| ___| |_/ ___|| |_ _ __ _   _  ___| |_ 
    | |  _ / _ \ __\___ \| __| '__| | | |/ __| __|
    | |_| |  __/ |_ ___) | |_| |  | |_| | (__| |_ 
     \____|\___|\__|____/ \__|_|   \__,_|\___|\__|
*/
/**
 *  \param var variable name
 *  \param vars_value map containing all numeric values
 *  \param vars_string map containing all string values
 *
 * This function gets a struct from  the wrapper environment. The struct is returned as a set of
 * numeric values and string values. Any other kind of field will be discharged.
 */
void
CWrapperLua::GetStruct( const std::string   & var,
                        _mapValues          & vars_value,
                        _mapStrings         & vars_string ) 
{
  
    lua_pushstring (L, var . c_str() );
    lua_gettable   (L, LUA_GLOBALSINDEX);
  
    if ( lua_isnil(L, -1) )
    {
        Error(ID_GETSTRUCT,var.c_str());
        return;
    }
    if (!lua_istable(L,-1))  
    {
        Error(ID_NOTATABLE,var);
    } else 
    {
        lua_pushnil(L) ;  // first key
        lua_gettable(L,-2) ; // table is in the stack at index `t' */
        while ( lua_next(L,-2) != 0 ) 
        {
            switch( lua_type(L,-1) ) 
            {
                case LUA_TNUMBER:
                    vars_value[lua_tostring(L,-2)] = lua_tonumber(L, -1) ;
                    break ;
                case LUA_TBOOLEAN:
                    vars_value[lua_tostring(L,-2)] = lua_toboolean(L, -1) ;
                    break ;
                case LUA_TSTRING:    
                    vars_string[lua_tostring(L,-2)] = lua_tostring(L, -1) ;
                    break ;
                default:
                    Error(ID_NOTAVALIDTABLEMEMBER,var);
            }
            lua_pop(L, 1);  // removes `value'; keeps `key' for next iteration
        }
    }
    lua_settop(L, 0);
}

/*
     ____                         _____  _  _      
    |  _ \  __ _ _ __  ___   ___ |  ___|(_)| | ___ 
    | |_) |/ _` | '__|/ __| / _ \| |_   | || |/ _ \
    |  __/| (_| | |   \__ \|  __/|  _|  | || |  __/
    |_|    \__,_|_|   |___/ \___||_|    |_||_|\___|
*/

/**
 *  \param file_name name of the file to parse
 *  \return \b TRUE on success, \b FALSE otherwise
 *
 * This method parses a file containing a lua script text. If the file contains errors then depending on the 
 * error management status the result can signal errors in the file.
 */
bool
CWrapperLua::ParseFile( const std::string & file_name )
{
    if ( luaL_loadfile(L, file_name.c_str()) || lua_pcall(L, 0, 0, 0) )
    {
        Error(ID_PARSEFILE,file_name);
        return false;
    }
    return true;
}

/*
     ____        ____  _        _             
    |  _ \  ___ / ___|| |_ _ __(_)_ __   __ _ 
    | | | |/ _ \\___ \| __| '__| | '_ \ / _` |
    | |_| | (_) |___) | |_| |  | | | | | (_| |
    |____/ \___/|____/ \__|_|  |_|_| |_|\__, |
                                        |___/ 
*/
/**
 *  \param cmd command to parse
 *
 * This method parses a string containing a lua script.
 */
void
CWrapperLua::DoString( const std::string & cmd ) {
    if ( luaL_loadbuffer(L, cmd . c_str(), cmd . length(), "line") || lua_pcall(L, 0, 0, 0) ) 
        Error(ID_DOSTRING,cmd);
}

/*
   ____       _   _____                     _                   _
  / ___|  ___| |_| ____|_ __ _ __ ___  _ __| |    _____   _____| |
  \___ \ / _ \ __|  _| | '__| '__/ _ \| '__| |   / _ \ \ / / _ \ |
   ___) |  __/ |_| |___| |  | | | (_) | |  | |__|  __/\ V /  __/ |
  |____/ \___|\__|_____|_|  |_|  \___/|_|  |_____\___| \_/ \___|_|
*/
/**
 *  \param Code error managment code
 *
 * This method sets the error management of the wrapper. Look EMan for more details.
 */
void CWrapperLua::SetErrorLevel ( EMan Code ) {
    EMan OldErrLev;
    OldErrLev=_errorLevel;
    try {_errorLevel=Code;} catch(...){_errorLevel=OldErrLev;}
}

/*
   _____                      ___                                   _
  | ____|_ __ _ __ ___  _ __ / _ \  ___ ___ _   _ _ __ _ __ ___  __| |
  |  _| | '__| '__/ _ \| '__| | | |/ __/ __| | | | '__| '__/ _ \/ _` |
  | |___| |  | | | (_) | |  | |_| | (_| (__| |_| | |  | | |  __/ (_| |
  |_____|_|  |_|  \___/|_|   \___/ \___\___|\__,_|_|  |_|  \___|\__,_|
*/
/**
 *  \return Error state of the wrapper
 *
 * This method checks if the wrapper detected some errors. If the return value is \b TRUE then call GetErrorMsg
 * to have more details about the error.
 */
bool CWrapperLua::ErrorOccurred() {
    return _hasError;
}

/*
    ____      _   _____                     __  __
   / ___| ___| |_| ____|_ __ _ __ ___  _ __|  \/  |___  __ _
  | |  _ / _ \ __|  _| | '__| '__/ _ \| '__| |\/| / __|/ _` |
  | |_| |  __/ |_| |___| |  | | | (_) | |  | |  | \__ \ (_| |
   \____|\___|\__|_____|_|  |_|  \___/|_|  |_|  |_|___/\__, |
                                                       |___/
*/
/**
 *  \param Res message containing the error description
 *
 * Call this method if ErrorOccurred method returned \b TRUE.
 */
void CWrapperLua::GetErrorMsg(std::string &Res) {

    Res=string("");
    Res.append(_errorMsg.str());
}

/*
    ____      _   _____                     __  __
   / ___| ___| |_| ____|_ __ _ __ ___  _ __|  \/  |___  __ _
  | |  _ / _ \ __|  _| | '__| '__/ _ \| '__| |\/| / __|/ _` |
  | |_| |  __/ |_| |___| |  | | | (_) | |  | |  | \__ \ (_| |
   \____|\___|\__|_____|_|  |_|  \___/|_|  |_|  |_|___/\__, |
                                                       |___/
*/
/**
 *  \param Res message containing the error description
 *
 * Call this method if ErrorOccurred method returned \b TRUE.
 */
void CWrapperLua::GetErrorMsg(char Res[]) {
    strcpy(Res,_errorMsg.str().c_str());
}

/*
   _____                     ____               _
  | ____|_ __ _ __ ___  _ __|  _ \ ___ ___  ___| |_
  |  _| | '__| '__/ _ \| '__| |_) / _ Y __|/ _ \ __|
  | |___| |  | | | (_) | |  |  _ <  __|__ \  __/ |_
  |_____|_|  |_|  \___/|_|  |_| \_\___|___/\___|\__|
*/
/**
 * Call this method to reset the error state of the wrapper.
 */
void CWrapperLua::ErrorReset() {
    _hasError=false;
    _errorMsg.str("");
}

/*
      ____                   _                   _              
     / ___| ___  _ __   ___ | |_ _ __ _   _  ___| |_  ___  _ __ 
    | |    / _ \| '_ \ / __|| __| '__| | | |/ __| __|/ _ \| '__|
    | |___| (_) | | | |\__ \| |_| |  | |_| | (__| |_| (_) | |   
     \____|\___/|_| |_||___/ \__|_|   \__,_|\___|\__|\___/|_|
*/
CWrapperLua::CWrapperLua() {

    L = lua_open();   /* opens Lua */
  
    luaopen_io(L); // provides io.*
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_loadlib(L);

    // Lua Error
    DoString("local ErrMsg");
    _errorLevel=CatchAndShow;
    _hasError=false;
    _firstLogWrite=true;
    strcpy(_logFileName,"Lua.log");

    _errCounter=0;
    // esempi di variabili da aggiungere all'enviromnet di LUA
    //int GREATER_THAN = 1 ;
    //int LESS_THAN = 1 ;
    //ADD_VALUE_TO_LUA( GREATER_THAN ) ;
    //ADD_VALUE_TO_LUA( LESS_THAN ) ;
    //DoString(cmds) ;
}

/*
      ____                   _                   _              
     / ___| ___  _ __   ___ | |_ _ __ _   _  ___| |_  ___  _ __ 
    | |    / _ \| '_ \ / __|| __| '__| | | |/ __| __|/ _ \| '__|
    | |___| (_) | | | |\__ \| |_| |  | |_| | (__| |_| (_) | |   
     \____|\___/|_| |_||___/ \__|_|   \__,_|\___|\__|\___/|_|
*/
CWrapperLua::CWrapperLua(const char *pLogFileName) {

    L = lua_open();   /* opens Lua */
  
    luaopen_io(L); // provides io.*
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_loadlib(L);

    // Lua Error
    DoString("local ErrMsg");
    _errorLevel=CatchAndShow;
    _hasError=false;
    strcpy(_logFileName,pLogFileName);

    _errCounter=0;
}

/*
     ____             _                   _              
    |  _ \  ___  ___ | |_ _ __ _   _  ___| |_  ___  _ __ 
    | | | |/ _ \/ __|| __| '__| | | |/ __| __|/ _ \| '__|
    | |_| |  __/\__ \| |_| |  | |_| | (__| |_| (_) | |   
    |____/ \___||___/ \__|_|   \__,_|\___|\__|\___/|_|   
*/
CWrapperLua::~CWrapperLua() {
  lua_close(L);
  _logFile.close();
}
