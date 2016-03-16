/*
 * MultiRenderTarget.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <RenderSystem.h>
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

void MultiRenderTarget::Create(const CreateParams& params) {
	/* create the individual textures */
	const TargetParamArray& tpa = params.targets;
	dimensions = params.dimensions;
	numColorTargets = params.numColorTargets;
	for (uint32 i = 0; i < numColorTargets; ++i) {
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

void MultiRenderTarget::ResizeImpl(Size d) {
	dimensions = d;
	for (uint32 i = 0; i < numColorTargets; ++i) {
		if(color[i])
			color[i]->Resize(d);
	}
	if (depth)
		depth->Resize(d);
	ContextObject::RequestUpdate(MSG_RT_RESIZE,
				reinterpret_cast<ContextObject::ContextParamPtr>(this));

}

void MultiRenderTarget::Capture(RenderContext* rc, 
								PixelBox& image, FrameBuffer) {
	NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
}

} /* namespace nextar */
