/*
 * CanvasGLX.h
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#ifndef CANVASGLX_H_
#define CANVASGLX_H_

#include <CanvasGL.h>

namespace RenderOpenGL {

	class CanvasGLX : public CanvasGL {

	public:
		CanvasGLX(const nextar::String& canvName, bool isWind);

		inline void SetDrawable(GLXDrawable drawable) {
			this->drawable = drawable;
		}

		inline GLXDrawable GetDrawable() {
		        return drawable;
		}

	protected:

		GLXDrawable drawable;
	};

}



#endif /* CANVASGLX_H_ */
