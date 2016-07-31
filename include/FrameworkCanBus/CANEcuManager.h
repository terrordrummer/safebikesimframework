#pragma once

#include "CANBasicClass.h"
#include "CANEcu.h"
#include <map>
#include <string>

namespace FrameworkCanBus {

    /** \brief This class manages the list of ECU existing in the CANBUS architecture.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class collects the ECUs of the CANBUS architecture. It allows to:
     * \li add an ECU to the CANBUS architecture
     * \li remove an ECU from the CANBUS architecture
     * \li obtain an ECU reference by the ECU name
     *
     * ECU manager is just a container of the exsisting ECU in the architecture. Any ECU-specific operation
     * like marking an ECU as transmitter of a message must be done retriving the specific ECU instance pointer
     * and then calling the right ECU instance methods.
     */
    class framework_canbus_class CANEcuManager : public CANBasicClass {
    private:
        /** \brief Map type mapping ECU by names.                       */
        typedef std::map<CANBasicTypes::type_string,CANEcu *> _ecuMapType;

        /** \brief ECU map by name.                                      */
        _ecuMapType _ecuMap;

    public:
        
        /** \brief Class constructor.                                    */
        CANEcuManager();

        /** \brief Class destructor.                                     */
        ~CANEcuManager();

        /** \brief Returns the ECU list.                                 */
        CANEcuTypes::ecu_list_type get_ecu_list() ;

        /** \brief Returns an ECU pointer by its name.
         *  \param _ecuName name of the ECU
         *  \return ECU reference
         */
        CANEcu *get_ecu(CANBasicTypes::type_string _ecuName);
        
        /** \brief Add an ECU to the architecture.
         *  \param _ecuName name of the ECU that has to be added
         *  \return TRUE if ECU has been added
         */
        bool AddECU(CANBasicTypes::type_string _ecuName);

        /** \brief Add the list of ECU of the architecture.
         *  \param ecu_name_list list of ECUs that have to be added
         *  \return TRUE if ECUs have been added
         */
        bool AddECUList(CANEcuTypes::ecu_list_type ecu_name_list);

        /** \brief Check for ECU to exist in the architecture.
         *  \param _ecuName name of the ECU that has to be checked
         *  \return TRUE if ECU already exist in the architecture
         */
        bool ExistECU(CANBasicTypes::type_string _ecuName);

        /** \brief Remove an ECU from the CANBUS architecture.
         *  \param _ecuName name of the ECU that has to be removed
         *  \return TRUE if ECU has been removed
         */
        bool RemoveECU(CANBasicTypes::type_string _ecuName);

        /** \brief Description of ECU manager.
         *  \return ECU management status description
         */
        virtual std::string GetDescription();

        };

    }