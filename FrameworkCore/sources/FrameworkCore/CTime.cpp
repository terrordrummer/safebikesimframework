#include "FrameworkCore/CTime.h"

using namespace FrameworkCore;

// -------------------------- static members declaration
double          CTime::_absoluteDoubleTicClock = 0;
LARGE_INTEGER   CTime::_absoluteTicksPerSecond;
double          CTime::_absoluteTicClockStart  = 0;
// -----------------------------------------------------


/*
  ____ _____ _
 / ___|_   _(_)_ __ ___   ___
| |     | | | | '_ ` _ \ / _ \
| |___  | | | | | | | | |  __/
 \____| |_| |_|_| |_| |_|\___|
*/
CTime::CTime() {
    // get the high resolution counter's accuracy
    QueryPerformanceFrequency(&_ticksPerSecond);
    Tic();
    };

/*
 _____ _
|_   _(_) ___
  | | | |/ __|
  | | | | (__
  |_| |_|\___|
*/
void CTime::Tic() {
    QueryPerformanceCounter(&_ticClock);
    _doubleTicClock = double(_ticClock.QuadPart)/double(_ticksPerSecond.QuadPart);
    };

/*
 _____
|_   _|__   ___
  | |/ _ \ / __|
  | | (_) | (__
  |_|\___/ \___|
*/
/**
 *  \return Toc time value in seconds (float)
 */
CTime::time_type CTime::Toc() {
    LARGE_INTEGER toc_clock;
    QueryPerformanceCounter(&toc_clock);

    return (double(toc_clock.QuadPart)/double(_ticksPerSecond.QuadPart) - _doubleTicClock);
    };

/*
  ____      _  _____ ___ __  __ _____
 / ___| ___| ||_   _|_ _|  \/  | ____|
| |  _ / _ \ __|| |  | || |\/| |  _|
| |_| |  __/ |_ | |  | || |  | | |___
 \____|\___|\__||_| |___|_|  |_|_____|
*/
/**
 *  \return System time.
 */
CTime::time_type CTime::GetTIME() {
    LARGE_INTEGER toc_clock;
    QueryPerformanceCounter(&toc_clock);

    return _absoluteDoubleTicClock + (double(toc_clock.QuadPart)/double(_absoluteTicksPerSecond.QuadPart) - _absoluteDoubleTicClock);
    };

/*
     _        _   _      ___       _ _   _       _ _
 ___| |_ __ _| |_(_) ___|_ _|_ __ (_) |_(_) __ _| (_)_______ _ __
/ __| __/ _` | __| |/ __|| || '_ \| | __| |/ _` | | |_  / _ \ '__|
\__ \ || (_| | |_| | (__ | || | | | | |_| | (_| | | |/ /  __/ |
|___/\__\__,_|\__|_|\___|___|_| |_|_|\__|_|\__,_|_|_/___\___|_|
*/
void CTime::staticInitializer() {
    QueryPerformanceFrequency(&_absoluteTicksPerSecond);
    LARGE_INTEGER ticClock;
    QueryPerformanceCounter(&ticClock);
    _absoluteDoubleTicClock =  double(ticClock.QuadPart)/double(_absoluteTicksPerSecond.QuadPart);
    timeBeginPeriod(1);
    _absoluteTicClockStart = ( double(timeGetTime()) ) / 1000.;
    timeEndPeriod(1);
}