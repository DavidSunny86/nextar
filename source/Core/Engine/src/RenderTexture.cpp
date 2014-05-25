/*
 * RenderTexture.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <RenderTexture.h>

namespace nextar {

	RenderTexture::RenderTexture() : RenderTarget(RenderTargetType::RENDER_TEXTURE) {
		// TODO Auto-generated constructor stub

	}

	RenderTexture::~RenderTexture() {
		// TODO Auto-generated destructor stub
	}

	PixelFormat RenderTexture::GetPixelFormat() const {
		return format;
	}

	Size RenderTexture::GetDimensions() const {
		return Size(width, height);
	}

	void RenderTexture::NotifyUpdated(ContextObject::UpdateParamPtr ptr) {
		Params* p = reinterpret_cast<Params*>(ptr);
		width = p->dimensions.width;
		height = p->dimensions.height;
		ContextObject::RequestUpdate(ptr);
	}

	void RenderTexture::Update(RenderContext* rc, ContextObject::UpdateParamPtr ptr) {
		Params* p = reinterpret_cast<Params*>(ptr);
		Reset(rc, p->dimensions, p->format);
	}

} /* namespace nextar */
