/*
 * RenderSystemImpl.cpp
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#include <RenderSystemImpl.h>

namespace nextar {

RenderSystemImpl::RenderSystemImpl() {

}

RenderSystemImpl::~RenderSystemImpl() {
}

void RenderSystemImpl::RegisterTarget(StringID as,
		RenderTargetPtr target) {
	targets[as] = target;
}

RenderTargetPtr RenderSystemImpl::GetTarget(StringID name) {
	auto it = targets.find(name);
	if (it != targets.end())
		return (*it).second;
	return RenderTargetPtr();
}

} /* namespace nextar */
