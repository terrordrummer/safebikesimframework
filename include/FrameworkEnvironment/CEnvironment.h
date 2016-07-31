#pragma once

#include <map>
#include <list>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "FrameworkEnvironment/common_defines.h"
#include "FrameworkCore/CGenericType.h"
#include "FrameworkCore/CTime.h"
#include "FrameworkCore/CSynch.h"

/** \brief Environment classes namespace.
 *
 * This namespace contains the classes that manage and _environment container.
 */
namespace FrameworkEnvironment {
    
    /** \brief Environment class.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements the management of a data environment which is a container of 
     * variables. User can manage the environment at runtime in the following manner:
     *
     * \li get/set/remove variables in the _environment
     * \li check the existence of a variable
     * \li get the list of existing variables
     * \li merge different environments
     * \li save/load _environment
     * \li log environment changes in time
     * \li lock/unlock environment for synchronizing multithread access to the data
     *
     * An environment is useful to create a data container that cn be saved, loaded
     * and traced.
     */
    class framework_environment_class CEnvironment {
    protected:
        /** \brief type of _environment container                               */
        typedef std::map<std::string, FrameworkCore::CGenericType> _environmentType;
        /** \brief Environment variable.                                        */
        _environmentType _environment;

    private:
        /** \brief Element used for logging an _environment assignment.         */
        class CLogElement {
        public:
            /** \brief Time at which the data has changed.                      */
            FrameworkCore::CTime::time_type _logTime;   
            /** \brief Name of the variable that has changed.                   */
            std::string                     _name;
            /** \brief New data value.                                          */
            FrameworkCore::CGenericType     _element;
            };

        /** \brief Logging flag.                                                */
        bool _logDataFlag;
        /** \brief Logging to file flag.                                        */
        bool _logDataToTextFileFlag;
        /** \brief logging buffer.                                              */
        std::list<CLogElement> _logData;
        /** \brief file output stream for direct logging to file.               */
        std::ofstream _logFout;
        /** \brief Time counter for logging data.                               */
        FrameworkCore::CTime _time;
        /** \brief Synchronization variable.                                    */
        FrameworkCore::CSynch _synch;

        // OBJECTS TO TRACE
        /** \brief map type between object name and data.                       */
        typedef std::map<std::string, FrameworkCore::CGenericType>   t_candidateMap;
        /** \brief map type between object name and data.                       */
        typedef std::map<std::string, std::fstream *>      t_telemetryMap;
        /** \brief map type between object name and data.                       */
        t_candidateMap                        _candidateObjects;
        /** \brief map type between object name and data.                       */
        t_telemetryMap                        _telemetry;

    public:
        /** \brief Loggin type enumeration.                                     */
        typedef enum {
            RESET_LOGGING_DATA,     /**< \brief Logging data is resetted.       */
            APPEND_LOGGING_DATA     /**< \brief Logging data is appended.       */
            } LOGGINGTYPE;

        /** \brief Default constructor.                                         */
        CEnvironment();
        /** \brief Default destructor.                                          */
        ~CEnvironment();

        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, FrameworkCore::CGenericType &var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, unsigned char var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, char var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, unsigned short var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, short var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, unsigned int var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, int var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, unsigned long var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, long var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, unsigned long long var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, long long var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, float var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, double var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, long double var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, const char *var);
        /** \brief Set a variable to the _environment.                          */
        void SetData(std::string _name, std::string &var);

        /** \brief Check if a variable exist in the _environment.               */
        bool Exist(std::string _name);
        /** \brief Get the set of variables in the _environment.                */
        std::set<std::string> Who();
        /** \brief Get the set of variables in a printable string format.       */
        std::string WhoToString();

        /** \brief Get a variable from the _environment.                        */
        bool GetData(std::string _name, FrameworkCore::CGenericType &var);
        /** \brief Get numeric variable, efficient implementation.              */
        FrameworkCore::CGenericType::numericType GetNumber(std::string _name);
        /** \brief Remove a variable from the _environment.                     */
        void Remove(std::string _name);

        /** \brief Merge with another _environment.                             */
        void Merge(CEnvironment &_environment);
        /** \brief Clear the _environment.                                      */
        void Clear();

        /** \brief Dump _environment content to text file.                      */
        void DumpToFile(std::string file_name);
        /** \brief Undump _environment content from text file.                  */
        void UndumpFromFile(std::string file_name);

        /** \brief Start the _environment logging.                              */
        void StartLogging(LOGGINGTYPE type = RESET_LOGGING_DATA);
        /** \brief Stop the _environment logging.                               */
        void StopLogging();
        /** \brief Dump the logging data to file.                               */
        void DumpLogDataToTextFile(std::string filename);
        /** \brief Clear logged data.                                           */
        void ClearLogData();
        /** \brief Start the _environment logging.                              */
        void StartLoggingToTextFile(std::string filename, LOGGINGTYPE type = RESET_LOGGING_DATA);
        /** \brief Stop the _environment logging.                               */
        void StopLoggingToTextFile();
        /** \brief Convert  a logging data file into a telemetry file.          */
        void Log2Telemetry(std::string filename, bool showProgress = true);

        /** \brief Lock _environment.                                           */
        __inline void envLock() {_synch.InstanceLock("env");}
        /** \brief Unlock _environment.                                         */
        __inline void envUnlock() {_synch.InstanceUnlock("env");}
        };
    
    }