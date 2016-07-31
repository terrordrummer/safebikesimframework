#include "FrameworkCore/CError.h"
#include <sstream>

using namespace FrameworkCore;
using namespace std;

/*
  ____ _____                   
 / ___| ____|_ __ _ __ ___  _ __
| |   |  _| | '__| '__/ _ \| '__|
| |___| |___| |  | | | (_) | |  
 \____|_____|_|  |_|  \___/|_| 
*/
CError::CError() {
    ResetError();
    _errorClassPrefix.assign("");
    }

/* 
 /\/|___ _____                    
|/\/ ___| ____|_ __ _ __ ___  _ __
  | |   |  _| | '__| '__/ _ \| '__|
  | |___| |___| |  | | | (_) | |  
   \____|_____|_|  |_|  \___/|_|
*/
CError::~CError() {
    ResetError();
    _errorClassPrefix.assign("");
    }

/*
 ____       _   _____                      ____ _               ____            __ _
/ ___|  ___| |_| ____|_ __ _ __ ___  _ __ / ___| | __ _ ___ ___|  _ \ _ __ ___ / _(_)_  __
\___ \ / _ \ __|  _| | '__| '__/ _ \| '__| |   | |/ _` / __/ __| |_) | '__/ _ \ |_| \ \/ /
 ___) |  __/ |_| |___| |  | | | (_) | |  | |___| | (_| \__ \__ \  __/| | |  __/  _| |>  <
|____/ \___|\__|_____|_|  |_|  \___/|_|   \____|_|\__,_|___/___/_|   |_|  \___|_| |_/_/\_\

*/
void CError::SetErrorClassPrefix(std::string className) {
    _errorClassPrefix = string("[") + className + string("]");
    }

/*
 ____            _     _____
|  _ \ _   _ ___| |__ | ____|_ __ _ __ ___  _ __
| |_) | | | / __| '_ \|  _| | '__| '__/ _ \| '__|
|  __/| |_| \__ \ | | | |___| |  | | | (_) | |
|_|    \__,_|___/_| |_|_____|_|  |_|  \___/|_|

*/
void CError::PushError(std::string methodName, std::string err_msg) {
    _errorDescriptionStack.push(_errorClassPrefix + string("<") + methodName + string("> ") + err_msg);
    }

/*
 ____                                    _       _____
|  _ \ _ __ ___  _ __   __ _  __ _  __ _| |_ ___| ____|_ __ _ __ ___  _ __
| |_) | '__/ _ \| '_ \ / _` |/ _` |/ _` | __/ _ \  _| | '__| '__/ _ \| '__|
|  __/| | | (_) | |_) | (_| | (_| | (_| | ||  __/ |___| |  | | | (_) | |
|_|   |_|  \___/| .__/ \__,_|\__, |\__,_|\__\___|_____|_|  |_|  \___/|_|
                |_|          |___/
*/
void CError::PropagateError(std::string methodName, CError *error_obj) {
    // pop the error stack of object error_obj and push it into the locak stack
    PushError( methodName, error_obj->PopErrorStack());
    // if the error is not propagated inside the same object then reset the error of error_obj
    if (error_obj != this)
        error_obj->ResetError();
}
void CError::PropagateError(std::string methodName) {
    // pop the error stack of object error_obj and push it into the locak stack
    PushError( methodName, PopErrorStack());
}
/*
 _   _           _____
| | | | __ _ ___| ____|_ __ _ __ ___  _ __
| |_| |/ _` / __|  _| | '__| '__/ _ \| '__|
|  _  | (_| \__ \ |___| |  | | | (_) | |
|_| |_|\__,_|___/_____|_|  |_|  \___/|_|
*/
/**
 *  \return error status flag
 *
 * This method returns the error status flag of an object. \n
 * Return value is:
 * \li <b>TRUE</b> if the object is in error state
 * \li <b>FALSE</b>if the object isn't in error state
 *
 * If the error state is \c TRUE then the <b> GetLastError</b>, \b PopLastError or \b PopErrorStack methods can be used to
 * obtain the error messages.
 */
bool CError::HasError() {
    return !_errorDescriptionStack.empty();
    }

/*
 ____               _   _____
|  _ \ ___ ___  ___| |_| ____|_ __ _ __ ___  _ __
| |_) / _ Y __|/ _ \ __|  _| | '__| '__/ _ \| '__|
|  _ <  __|__ \  __/ |_| |___| |  | | | (_) | |
|_| \_\___|___/\___|\__|_____|_|  |_|  \___/|_|
*/
/** 
 * This method resets the error state of the instance and clears the error stack.
 */
void CError::ResetError() {
    while (!_errorDescriptionStack.empty())
        _errorDescriptionStack.pop();
    }

/*
  ____      _   _              _   _____
 / ___| ___| |_| |    __ _ ___| |_| ____|_ __ _ __ ___  _ __
| |  _ / _ \ __| |   / _` / __| __|  _| | '__| '__/ _ \| '__|
| |_| |  __/ |_| |__| (_| \__ \ |_| |___| |  | | | (_) | |
 \____|\___|\__|_____\__,_|___/\__|_____|_|  |_|  \___/|_|
*/
/**
 *  \return error description
 *
 * If an object instance is in error state this method returns the error description without 
 * removnig it from the top of error stack. The instance will remain in error state.
 */
std::string CError::GetLastError() {
    if (_errorDescriptionStack.empty())
        return "";
    else
        return _errorDescriptionStack.top();
    }

/*
 ____             _              _   _____
|  _ \ ___  _ __ | |    __ _ ___| |_| ____|_ __ _ __ ___  _ __
| |_) / _ \| '_ \| |   / _` / __| __|  _| | '__| '__/ _ \| '__|
|  __/ (_) | |_) | |__| (_| \__ \ |_| |___| |  | | | (_) | |
|_|   \___/| .__/|_____\__,_|___/\__|_____|_|  |_|  \___/|_|
           |_|
*/
/**
 *  \return error description
 *
 * This method returns the last error description and remove it from the error stack. If the stack 
 * is empty then an empty string is returned. If the stack results to be empty the status flag is set
 * to FALSE (no error).
 */
std::string CError::PopLastError() {
    std::string elem;
    elem.clear();
    elem = _errorDescriptionStack.top();
    _errorDescriptionStack.pop();
    return elem;
    }

/*
 ____             _____                     ____  _             _
|  _ \ ___  _ __ | ____|_ __ _ __ ___  _ __/ ___|| |_ __ _  ___| | __
| |_) / _ \| '_ \|  _| | '__| '__/ _ \| '__\___ \| __/ _` |/ __| |/ /
|  __/ (_) | |_) | |___| |  | | | (_) | |   ___) | || (_| | (__|   <
|_|   \___/| .__/|_____|_|  |_|  \___/|_|  |____/ \__\__,_|\___|_|\_\
           |_|
*/
/**
 *  \return error stack description
 *
 * This method returns a string containing all the error description stack. If the stack is containing
 * more than one error string then a string separator between each is added. The separator will be
 * \e std::endl. \n At the end the stack of the class instance will be cleared.
 */
std::string CError::PopErrorStack() {
    std::stringstream s;
    s.clear();
    while (!_errorDescriptionStack.empty()) {
        s << _errorDescriptionStack.top() << std::endl;
        _errorDescriptionStack.pop();
        }
    return s.str();
    }
