/*
 * UniformBufferGL.cpp
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */
#include "RenderOpenGL.h"
#include "UniformBufferGL.h"

namespace RenderOpenGL {

	UniformBufferGL::UniformBufferGL() : mask(0),
			numUnmappedParams(0), frameStamp(0), ubName(0), size(0) {
	}

	UniformBufferGL::~UniformBufferGL() {
	}

} /* namespace RenderOpenGL */
