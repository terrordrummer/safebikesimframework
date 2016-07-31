#include "FrameworkCore/CExtendedString.h"

using namespace FrameworkCore;

/*
  ____ __  __       ____  _        _
 / ___|  \/  |_   _/ ___|| |_ _ __(_)_ __   __ _
| |   | |\/| | | | \___ \| __| '__| | '_ \ / _` |
| |___| |  | | |_| |___) | |_| |  | | | | | (_| |
 \____|_|  |_|\__, |____/ \__|_|  |_|_| |_|\__, |
              |___/                        |___/
*/
CExtendedString::CExtendedString() {
    this->clear();
    };

/*
  ____ __  __       ____  _        _
 / ___|  \/  |_   _/ ___|| |_ _ __(_)_ __   __ _
| |   | |\/| | | | \___ \| __| '__| | '_ \ / _` |
| |___| |  | | |_| |___) | |_| |  | | | | | (_| |
 \____|_|  |_|\__, |____/ \__|_|  |_|_| |_|\__, |
              |___/                        |___/
*/
/**
 *  \param str another CExtendedString class
 */
//CExtendedString::CExtendedString(CExtendedString str) {
//    this->assign(str.c_str());
//    }

/*
  ____ __  __       ____  _        _
 / ___|  \/  |_   _/ ___|| |_ _ __(_)_ __   __ _
| |   | |\/| | | | \___ \| __| '__| | '_ \ / _` |
| |___| |  | | |_| |___) | |_| |  | | | | | (_| |
 \____|_|  |_|\__, |____/ \__|_|  |_|_| |_|\__, |
              |___/                        |___/
*/
/**      
 *  \param str constructor from c strings
 */
CExtendedString::CExtendedString(const char* str) {
    this->assign(str);
    };

/*
  ____ __  __       ____  _        _
 / ___|  \/  |_   _/ ___|| |_ _ __(_)_ __   __ _
| |   | |\/| | | | \___ \| __| '__| | '_ \ / _` |
| |___| |  | | |_| |___) | |_| |  | | | | | (_| |
 \____|_|  |_|\__, |____/ \__|_|  |_|_| |_|\__, |
              |___/                        |___/
*/
/**    
 *  \param str constructor from a string object
 */
CExtendedString::CExtendedString(std::string &str) {
    this->assign(str.c_str());
    };

/*
 ____  _        _
/ ___|| |_ _ __(_)_ __
\___ \| __| '__| | '_ \
 ___) | |_| |  | | |_) |
|____/ \__|_|  |_| .__/
                 |_|
*/
/**
 *  \return stripped string
 *
 * This method returns a copy of the string removed by the blank characters at the beginning 
 * and the end (if present).
 */
CExtendedString CExtendedString::Strip() {
    std::string local = *this;
    // empty string case
    if (local.length()==0)
        return local;

    unsigned int istart = 0;
    unsigned int iend = (unsigned int)local.length();

    while (local[istart]==' ' && istart != iend)
        istart ++;
    while (local[iend]==' ' && istart != iend)
        iend--;
    
    return local.substr(istart,iend-istart+1);
    };
