/*
 * RenderManagerGL.cpp
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <RenderManagerGL.h>
#include <RenderDriverGL.h>

namespace RenderOpenGL {

	RenderManagerGL::RenderManagerGL() {
		// TODO Auto-generated constructor stub

	}

	RenderManagerGL::~RenderManagerGL() {
		// TODO Auto-generated destructor stub
	}

	String RenderManagerGL::GetInfo() {
		// more info is required but pff
		return "OpenGL Render Manager";
	}

	RenderManager::ShaderProgramLanguage RenderManagerGL::GetProgramLanguage() {
		return RenderManager::SPP_GLSL;
	}

	RenderDriverPtr RenderManagerGL::CreateDriverImpl(DriverCreationParams& params) {
		return Assign(RenderDriverGL::CreateDriverGL(params.gpuIndex));
	}

	void RenderManagerGL::ConfigureImpl(const nextar::NameValueMap& nameValues) {
	}

	void RenderManagerGL::CloseImpl() {
		// discard pending jobs?
	}
} /* namespace RenderOpenGL */
