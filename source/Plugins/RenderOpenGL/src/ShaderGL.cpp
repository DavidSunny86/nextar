/*
 * GLProgram.cpp
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <GpuProgramGL.h>
#include <GpuShaderGL.h>
#include <RenderContextGL.h>

namespace RenderOpenGL {

GpuShaderGL::GpuShaderGL(nextar::AssetManager* m, const String& n) :
		Shader(m, n), iGlProgram(0) {
}

GpuShaderGL::~GpuShaderGL() {
}

} /* namespace RenderOpenGL */
