/* 
 * File:   FrameListener.h
 * Author: obhi
 *
 * Created on June 16, 2011, 8:18 AM
 */

#ifndef FRAMELISTENER_H
#define	FRAMELISTENER_H

#include <NexBase.h>

namespace nextar {

class FrameTimer;
class FrameListener {
public:
	/* Called to mark the start of a frame, return false to skip frame */
	virtual void Execute(const FrameTimer& frameTimer) = 0;


protected:
	~FrameListener() {
	}
};

}
#endif	/* FRAMELISTENER_H */

