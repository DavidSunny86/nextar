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

namespace nextar {

	class RenderWindowWin;
	class _NexBaseExport WmInputListener: public Singleton<WmInputListener> {
	public:
		WmInputListener();
		virtual ~WmInputListener();

		/* Received only when in foreground */
		virtual void OnWmInput(RenderWindowWin* window, HRAWINPUT) = 0;

	private:

	};
}

#endif	/* WMINPUTLISTENER_H */

