/* 
 * File:   WmInputListener.h
 * Author: obhi
 *
 * Created on June 10, 2011, 8:39 PM
 */

#ifndef WMINPUTLISTENER_H
#define	WMINPUTLISTENER_H

#include "IncWnds.h"
#include "NexSys.h"

namespace nextar {

	class _NexExport WmInputListener: public Singleton<WmInputListener> {
	public:
		WmInputListener();
		virtual ~WmInputListener();

		/* Received only when in foreground */
		virtual void OnWmInput(HRAWINPUT) = 0;

	private:

	};
}

#endif	/* WMINPUTLISTENER_H */

