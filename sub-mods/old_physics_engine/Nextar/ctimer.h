/*FILE MARK
 DOC ORIGINAL NAME:	timer_t.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/9/2007		AT:		2:06:07 PM
END
******************************************************************************************************
*/
#ifndef __XTIMER_H__
#define __XTIMER_H__

#include <xtypes.h>

namespace nextar
{

class timer_t
{

public:

	X_Api static timer_t skAsynchTimer;

private:
	
	u32		muStartTime;
	u32		muLastVirtualTime;
	u32		muCurrTime;
	u32		muRealTime;
	f32		mfSpeed;
	f32		mfElapsedSecs;
	bool	mbStopped;

public:

	/* timer initialization */
	X_Api timer_t();

	// get elapsed time in seconds
	_n_inline f32 getElapsedTime() { return mfElapsedSecs; }
	/* get the time when a tick was registered */
	_n_inline u32 getFrameTime() const { return muCurrTime; }
	/* get the time when this clock was reset last time */
	_n_inline u32 getLastResetTime() const {	return muStartTime;	}
	/* reset the clock */
	_n_inline void reset() {	muRealTime = muStartTime = getCurrentTime(); }
	/* is stopped */
	_n_inline bool isStopped() { return mbStopped; }
	/* set speed */
	_n_inline void setSpeed( f32 speed ) { mfSpeed = speed; }
	/* get speed */
	_n_inline f32 getSpeed() const { return mfSpeed; }
	/* get the actual timing */
	X_Api u32	getCurrentTime() const;
	/* click and tick */
	X_Api void	tick();		
	/* stop the clock */
	X_Api void	stop();
	/* start the clock */
	X_Api void	start();				
	/* sets a virtual time */
	X_Api void	setTime( u32 );	
};

}

#endif //__XTIMER_H__