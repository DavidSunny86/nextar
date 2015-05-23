/* 
 * File:   GraphicsWindowX.h
 * Author: obhi
 *
 * Created on February 13, 2011, 3:04 AM
 */

#ifndef GRAPHICSWINDOWX_H
#define	GRAPHICSWINDOWX_H

#include <LinuxDefs.h>
#include <RenderWindow.h>

namespace nextar {

/** Glx Graphics windows are derived from here
 */
class XWindow: public RenderWindow {
public:

	XWindow(Impl* imp) :
			RenderWindow(imp), display(0), window(0), msgDestroy(0), eventMask(0) {
	}

	inline Atom GetXDestroyMsg() const {
		return msgDestroy;
	}

	Display* GetDisplay() {
		return display;
	}
	;

	Window GetWindow() {
		return window;
	}

	void SetEventMask(long mask) {
		eventMask = mask;
	}

	long GetEventMask() {
		return eventMask;
	}

	static void ProcessEvent(XWindow* wind, XEvent& event);
protected:
	long eventMask;
	Display* display;
	Window window;
	Atom msgDestroy;
};

}

#endif	/* GRAPHICSWINDOWX_H */

