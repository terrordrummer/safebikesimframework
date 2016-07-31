#pragma once

#include "FrameworkCanBus/common_defines.h"
#include "FrameworkCanBus/CANBasicTypes.h"
#include "FrameworkCore/CError.h"
#include "FrameworkCore/CTrace.h"
#include "FrameworkCore/CThread.h"
#include <string>

namespace FrameworkCanBus {

    /** \brief CAN basic class 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     * 
     * This class inherit the basic classes for logging data, managing errors and launch 
     * a threaded method.
     * This class defines the following methods:
     * \li a virtual method that returns the description of the class (specialized by all childs)
     *
     * All the CAN classes inherites this class.
     */
    class framework_canbus_class CANBasicClass :    virtual public  FrameworkCore::CError, 
                                                    protected       FrameworkCore::CTrace, 
                                                    protected       FrameworkCore::CThread {
    public:

        /** \brief Class description.
         *  \result class description
         */
        virtual std::string GetDescription();
        
        };

    }