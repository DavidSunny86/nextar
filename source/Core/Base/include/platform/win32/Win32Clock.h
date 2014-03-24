#ifndef NEXTAR_CLOCKIMPL_H
#define NEXTAR_CLOCKIMPL_H

#include <IncWnds.h>

namespace nextar {

    /**
     * @class	ClockImpl
     *
     * @brief	Clock implementation. This class is not instantiated.
     *
     * @author	Abhishek Dey
     * @date	6/24/2010
     *
     **/
    class _NexBaseAPI ClockImpl :
    public NonCopyableObject {
        //! performance timer start timing.
        LARGE_INTEGER start_time;
        //! performance timer frequency.
        LARGE_INTEGER frequency;

        //! the starting time after Reset was called.
        DWORD start_tick;
        //! the last time GetMilliSecs was called.
        LONGLONG last_time;

        //! for timer to stick to one core.
        DWORD_PTR timer_mask;
    protected:
        ClockImpl(bool reset = true);
        ~ClockImpl();

        /**
         * @brief	Gets the milli seconds since last reset.
         *
         * @return	The milli seconds.
         **/
        uint32 GetMilliSecs();

        /**
         * @brief	Resets the clock.
         **/
        void Reset();
    };
}

#endif //NEXTAR_CLOCKIMPL_H

