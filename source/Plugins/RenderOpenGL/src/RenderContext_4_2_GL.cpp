/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#include "BaseRenderContext.h"
#include "RenderContext_4_2_GL.h"

namespace RenderOpenGL {


RenderContext_4_2_GL::RenderContext_4_2_GL(RenderDriverGL* d) : ParentContext(d) {
}

void RenderContext_4_2_GL::InitializeExtensions() {
	ParentContext::InitializeExtensions();
	FunctionTable::InitializeFunctionPointers();
}

void RenderContext_4_2_GL::AllocateTexture(GLenum target, GLint levels,
										   GLenum format, GLsizei width, 
										   GLsizei height, GLsizei depth) {
	switch (target) {
	case GL_TEXTURE_1D:
		GlTexStorage1D(target, levels, format, width);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_RECTANGLE:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_1D_ARRAY:
		GlTexStorage2D(target, levels, format, width, height);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		GlTexStorage3D(target, levels, format, width, height, depth);
		break;
	}
	GL_CHECK();
}

VersionGL RenderContext_4_2_GL::GetContextVersion() {
	return GLV_4_2;
}

}
