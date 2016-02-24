/*
 * RenderTarget.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#include <RenderTarget.h>

namespace nextar {

RenderTarget::RenderTarget(RenderTargetType ty) :
		targetType(ty), viewport(nullptr) {
}

RenderTarget::~RenderTarget() {
	for (Viewport::Iterator it(viewport); it;) {
		Viewport* o = (*it);
		++it;
		NEX_DELETE(o);
	}
}

Viewport* RenderTarget::CreateViewport(Camera* cam,
		const String& renderSys,
		float x, float y,
		float width, float height,
		int32 priority) {

	Viewport* vp =
			NEX_NEW(
					Viewport(cam, this, renderSys,
							x, y, width, height, priority,
							Viewport::ACTIVE, StringUtils::NullID, viewport));
	viewport = vp;
	/** todo Sort via priority (or do sorted insert) */
	return vp;
}

Viewport::Iterator RenderTarget::GetViewports() {
	return Viewport::Iterator(viewport);
}

void RenderTarget::Present(RenderContext* rc) {
}

void RenderTarget::Capture(RenderContext* rc, PixelBox& image, FrameBuffer fb) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

void RenderTarget::CopyTo(RenderContext* rc, FrameBuffer srcFb, RenderTarget* dest, FrameBuffer destFb) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

} /* namespace nextar */
