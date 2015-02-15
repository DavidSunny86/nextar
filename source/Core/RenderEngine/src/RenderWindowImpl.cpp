/*
 * RenderWindowImpl.cpp
 *
 *  Created on: 24-Mar-2014
 *      Author: obhi
 */

#include <NexRenderEngine.h>
#include <RenderWindowImpl.h>

namespace nextar {

RenderWindowImpl::RenderWindowImpl() :
		RenderTarget(RenderTargetType::BACK_BUFFER) {
}

RenderWindowImpl::~RenderWindowImpl() {
}

Size RenderWindowImpl::GetDimensions() const {
	return dimensions;
}

} /* namespace nextar */
