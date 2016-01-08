/*FILE MARK
 DOC ORIGINAL NAME:	timer_t.cpp
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/9/2007		AT:		2:07:58 PM
END
******************************************************************************************************
*/


#include <ctimer.h>

#if	defined( X_WIN32 ) || defined( XBOX )
#	ifdef X_WIN32
#		include <windows.h>
#	endif
#	ifdef XBOX
#		include <xtl.h>
#	endif


namespace nextar
{


LARGE_INTEGER gkHPFreq;
bool		  gbHPTimerSupport = false;

/******************************************************************************
./ Timer specs
******************************************************************************/
X_Api timer_t::timer_t() : muStartTime(0), muLastVirtualTime(0), muCurrTime(0), muRealTime(0), mbStopped(false), mfSpeed(1.0f)
{
	gbHPTimerSupport = QueryPerformanceFrequency(&gkHPFreq) ? true : false;
	reset();
}

/******************************************************************************
./ get current time
******************************************************************************/
X_Api u32 timer_t::getCurrentTime() const
{
	if(gbHPTimerSupport)
	{
		LARGE_INTEGER	t;
		QueryPerformanceCounter(&t);
		return u32( (t.QuadPart * 1000) / gkHPFreq.QuadPart );
	}
	return GetTickCount();
}

}

// end win32 or xbox
#else

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

namespace nextar
{

timeval tv;

X_Api timer_t::timer_t() : muStartTime(0), muLastVirtualTime(0), muCurrTime(0), muRealTime(0), mbStopped(false), mfSpeed(1.0f)
{
	reset();
}

X_Api u32 timer_t::getCurrentTime() const
{
	gettimeofday(&tv, 0);
	return u32((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

}

// end linux, posix
#endif

namespace nextar
{

X_Api timer_t timer_t::skAsynchTimer;

/******************************************************************************
./ tick
******************************************************************************/
X_Api void timer_t::tick()
{
	u32 uCurTime = muCurrTime;
	muRealTime = getCurrentTime();
	muCurrTime = muLastVirtualTime + u32((muRealTime - muStartTime)*mfSpeed);
	mfElapsedSecs = (f32)(muCurrTime-uCurTime)/1000.f;
}

/******************************************************************************
./ start
******************************************************************************/
X_Api void timer_t::start()
{
	if( mbStopped )
	{
		setTime(muLastVirtualTime);
		mbStopped = false;
	}
}

/******************************************************************************
./ stop
******************************************************************************/
X_Api void timer_t::stop()
{
	mbStopped = true;
	tick();
	muLastVirtualTime = muCurrTime;
}

/******************************************************************************
./ setTime
******************************************************************************/
X_Api void timer_t::setTime( u32 v )
{
	reset();
	muLastVirtualTime = v;
}


}