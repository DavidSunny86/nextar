/*
 * DebugDisplayImpl.cpp
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#include <DebugDisplayImpl.h>

namespace nextar {

DebugDisplayImpl::DebugDisplayImpl() {

}

DebugDisplayImpl::~DebugDisplayImpl() {

}

VisiblePrimitiveList& DebugDisplayImpl::GetPrimitives(CommitContext& context) {
	return alivePrimitives;
}

void DebugDisplayImpl::Register(CommitContext& context, const AABox3& box,
		const Color& color, float expiryTimeInSec) {
}

void DebugDisplayImpl::Register(CommitContext& context, Mat3x4R tform,
		const Color& color, float expiryTimeInSec) {


}

void DebugDisplayImpl::Register(CommitContext& context, PlaneF plane,
		const Color& color, float expiryTimeInSec) {
}

void DebugDisplayImpl::Register(CommitContext& context, const Frustum& frustum,
		const Color& color, float expiryTimeInSec) {
}

void DebugDisplayImpl::Register(CommitContext& context, const Box2D& rect,
		const Color& color, TextureBase* textured, bool border,
		float expiryTimeInSec) {
}

} /* namespace nextar */
