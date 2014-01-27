
#include "NexHeaders.h"
#if defined(NEX_LINUX)
#include "platform/linux/LinuxClock.cpp"
#elif defined(NEX_WINDOWS)
#include "platform/win32/Win32Clock.cpp"
#else
#error Clock not yet implemented
#endif
#include "Clock.h"

using namespace nextar;

namespace nextar {

    /*
    ./ ctor
     */
    Clock::Clock() : ClockImpl(false) {
        ResetClock();
    }

    /*
    ./ Dtor
     */
    Clock::~Clock() {
    }

    /*
    ./ Tick
     */
    uint32 Clock::Tick() {
        NEX_ASSERT(!stopped);
        uint32 curTime = GetMilliSecs();
        elapsedMilliSecs = curTime - lastTick;
        lastTick = curTime;
        return elapsedMilliSecs;
    }

    /*
    ./ Start
     */
    void Clock::StartClock() {
        if (stopped) {
            ResetClock();
        }
    }

    /*
    ./ Stop
     */
    void Clock::StopClock() {
        stopped = true;
    }

    void Clock::ResetClock() {
        Reset();
        lastTick = GetMilliSecs();
        elapsedMilliSecs = 0;
        stopped = false;
    }
}

