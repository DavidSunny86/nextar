/*
 * XWinInputListener.h
 *
 *  Created on: 21-Apr-2015
 *      Author: obhi
 */

#ifndef CORE_BASE_INCLUDE_PLATFORM_LINUX_XWININPUTLISTENER_H_
#define CORE_BASE_INCLUDE_PLATFORM_LINUX_XWININPUTLISTENER_H_

#include <LinuxDefs.h>
#include <NexBase.h>
#include <Singleton.h>
#include <XWindow.h>

namespace nextar {

class _NexBaseAPI XWinInputListener: public Singleton<XWinInputListener> {
public:
	XWinInputListener();
	virtual ~XWinInputListener();

	virtual void ConsumeKeyEvent(nextar::XWindow* window, XEvent& event, bool pressEvent) = 0;
	virtual void ConsumeMouseMove(nextar::XWindow* window, XEvent& event) = 0;
	virtual void ConsumeMouseButtonEvent(nextar::XWindow* window, XEvent& event, bool pressEvent) = 0;
};

} /* namespace nextar */

#endif /* CORE_BASE_INCLUDE_PLATFORM_LINUX_XWININPUTLISTENER_H_ */
