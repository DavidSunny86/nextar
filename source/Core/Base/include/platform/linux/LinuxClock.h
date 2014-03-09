#ifndef NEXTAR_CLOCKIMPL_H
#define NEXTAR_CLOCKIMPL_H

#include "NexBase.h"
#include "time.h"

namespace nextar {

    /**
     * \class	ClockImpl
     *
     * \brief	Clock implementation. This class is not instantiated.
     *
     * \author	Abhishek Dey
     * \date	6/24/2010
     *
     **/
    class _NexBaseExport ClockImpl :
    public NonCopyableObject {
        //! performance timer start timing.
        struct timeval start;

    protected:
        ClockImpl(bool reset = true);
        ~ClockImpl();

        /**
         * \fn	uint32 GetMilliSecs::GetMilliSecs()
         *
         * \brief	Gets the milli seconds since last reset.
         *
         * \return	The milli seconds.
         **/
        uint32 GetMilliSecs();

        /**
         * \fn	void Reset::Reset()
         *
         * \brief	Resets the clock.
         **/
        void Reset();
    };
}

#endif //NEXTAR_CLOCKIMPL_H

