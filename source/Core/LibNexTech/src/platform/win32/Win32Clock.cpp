//@	Obi De
//@ CREATED:	1/9/2007		AT:		2:07:58 PM
//@ Jan 12 08
#include "NexHeaders.h"

using namespace nextar;

#if defined(NEX_WINDOWS)
#include "platform/win32/win32Clock.h"

ClockImpl::ClockImpl(bool reset) : timer_mask(0) {
    if (reset)
        Reset();
}

ClockImpl::~ClockImpl() {
}

void ClockImpl::Reset() {
    if (!timer_mask) {
        DWORD_PTR proc_mask;
        DWORD_PTR sys_mask;
        GetProcessAffinityMask(GetCurrentProcess(), &proc_mask, &sys_mask);
        timer_mask = 1;
        // Find a proper core this timer can work on
        while ((timer_mask & proc_mask) == 0)
            timer_mask <<= 1;
    }

    HANDLE Thread = GetCurrentThread();
    DWORD_PTR olmask = SetThreadAffinityMask(Thread, timer_mask);
    // query counters
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);
    start_tick = GetTickCount();
    SetThreadAffinityMask(Thread, olmask);
    last_time = 0;
}

uint32 ClockImpl::GetMilliSecs() {
    LARGE_INTEGER cur_time;
    HANDLE Thread = GetCurrentThread();
    DWORD_PTR olmask = SetThreadAffinityMask(Thread, timer_mask);
    // query counters
    QueryPerformanceCounter(&cur_time);
    SetThreadAffinityMask(Thread, olmask);

    LONGLONG elapsed_time = cur_time.QuadPart - start_time.QuadPart;
    uint32 per_elapsed = static_cast<uint32> ((1000 * elapsed_time) / frequency.QuadPart);
    uint32 tick_elapsed = GetTickCount() - start_tick;
    long leap = static_cast<long> (per_elapsed - tick_elapsed);
    if (abs(leap) > 100) {
        LONGLONG adjust = std::min(leap * frequency.QuadPart / 1000, elapsed_time - last_time);
        start_time.QuadPart += adjust;
        elapsed_time -= adjust;
        per_elapsed = static_cast<uint32> ((1000 * elapsed_time) / frequency.QuadPart);
    }
    last_time = elapsed_time;
    return per_elapsed;
}

#endif