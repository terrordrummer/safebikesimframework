#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "FrameworkCore/common_defines.h"
#include "FrameworkCore/CSynch.h"

namespace FrameworkCore {

    /** \brief Messages manager for tracing application.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class manages the tracing used for debugging at runtime.
     * Inheriting this class allows to use stream operators to stream logging data.\n
     * The stream destination is settable using SetStream() method. \n
     * Available channels are:
     *
     * \li cout, cerr and clog standard outputs
     * \li file output
     * \li no output (logging disabled)
     *
     */
    class framework_core_class CTrace {
    private:
        /** \brief defines the list of the possible output streams      */
        typedef enum {
            STREAM_cout,    /**< \brief cout standard output.           */
            STREAM_cerr,    /**< \brief cerr standard output.           */
            STREAM_clog,    /**< \brief clog standard output.           */
            STREAM_tofile,  /**< \brief File standard output.           */
            STREAM_none     /**< \brief Standard output disactivated.   */
            } StreamList;
   
        StreamList       _streamType;   /**< \brief Stores the assigned stream output.          */
        std::ofstream*   _fileStream;   /**< \brief File stream if data is written to a file.   */
        std::string      _streamName;   /**< \brief Stream name.                                 */
        int              _instCounter;  /**< \brief Counter of the _instances of this class.    */    

		unsigned int	 _streamStaticIndex; /** \brief position into the static array containing the list of all
											            _instances CTrace classes.		        */
        CSynch           _synch;            /** \brief Synchronize stream access.               */
		bool			 _bufferize;        /** \brief Bufferization flag.                      */

    public:

        CTrace();
        explicit CTrace(std::string str_param); /**< \brief Constructor redirecting the output to a file.   */
        virtual ~CTrace();                      /**< \brief overriding destructor to guarantee the 
                                                                fstream to be closed at the end.  */

        CTrace& operator << (std::string& str);     /**< \brief Overload of "<<" operator. */
        CTrace& operator << (const char* str);      /**< \brief Overload of "<<" operator. */
        CTrace& operator << (bool val);             /**< \brief Overload of "<<" operator. */
        CTrace& operator << (short val);            /**< \brief Overload of "<<" operator. */
        CTrace& operator << (unsigned short val);   /**< \brief Overload of "<<" operator. */
        CTrace& operator << (int val);              /**< \brief Overload of "<<" operator. */
        CTrace& operator << (unsigned int val);     /**< \brief Overload of "<<" operator. */
        CTrace& operator << (long val);             /**< \brief Overload of "<<" operator. */
        CTrace& operator << (unsigned long val);    /**< \brief Overload of "<<" operator. */
        CTrace& operator << (double val);           /**< \brief Overload of "<<" operator. */
        CTrace& operator << (long double val);      /**< \brief Overload of "<<" operator. */
        
        /** \brief Overload of "<<" operator.                                                       */
        CTrace& operator << (std::streambuf* sb);
        /** \brief Overload of "<<" operator.                                                       */
        CTrace& operator << (std::ostream& ( *pf )(std::ostream&));
        /** \brief Overload of "<<" operator.                                                       */
        CTrace& operator << (std::ios& ( *pf )(std::ios&));
        /** \brief Overload of "<<" operator.                                                       */
        CTrace& operator << (std::ios_base& ( *pf )(std::ios_base&));

        /** \brief Re-assign the stream output to the selected channel.                             */
        void SetLogStream(std::string str_param);

        /** \brief Re-assign the stream output to the selected channel.
         *  \param str_param stream name: "cout", "cerr", "clog", "none" or valid file name
         */
        void SetLogStream(const char *);

		///** \brief Set bufferization flag.															*/
		//void Bufferize(bool);
        
        };

    }