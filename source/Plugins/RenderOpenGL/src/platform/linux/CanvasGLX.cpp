/*
 * CanvasGLX.cpp
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <CanvasGL.h>
#include <glx/CanvasGLX.h>

namespace RenderOpenGL {

CanvasGLX::CanvasGLX(const String& canvName, bool isWind) :
		CanvasGL(canvName, isWind), drawable(0) {
}
}

