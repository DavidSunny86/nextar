/*
 * RenderManagerGL.cpp
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <RenderManagerGL.h>
#include <RenderDriverGL.h>
#include <PassViewGL.h>

namespace RenderOpenGL {

RenderManagerGL::RenderManagerGL() {
	ConstantParameter::stride = sizeof(UniformGL);
	SamplerParameter::stride = sizeof(SamplerState);
}

RenderManagerGL::~RenderManagerGL() {
	// TODO Auto-generated destructor stub
}

String RenderManagerGL::GetInfo() {
	// more info is required but pff
	return "OpenGL Render Manager";
}

RenderManager::ShaderLanguage RenderManagerGL::GetProgramLanguage() {
	return RenderManager::SPP_GLSL;
}

RenderDriverPtr RenderManagerGL::CreateDriverImpl(
		DriverCreationParams& params) {
	return Assign(RenderDriverGL::CreateDriverGL(params.gpuIndex));
}

void RenderManagerGL::ConfigureImpl(const nextar::NameValueMap& nameValues) {
	BaseRenderManager::ConfigureImpl(nameValues);
}

void RenderManagerGL::CloseImpl() {
	// discard pending jobs?
}
} /* namespace RenderOpenGL */
