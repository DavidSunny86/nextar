/*
 * RenderWindowImpl.cpp
 *
 *  Created on: 24-Mar-2014
 *      Author: obhi
 */

#include <RenderSystem.h>
#include <RenderWindowImpl.h>

namespace nextar {

	RenderWindowImpl::RenderWindowImpl() :
		RenderTarget(RenderTargetType::RENDER_WINDOW) {
	}

	RenderWindowImpl::~RenderWindowImpl() {
	}

	Size RenderWindowImpl::GetDimensions() const {
		return dimensions;
	}

} /* namespace nextar */
