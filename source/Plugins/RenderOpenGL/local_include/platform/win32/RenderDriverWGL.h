#pragma once

#include <RenderOpenGL.h>
#include <RenderDriverGL.h>

namespace RenderOpenGL {

class RenderDriverWGL: public RenderDriverGL {

	NEX_LOG_HELPER(RenderDriverWGL)
	;

public:
	RenderDriverWGL(int32 gpuIndex);
	virtual ~RenderDriverWGL();

protected:
	virtual RenderContextGL* CreateContextImpl();

};
}
