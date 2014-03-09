//@	Obi De
//@ CREATED:	1/9/2007		AT:		2:07:58 PM
//@ Jan 12 08
#include <BaseHeaders.h>

#if defined(NEX_LINUX)
#include <sys/time.h>
#include <platform/linux/LinuxClock.h>

namespace nextar {

    ClockImpl::ClockImpl(bool reset) {
        if (reset)
            Reset();
    }

    ClockImpl::~ClockImpl() {
    }

    uint32 ClockImpl::GetMilliSecs() {
        timeval now;
        gettimeofday(&now, NULL);
        return (now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
    }

    void ClockImpl::Reset() {
        gettimeofday(&start, NULL);
    }
}

#endif
