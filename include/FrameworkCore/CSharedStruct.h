#pragma once

#include <string>
#include <FrameworkCore/common_defines.h>
#include <FrameworkCore/CSynch.h>
#include <windows.h>

namespace FrameworkCore {

    /** \brief Shared Struct class template.
     *
     * This class implements the management of a struct which is shared between processes. The class is a 
     * \b template parametric on the structure to be shared and so it can share any kind of pre-defined
     * structure.
     */
    template <class StructType>
    class CSharedStruct
    {
    public:
        /** \brief Possible results of Acquire method.                  */
        typedef enum {
        CSHAREDSTRUCT_FAILED            = -1,   /**< \brief Failed to create shared struct.     */
        CSHAREDSTRUCT_CREATED           = 0,    /**< \brief Shared struct created.              */
        CSHAREDSTRUCT_ALREADY_EXISTENT  = 1     /**< \brief Shared struct already existend.     */
        } CSHAREDSTRUCT_RESULT;

    private:
       HANDLE       file_mapping;   /**< \brief pipe-file handle                    */
       std::string  name;           /**< \brief name of the struct                  */
       DWORD        struct_size;    /**< \brief size of the struct                  */
       StructType*  data;           /**< \brief struct data pointer                 */
       bool         created;        /**< \brief shared data creation status flag.   */

    public:
        
        /*
          ____ ____  _                        _ ____  _                   _
         / ___/ ___|| |__   __ _ _ __ ___  __| / ___|| |_ _ __ _   _  ___| |_
        | |   \___ \| '_ \ / _` | '__/ _ \/ _` \___ \| __| '__| | | |/ __| __|
        | |___ ___) | | | | (_| | | |  __/ (_| |___) | |_| |  | |_| | (__| |_
         \____|____/|_| |_|\__,_|_|  \___|\__,_|____/ \__|_|   \__,_|\___|\__|
        */
        /** \brief Default constructor.                     */
        CSharedStruct(): file_mapping(NULL),
                         data(NULL),
                         struct_size(0),
                         created(false),
                         name(""){
        }

        /*
            _                   _
           / \   ___ __ _ _   _(_)_ __ ___
          / _ \ / __/ _` | | | | | '__/ _ \
         / ___ \ (_| (_| | |_| | | | |  __/
        /_/   \_\___\__, |\__,_|_|_|  \___|
                       |_|
        */
        /** \brief Acquire the shared struct data.
         *
         *  \param struct_name name of the struct to be shared.
         *  \result acquire result code
         *
         * A shared structure has to be acquired before using it. Return values are:
         * \li CSHAREDSTRUCT_FAILED:  error occurred while trying to acquire the shared memory. structure
         *                            is not accessible.
         * \li CSHAREDSTRUCT_CREATED: shared memory is created and the structure is accessible.
         * \li CSHAREDSTRUCT_ALREADY_EXISTENT: If the structure has already been created then
         *                                     this return code is returned and the shared structure is 
         *                                     ready to be used.
         */
        CSHAREDSTRUCT_RESULT Acquire(std::string struct_name) {
            // create file mapping
           struct_size = 0;
           file_mapping = CreateFileMapping (   INVALID_HANDLE_VALUE, 
                                                NULL,
                                                PAGE_READWRITE, 
                                                0, 
                                                sizeof(StructType), 
                                                struct_name.c_str());

           if (file_mapping == NULL) {
                // error creating file mapping
                return CSHAREDSTRUCT_FAILED;
           }
           // get last error, it returns ERROR_ALREADY_EXISTS if file mapping was already created
           DWORD open_state = GetLastError();
           // now create a pointer to the shared memory segment shared processes
           struct_size = sizeof(StructType);
           name = struct_name;
           data = (StructType *) MapViewOfFile( file_mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
           if (data == NULL) {    
                // error creating the pointer to the shared memory
                CloseHandle(file_mapping);
                return CSHAREDSTRUCT_FAILED;
           }
           // operation success, returns the correct operation result
           created = true;
           if (open_state == ERROR_ALREADY_EXISTS)
               return CSHAREDSTRUCT_ALREADY_EXISTENT;
           else
                return CSHAREDSTRUCT_CREATED;
        }

        /*
         ____      _
        |  _ \ ___| | ___  __ _ ___  ___
        | |_) / _ \ |/ _ \/ _` / __|/ _ \
        |  _ <  __/ |  __/ (_| \__ \  __/
        |_| \_\___|_|\___|\__,_|___/\___|
        */
        /** \brief Release the shared struct data.
         *
         * After having acquired and used the shared structure it has to be released. The last process that 
         * releases the structure will cause the shared memory to be closed.
         */
        void Release() {
           if (data) {
              UnmapViewOfFile(data);
           }
           if (file_mapping) {
              CloseHandle(file_mapping);
           }
           created = false;
        }
       
        /*
         _               _
        | |    ___   ___| | __
        | |   / _ \ / __| |/ /
        | |__| (_) | (__|   <
        |_____\___/ \___|_|\_\
        */
        /** \brief Lock the access to the shared data.
         *
         * To access correctly to the shared structure in a multithread-multiprocess context without
         * falling into synchronization problems the shared sttructure has to be locked before reading/writing it
         * and unlocked at the end of the read/write operation.
         */
        __inline void Lock() {
            if (created)
                CSynch::InterProcessLock(name);
            }
        
        /*
         _   _       _            _
        | | | |_ __ | | ___   ___| | __
        | | | | '_ \| |/ _ \ / __| |/ /
        | |_| | | | | | (_) | (__|   <
         \___/|_| |_|_|\___/ \___|_|\_\
        */        
        /** \brief Unlock the access to the shared data.
         *
         * To access correctly to the shared structure in a multithread-multiprocess context without
         * falling into synchronization problems the shared sttructure has to be locked before reading/writing it
         * and unlocked at the end of the read/write operation.
         */
        __inline void Unlock() {
            if (created)    
                CSynch::InterProcessUnock(name);
            }

        /*
                                    _                 __
          ___  _ __   ___ _ __ __ _| |_ ___  _ __     \ \
         / _ \| '_ \ / _ \ '__/ _` | __/ _ \| '__|_____\ \
        | (_) | |_) |  __/ | | (_| | || (_) | |  |_____/ /
         \___/| .__/ \___|_|  \__,_|\__\___/|_|       /_/
              |_|
        */
        /** \brief Get the pointer to the shared data.
         *
         * Reference operator to access the shared structure data.
         */
        __inline StructType *operator->() {
           return data;
        }
    };

    }