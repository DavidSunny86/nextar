/*
 * RenderTexture.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <RenderSystem.h>
#include <RenderTexture.h>
#include <RenderConstants.h>
#include <RenderContext.h>

namespace nextar {

RenderTexture::RenderTexture() :
		TextureBase(IS_RENDER_TEXTURE, ContextObject::TYPE_RENDER_TEXTURE), RenderTarget(
				RenderTargetType::TEXTURE) {
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

void RenderTexture::Create(TextureType type, PixelFormat format, uint32 width,
		uint32 height, uint32 depth) {
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
	params.textureFlags = TextureBase::PRE_ALLOCATE_STORAGE | textureFlags;
	params.textureFormat = format;
	params.type = type;
	RequestUpdate(MSG_TEX_CREATE,
			static_cast<ContextObject::ContextParamPtr>(&params));
}

void RenderTexture::Capture(RenderContext* rc, PixelBox& image,
		FrameBuffer frameBuffer) {

	ReadPixelUpdateParams update;
	update.box = &image;
	View* v = static_cast<View*>(rc->GetView(this));
	v->Update(rc, MSG_TEX_READ, &update);
}

void RenderTexture::ResizeImpl(Size newDimensions) {

	width = newDimensions.dx;
	height = newDimensions.dy;
	TextureBase::ResizeParams params;
	params.desc.maxWidth = width;
	params.desc.maxHeight = height;
	params.desc.maxDepth = depth;
	params.desc.maxMipMapCount = 1;
	RequestUpdate(MSG_TEX_RESIZE,
				static_cast<ContextObject::ContextParamPtr>(&params));

}

} /* namespace nextar */
