/* 
 * File:   FrameListener.h
 * Author: obhi
 *
 * Created on June 16, 2011, 8:18 AM
 */

#ifndef FRAMELISTENER_H
#define	FRAMELISTENER_H

#include "NexSys.h"

namespace nextar {

	class FrameListener {
	public:

		virtual ~FrameListener() {}

		/* Called to mark the start of a frame, return false to skip frame */
		virtual void BeginFrame(uint32 frameNumber) {
			(void) frameNumber;
		}

		/* Called to mark the end of a frame, timeElapsed in milliseconds */
		virtual void EndFrame(uint32 timeElapsed) {
			(void) timeElapsed;
		}

	private:

	};
}
#endif	/* FRAMELISTENER_H */

