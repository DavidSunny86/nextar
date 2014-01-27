/* 
 * File:   GraphicsWindowX.h
 * Author: obhi
 *
 * Created on February 13, 2011, 3:04 AM
 */

#ifndef GRAPHICSWINDOWX_H
#define	GRAPHICSWINDOWX_H

#include "LinuxDefs.h"

namespace nextar {

    /** Glx Graphics windows are derived from here
     */
    class RenderWindowX : public RenderWindow {
    protected:
        Display*    display;
        Window      window;
        Atom		msgDestroy;
    public:

        RenderWindowX() : display(0), window(0), msgDestroy(0) {
        }
        
        inline Atom GetXDestroyMsg() const {
        	return msgDestroy;
        }

        Display* GetDisplay() {
            return display;
        };

        Window GetWindow() {
            return window;
        }

		static void ProcessEvent(RenderWindowX* wind, XEvent& event);
    };
    
}


#endif	/* GRAPHICSWINDOWX_H */


