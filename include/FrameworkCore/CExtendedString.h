#pragma once

#include <string>
#include "FrameworkCore/common_defines.h"

namespace FrameworkCore {
    /** \brief This class extends the standard \e std::string type.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class extends the standard string class adding the following: \n
     * \li a method called "strip" which returns a copy of the string removing by the blank characters 
     *     at the beginning and the end (if present).
     */
    class framework_core_class CExtendedString : public std::string {
    public :
        
        CExtendedString();                      /**< \brief Class constructor.                                  */
        CExtendedString(const char* str);       /**< \brief Class constructor from char *.                       */
        CExtendedString(std::string &str);      /**< \brief Class constructor from std::string.                  */
        CExtendedString Strip();                /**< \brief Removes white characters at the beginning and at the 
                                                            end of the string.                                   */

        };
    };