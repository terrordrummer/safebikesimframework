#include "windows.h"
#include "FrameworkLUA/CWrapperLua.h"
#include <iostream>
#include <sstream>
#include <map>


#ifdef WIN32
  #define STRINGARG(A) char A[], int len##A
  #define STRINGUSE(A) A, len##A
#else
  #define STRINGARG(A) char A[]
  #define STRINGUSE(A) A
#endif

using namespace FrameworkLUA;
using namespace std;

//#define MAX_STR_LEN 1024

static CWrapperLua * pWrapperLua = NULL ;
//static char Clast_error[MAX_STR_LEN] ;

#define CHECK_INIT(MSG) if ( pWrapperLua == NULL ) { strcpy(Clast_error,"\nF90-LUA Wrapper ERROR, call to " #MSG " with LUA parser not opened" ) ; return false ; }

#ifndef LUADLL_EXPORTS
  #ifdef __GNUC__
    #define DLL_EXPORT_SYMBOL __attribute__((visibility("default")))
  #else
    #define DLL_EXPORT_SYMBOL
  #endif
#else
#ifdef __GNUC__
    #define DLL_EXPORT_SYMBOL __attribute__((visibility("default")))
  #else
    #define DLL_EXPORT_SYMBOL __declspec(dllexport)
  #endif
#endif

#define     cIgnore            0
#define     cCatchAndContinue  1
#define     cCatchAndShow      2


#define cEMan CWrapperLua::EMan

//      _
//  ___| |_   _  __ _     ___  _ __   ___ _ __       _ __   __ _ _ __ ___  ___ _ __
// / __| | | | |/ _` |   / _ \| '_ \ / _ \ '_ \     | '_ \ / _` | '__/ __|/ _ \ '__|
//| (__| | |_| | (_| |  | (_) | |_) |  __/ | | |    | |_) | (_| | |  \__ \  __/ |
// \___|_|\__,_|\__,_|___\___/| .__/ \___|_| |_|____| .__/ \__,_|_|  |___/\___|_|
//                  |_____|   |_|             |_____|_|

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_open_parser() {
  if ( pWrapperLua != NULL ) {
    delete pWrapperLua;
  }

  try {
     pWrapperLua = new CWrapperLua() ;
  }
  catch (...) {
    MessageBox(NULL,TEXT("lua_open_parser() : Cannot allocate LUA parser"),TEXT("FATAL ERROR"),MB_OK);
    return false ;
  }
  return true ;
}

//      _                                                                            _
//  ___| |_   _  __ _     ___  _ __   ___ _ __       _ __   __ _ _ __ ___  ___ _ __ | | ___   __ _
// / __| | | | |/ _` |   / _ \| '_ \ / _ \ '_ \     | '_ \ / _` | '__/ __|/ _ \ '__|| |/ _ \ / _` |
//| (__| | |_| | (_| |  | (_) | |_) |  __/ | | |    | |_) | (_| | |  \__ \  __/ |   | | (_) | (_| |
// \___|_|\__,_|\__,_|___\___/| .__/ \___|_| |_|____| .__/ \__,_|_|  |___/\___|_|___|_|\___/ \__, |
//                  |_____|   |_|             |_____|_|                        |_____|       |___/

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_open_parser_log(STRINGARG(_logFileName)) {
  if ( pWrapperLua != NULL ) {
    MessageBox(NULL,TEXT("lua_open_parser() : Parser already open"),TEXT("FATAL ERROR"),MB_OK);
    return false ;
  }

  try {
     pWrapperLua = new CWrapperLua(_logFileName) ;
  }
  catch (...) {
    MessageBox(NULL,TEXT("lua_open_parser() : Cannot allocate LUA parser"),TEXT("FATAL ERROR"),MB_OK);
    return false ;
  }
  return true ;
}


//      _                                                 __ _ _
//  ___| |_   _  __ _      _ __   __ _ _ __ ___  ___     / _(_) | ___
// / __| | | | |/ _` |    | '_ \ / _` | '__/ __|/ _ \   | |_| | |/ _ \
//| (__| | |_| | (_| |    | |_) | (_| | |  \__ \  __/   |  _| | |  __/
// \___|_|\__,_|\__,_|____| .__/ \__,_|_|  |___/\___|___|_| |_|_|\___|
//                  |_____|_|                      |_____|
                                           
