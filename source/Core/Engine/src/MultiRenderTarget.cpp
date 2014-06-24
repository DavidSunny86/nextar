/*
 * MultiRenderTarget.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <MultiRenderTarget.h>
#include <RenderContext.h>
#include <RenderManager.h>
#include <RenderTexture.h>
#include <RenderBuffer.h>

namespace nextar {

MultiRenderTarget::MultiRenderTarget() :
		RenderTarget(RenderTargetType::MULTI_RENDER_TARGET)
		,ContextObject(ContextObject::TYPE_MULTI_RENDER_TARGET, 0)
		,flags(0)
		,dimensions(0, 0)
		,numColorTargets(0) {
}

MultiRenderTarget::~MultiRenderTarget() {
}

Size MultiRenderTarget::GetDimensions() const {
	return dimensions;
}

void MultiRenderTarget::Create(const CreateParam& params) {
	/* create the individual textures */
	const TargetParamArray& tpa = params.targets;
	dimensions = params.dimensions;
	numColorTargets = params.numColorTargets;
	for (uint32 i = numColorTargets; i < params.numColorTargets; ++i) {
		color[i] = tpa[i].useTarget ? tpa[i].useTarget : CreateTexture(tpa[i]);

	}

	bool validDepth = (params.useDepth);
	if (validDepth) {
		depth = params.depth.useTarget ?
				params.depth.useTarget : CreateTexture(params.depth);
	}

	ContextObject::RequestUpdate(MSG_RT_CREATE,
			reinterpret_cast<ContextObject::ContextParamPtr>(this));
}

PixelFormat MultiRenderTarget::GetPixelFormat() const {
	NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
	return PixelFormat::UNKNOWN;
}

RenderTargetPtr MultiRenderTarget::GetAttachment(uint16 i) const {
	return color[i];
}

RenderTargetPtr MultiRenderTarget::GetDepthAttachment() const {
	return depth;
}

RenderTargetPtr MultiRenderTarget::CreateTexture(const TargetParam& tp) {
	if (tp.useAsTexture) {
		RenderTexture* rt = NEX_NEW(RenderTexture());
		rt->Create(TextureBase::TEXTURE_2D, tp.format, dimensions.width,
				dimensions.height, 1);
		return Assign(rt);
	} else {
		RenderBuffer* rt = NEX_NEW(RenderBuffer());
		rt->Create(tp.format, dimensions.width, dimensions.height);
		return Assign(rt);
	}
}

void MultiRenderTarget::Capture(RenderContext* rc, 
								PixelBox& image, FrameBuffer) {
	NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
}

} /* namespace nextar */
