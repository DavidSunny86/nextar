/* 
 * File:   WmInputListener.h
 * Author: obhi
 *
 * Created on June 10, 2011, 8:39 PM
 */

#ifndef WMINPUTLISTENER_H
#define	WMINPUTLISTENER_H

#include <IncWnds.h>
#include <NexBase.h>
#include <Singleton.h>

namespace nextar {

class Win32Window;
class _NexBaseAPI Win32InputListener: public Singleton<Win32InputListener> {
public:
	Win32InputListener();
	virtual ~Win32InputListener();

	/* Received only when in foreground */
	virtual void ConsumeEvent(Win32Window* window, HRAWINPUT) = 0;
};
}

#endif	/* WMINPUTLISTENER_H */

