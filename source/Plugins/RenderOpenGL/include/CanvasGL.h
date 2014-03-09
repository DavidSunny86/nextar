/*
 * CanvasGL.h
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#ifndef CANVASGL_H_
#define CANVASGL_H_

#include <Canvas.h>

namespace RenderOpenGL {

	class CanvasGL : public nextar::RenderTarget {
	public:
		CanvasGL(const nextar::String& canvName, bool isWind);
	};

}


#endif /* CANVASGL_H_ */
