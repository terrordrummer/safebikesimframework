#pragma once

#include "FrameworkCanBus/CANBasicClass.h"
#include "FrameworkCanBus/CANArchitectureManager.h"
#include "FrameworkCanBus/CANEcuManager.h"
#include "FrameworkCanBus/CANMessageManager.h"
#include "FrameworkCanBus/CANSignalManager.h"
#include <vector>
#include <map>
#include <set>
#include <string>

namespace FrameworkCanBus {

    /** \brief DBC wrapper class. 
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class reads a ".dbc" file data created with VECTOR Canalyzer software and offers several methods 
     * to query this data.
     */
    class CDbcWrapper : public CANBasicClass {

    public:
        /** \brief Class constructor.   */
        CDbcWrapper();
        /** \brief Class destructor.    */
        ~CDbcWrapper();

        /** \brief Parse dbc file.       */
        bool ParseDBCFile(  std::string,
                            CANArchitectureManager *architecture_manager,
                            CANEcuManager *_ecuManager,
                            CANMessageManager *_messageManager,
                            CANSignalManager *_signalManager);
        };

    }
