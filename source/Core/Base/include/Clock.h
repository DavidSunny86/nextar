//@ Obi De
//@	1/9/2007		AT:		2:06:07 PM
//@ Jan 12 08

#ifndef NEXTAR_TIMER_H
#define NEXTAR_TIMER_H

#include <NexBase.h>
#if defined(NEX_LINUX)
#include <platform/linux/LinuxClock.h>
#elif defined(NEX_WINDOWS)
#include <platform/win32/Win32Clock.h>
#else
#error Clock not yet implemented
#endif

namespace nextar {

class Clock;
//@

/**
 * @class	Clock
 *
 * @brief	Timer class, the global timer is Thread safe.
 *
 * @author	Abhishek Dey
 * @date	11/27/2009
 **/
class _NexBaseAPI Clock: public AllocGeneral, public ClockImpl {
	friend class ApplicationContext;

public:

	Clock();
	~Clock();

	/**
	 * @brief	Stops the timer.
	 *
	 **/
	void StopClock();

	/**
	 * @brief	Starts the timer.
	 *
	 **/
	void StartClock();

	/**
	 * @brief	Resets the timer.
	 *
	 **/
	void ResetClock();

	/**
	 * @brief	Tick once per frame.
	 *
	 **/
	uint32 Tick();

	/**
	 * @brief	Gets the elapsed seconds between last two ticks.
	 *
	 * @return	The elapsed seconds.
	 *
	 **/
	_NexInline float GetElapsedSeconds() const {
		return static_cast<float>(elapsedMilliSecs) / 1000;
	}

	/**
	 * @brief	Gets the elapsed milli seconds between last two ticks.
	 *
	 * @return	The elapsed milli seconds.
	 *
	 **/
	_NexInline uint32 GetElapsedMilliSeconds() const {
		return elapsedMilliSecs;
	}

	/**
	 * @brief	Gets the last tick time.
	 *
	 * @return	The last tick time.
	 *
	 **/
	_NexInline uint32 GetLastTickTime() const {
		return lastTick;
	}

	/** Get milliseconds from the last reset */
	_NexInline uint32 GetMilliSeconds() {
		return GetMilliSecs();
	}

	/**
	 * @brief	Query if this clock is stopped.
	 *
	 * @author	Abhishek Dey
	 * @date	6/24/2010
	 *
	 * @return	true if stopped, false if not.
	 *
	 **/
	_NexInline bool IsStopped() const {
		return stopped;
	}

private:
	bool stopped;
	uint32 lastTick;
	uint32 elapsedMilliSecs;

};

}

#endif //__XTIMER_H__