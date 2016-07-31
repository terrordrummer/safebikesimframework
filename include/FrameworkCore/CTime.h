#pragma once

#include "windows.h"
#include "FrameworkCore/common_defines.h"

namespace FrameworkCore {

    /** \brief This class implements some basic function for time elapse computation.
     *  \author Roberto Sartori, Dynamotion S.r.l.
     *
     * This class implements the basic tic-toc functions: Tic() initializes the timer,
     * Toc() returns the time elapsed from the last Tic().\n
     * TOC() returns the system clock value.
     *
     * \code
     * int main (int argc, char *argv[]) {
     *      CTime::TIC();   // Get the application starting time
     *      ...
     * }
     * \endcode
     *
     */
    class framework_core_class CTime {
    private :
        /** \brief Stores the last 'tic' clock measurement.                             */
        LARGE_INTEGER _ticClock;

        /** \brief Stores the last 'tic' clock measurement (float value).               */
        double _doubleTicClock;

        /** \brief Stores the Number of clock fronts per second.                        */
        LARGE_INTEGER _ticksPerSecond;

        /** \brief Stores statically the last 'tic' clock measurement (float value).    */
        static double _absoluteDoubleTicClock;

        /** \brief Stores the last 'tic' clock measurement when the library is loaded.  */
        static double _absoluteTicClockStart;

        /** \brief Stores statically the number of clock fronts per second.             */
        static LARGE_INTEGER _absoluteTicksPerSecond;


    public:
        /** \brief Time type definition.                                                */
        typedef double time_type;

        /** \brief Default constructor.                                                 */
        CTime();

        /** \brief Clock reset method.                                                  */
        void Tic();

        /** \brief Time elapsed since the last Tic().                                   */
        time_type Toc();

        /** \brief System clock.                                                        */
        static time_type GetTIME();

        /** \brief Static data initializer.                                             */
        static void staticInitializer();
        
        };

    };