extern "C"
DLL_EXPORT_SYMBOL
bool
clua_parse_file( STRINGARG(file_name) ) {
  return pWrapperLua -> ParseFile( file_name ) ;
}


//      _                                                    _        _
//  ___| |_   _  __ _      _ __   __ _ _ __ ___  ___     ___| |_ _ __(_)_ __   __ _
// / __| | | | |/ _` |    | '_ \ / _` | '__/ __|/ _ \   / __| __| '__| | '_ \ / _` |
//| (__| | |_| | (_| |    | |_) | (_| | |  \__ \  __/   \__ \ |_| |  | | | | | (_| |
// \___|_|\__,_|\__,_|____| .__/ \__,_|_|  |___/\___|___|___/\__|_|  |_|_| |_|\__, |
//                  |_____|_|                      |_____|                    |___/

extern "C" DLL_EXPORT_SYMBOL
bool
clua_parse_string( STRINGARG(cmd) ) {
  pWrapperLua -> DoString( cmd ) ;
  return true ;
}


//      _                            _                _
//  ___| |_   _  __ _      __ _  ___| |_  __   ____ _| |_   _  ___
// / __| | | | |/ _` |    / _` |/ _ \ __| \ \ / / _` | | | | |/ _ \
//| (__| | |_| | (_| |   | (_| |  __/ |_   \ V / (_| | | |_| |  __/
// \___|_|\__,_|\__,_|____\__, |\___|\__|___\_/ \__,_|_|\__,_|\___|
//                  |_____|___/        |_____|

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_get_value( STRINGARG(var), CWrapperLua::_valueType & res ) {
  res = 0 ;
  bool ok = pWrapperLua -> GetValue( var, res ) ;
  if ( !ok ) {
    ostringstream cmd ;
    cmd << "F90-LUA Wrapper ERROR, lua_get_value('" << var << "'), cannot get variable value" ;
  }
  return ok ;
}


//      _                            _           _        _
//  ___| |_   _  __ _      __ _  ___| |_     ___| |_ _ __(_)_ __   __ _
// / __| | | | |/ _` |    / _` |/ _ \ __|   / __| __| '__| | '_ \ / _` |
//| (__| | |_| | (_| |   | (_| |  __/ |_    \__ \ |_| |  | | | | | (_| |
// \___|_|\__,_|\__,_|____\__, |\___|\__|___|___/\__|_|  |_|_| |_|\__, |
//                  |_____|___/        |_____|                    |___/

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_get_string( STRINGARG(var), STRINGARG(res) ) {
  string res1 ;
  bool ok = pWrapperLua -> GetString( var, res1 ) ;
  if ( ok ) {
    strcpy(res,res1.c_str()) ;
  } else {
    ostringstream cmd ;
    cmd << "F90-LUA Wrapper ERROR, lua_get_string('" << var << "'), cannot get variable value" ;
  }
  return ok ;
}

//      _                          _                _
//  ___| |_   _  __ _     ___  ___| |_  __   ____ _| |_   _  ___
// / __| | | | |/ _` |   / __|/ _ \ __| \ \ / / _` | | | | |/ _ \
//| (__| | |_| | (_| |   \__ \  __/ |_   \ V / (_| | | |_| |  __/
// \___|_|\__,_|\__,_|___|___/\___|\__|___\_/ \__,_|_|\__,_|\___|
//                  |_____|          |_____|

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_set_value( STRINGARG(var), CWrapperLua::_valueType const & value ) {
  pWrapperLua -> SetValue( var, value ) ;
  return true ;
}

//      _                          _           _        _
//  ___| |_   _  __ _     ___  ___| |_     ___| |_ _ __(_)_ __   __ _
// / __| | | | |/ _` |   / __|/ _ \ __|   / __| __| '__| | '_ \ / _` |
//| (__| | |_| | (_| |   \__ \  __/ |_    \__ \ |_| |  | | | | | (_| |
// \___|_|\__,_|\__,_|___|___/\___|\__|___|___/\__|_|  |_|_| |_|\__, |
//                  |_____|          |_____|                    |___/

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_set_string( STRINGARG(var), STRINGARG(value)) {
  pWrapperLua -> SetString( var, value ) ;
  return true ;
}

