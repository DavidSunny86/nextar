/*
 * RenderTexture.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <RenderTexture.h>

namespace nextar {

	RenderTexture::RenderTexture() :
		TextureBase(IS_RENDER_TEXTURE, ContextObject::TYPE_RENDER_TEXTURE),
		RenderTarget(RenderTargetType::RENDER_TEXTURE) {
		numMipMaps = 1;
	}

	RenderTexture::~RenderTexture() {
	}

	PixelFormat RenderTexture::GetPixelFormat() const {
		return format;
	}

	Size RenderTexture::GetDimensions() const {
		return Size(width, height);
	}

	void RenderTexture::Create(TextureType type, PixelFormat format,
			uint32 width, uint32 height, uint32 depth) {
		this->format = format;
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->type = type;

		TextureBase::UpdateParams params;
		params.desc.maxWidth = width;
		params.desc.maxHeight = height;
		params.desc.maxDepth = depth;
		params.desc.maxMipMapCount = 1;
		params.textureFlags = TextureBase::PRE_ALLOCATE_STORAGE|textureFlags;
		params.textureFormat = format;
		params.type = type;
		RequestUpdate(MSG_TEX_CREATE,
				static_cast<ContextObject::ContextParamPtr>(&params));
	}

	void RenderTexture::Capture(RenderContext* rc, PixelBox& image,
			FrameBuffer frameBuffer) {
		RenderTargetView* v = static_cast<RenderTargetView*>(rc->GetView(this));
		v->Capture(rc, image, frameBuffer);
	}

	void RenderTexture::Present(RenderContext* rc) {
		RenderTargetView* v = static_cast<RenderTargetView*>(rc->GetView(this));
		v->Present(rc);
	}
} /* namespace nextar */
