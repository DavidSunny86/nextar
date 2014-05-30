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
			RenderTarget(RenderTargetType::MULTI_RENDER_TARGET),
			ContextObject(ContextObject::TYPE_MULTI_RENDER_TARGET),
			flags(0), depth(nullptr), dimensions(0,0), numColorTargets(0) {
	}

	MultiRenderTarget::~MultiRenderTarget() {
		for(uint32 i = 0; i < numColorTargets; ++i)
			NEX_DELETE(color[i]);
		numColorTargets = 0;
		if (depth)
			NEX_DELETE(depth);
		depth = 0;
	}

	Size MultiRenderTarget::GetDimensions() const {
		return dimensions;
	}

	void MultiRenderTarget::Create(const CreateParam& params) {
		/* create the individual textures */
		TargetParamArray& tpa = params.targets;
		dimensions = params.dimensions;
		numColorTargets = params.numColorTargets;
		for(uint32 i = numColorTargets; i < params.numColorTargets; ++i) {
			if ()
				color[i] = tpa[i].useTarget ? tpa[i].useTarget :
				CreateColorTexture(tpa[i]);

		}

		bool validDepth = (params.useDepth);
		if (validDepth != (IsDepthBufferValid())) {
			if(validDepth)
				depth = params.depth.useTarget ? params.depth.useTarget : RenderManager::Instance().CreateRenderTarget();
			else {
				NEX_DELETE(depth);
				depth = 0;
			}
		}

		ContextObject::RequestUpdate(updatePtr);
	}

	PixelFormat MultiRenderTarget::GetPixelFormat() const {
		NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
		return PixelFormat::UNKNOWN;
	}

	void MultiRenderTarget::Capture(RenderContext* rc, PixelBox& image, FrameBuffer fb) {
		NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
	}

	RenderTarget* MultiRenderTarget::GetAttachment(uint16 i) {
		return color[i];
	}

	RenderTarget* MultiRenderTarget::GetDepthAttachment() {
		return depth;
	}

	RenderTarget* MultiRenderTarget::CreateColorTexture(const TargetParam& tp) {
		if(tp.useAsTexture) {
			RenderTexture* rt = NEX_NEW( RenderTexture() );
			rt->Create(TextureBase::TEXTURE_2D, tp.format, dimensions.width, dimensions.height, 1);
			return rt;
		} else {
			RenderBuffer* rt = NEX_NEW( RenderBuffer() );
			rt->Create(tp.format, dimensions.width, dimensions.height);
			return rt;
		}
	}

} /* namespace nextar */