//      _                     _
//  ___| |_   _  __ _     ___| | ___  ___  ___      _ __   __ _ _ __ ___  ___ _ __
// / __| | | | |/ _` |   / __| |/ _ \/ __|/ _ \    | '_ \ / _` | '__/ __|/ _ \ '__|
//| (__| | |_| | (_| |  | (__| | (_) \__ \  __/    | |_) | (_| | |  \__ \  __/ |
// \___|_|\__,_|\__,_|___\___|_|\___/|___/\___|____| .__/ \__,_|_|  |___/\___|_|
//                  |_____|                  |_____|_|

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_close_parser( ) {
  if (pWrapperLua) { delete pWrapperLua; }
  pWrapperLua = NULL ;
    return true ;
}

//      _                            _
//  ___| |_   _  __ _      __ _  ___| |_     ___ _ __ _ __ ___  _ __
// / __| | | | |/ _` |    / _` |/ _ \ __|   / _ \ '__| '__/ _ \| '__|
//| (__| | |_| | (_| |   | (_| |  __/ |_   |  __/ |  | | | (_) | |
// \___|_|\__,_|\__,_|____\__, |\___|\__|___\___|_|  |_|  \___/|_|
//                  |_____|___/        |_____|

extern "C"
DLL_EXPORT_SYMBOL
void
clua_get_error( STRINGARG(msg) ) {
  //strcpy(msg,last_error) ;
}


//      _                          _                                _                _
//  ___| |_   _  __ _     ___  ___| |_     ___ _ __ _ __ ___  _ __ | | _____   _____| |
// / __| | | | |/ _` |   / __|/ _ \ __|   / _ \ '__| '__/ _ \| '__|| |/ _ \ \ / / _ \ |
//| (__| | |_| | (_| |   \__ \  __/ |_   |  __/ |  | | | (_) | |   | |  __/\ V /  __/ |
// \___|_|\__,_|\__,_|___|___/\___|\__|___\___|_|  |_|  \___/|_|___|_|\___| \_/ \___|_|
//                  |_____|          |_____|                  |_____|

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_set_error_level(int i) {
    cEMan Code;
    switch (i) {
        case cIgnore:
            Code=(cEMan)i;
            break;
        case cCatchAndContinue:
            Code=(cEMan)i;
            break;
        case cCatchAndShow:
            Code=(cEMan)i;
            break;
        default:
            Code=(cEMan)0;
    }
    pWrapperLua->SetErrorLevel(Code);
    return true;
}


// _                                                                                _
//| |_   _  __ _     ___ _ __ _ __ ___  _ __  ___   ___ ___ _   _ _ __ _ __ ___  __| |
//| | | | |/ _` |   / _ \ '__| '__/ _ \| '__|/ _ \ / __/ __| | | | '__| '__/ _ \/ _` |
//| | |_| | (_| |  |  __/ |  | | | (_) | |  | (_) | (_| (__| |_| | |  | | |  __/ (_| |
//|_|\__,_|\__,_|___\___|_|  |_|  \___/|_|___\___/ \___\___|\__,_|_|  |_|  \___|\__,_|
//             |_____|                  |_____|

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_error_occurred() {
    return pWrapperLua->ErrorOccurred();
}

//            _
//  __ _  ___| |_     ___ _ __ _ __ ___  _ __  _ __ ___  ___  __ _
// / _` |/ _ \ __|   / _ \ '__| '__/ _ \| '__|| '_ ` _ \/ __|/ _` |
//| (_| |  __/ |_   |  __/ |  | | | (_) | |   | | | | | \__ \ (_| |
// \__, |\___|\__|___\___|_|  |_|  \___/|_|___|_| |_| |_|___/\__, |
// |___/        |_____|                  |_____|             |___/

extern "C"
DLL_EXPORT_SYMBOL
bool
clua_get_error_msg(STRINGARG(msg)) {
    try {
        pWrapperLua->GetErrorMsg(msg);
        return true;
    } catch (...){return false;}
}

//      _                                                            _
//  ___| |_   _  __ _     ___ _ __ _ __ ___  _ __  _ __ ___ ___  ___| |_
// / __| | | | |/ _` |   / _ \ '__| '__/ _ \| '__|| '__/ _ Y __|/ _ \ __|
//| (__| | |_| | (_| |  |  __/ |  | | | (_) | |   | | |  __|__ \  __/ |_
// \___|_|\__,_|\__,_|___\___|_|  |_|  \___/|_|___|_|  \___|___/\___|\__|
//                  |_____|                  |_____|

extern "C"
DLL_EXPORT_SYMBOL
bool 
clua_error_reset(){
    pWrapperLua->ErrorReset();
    return true;
}