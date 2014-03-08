/*
 * MultiRenderTarget.cpp
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "MultiRenderTarget.h"

namespace nextar {

	MultiRenderTarget::MultiRenderTarget() :
			RenderTarget(RenderTargetType::MULTI_RENDER_TARGET),
			flags(0), depth(nullptr), dimensions(0,0), numColorTargets(0) {
	}

	MultiRenderTarget::~MultiRenderTarget() {
		Reset();
	}

	Size MultiRenderTarget::GetDimensions() const {
		return dimensions;
	}

	void MultiRenderTarget::Reset() {
		for(uint32 i = 0; i < numColorTargets; ++i)
			NEX_DELETE color[i];
		numColorTargets = 0;
		if (depth)
			NEX_DELETE depth;
		depth = 0;
	}

	void MultiRenderTarget::NotifyUpdated(ContextObject::UpdateParamPtr updatePtr) {
		/* create the individual textures */
		UpdateParam* updateParams = static_cast<UpdateParam*>(updatePtr);
		if (updateParams->flags & UpdateParam::UPDATE_ALL)
			Reset();

		TargetParamArray& tpa = updateParams->targets;
		dimensions = updateParams->dimensions;

		if(updateParams->numColorTargets != numColorTargets) {
			if (updateParams->numColorTargets < numColorTargets) {
				for(uint32 i = updateParams->numColorTargets; i < numColorTargets; ++i)
					NEX_DELETE color[i];
			}
			else {
				for(uint32 i = numColorTargets; i < updateParams->numColorTargets; ++i)
						color[i] = tpa[i].useTarget ? tpa[i].useTarget : RenderManager::Instance().CreateRenderTarget(tpa[i].useAsTexture);
			}
		}

		bool validDepth = (updateParams->flags & UpdateParam::INCLUDE_DEPTH);
		if (validDepth != (IsDepthBufferValid())) {
			if(validDepth)
				depth = updateParams->depth.useTarget ? updateParams->depth.useTarget : RenderManager::Instance().CreateRenderTarget();
			else {
				NEX_DELETE depth;
				depth = 0;
			}
		}

		ContextObject::NotifyUpdated(updatePtr);
	}

	void MultiRenderTarget::Update(RenderContext* rc, ContextObject::UpdateParamPtr updatePtr) {
		UpdateParam* updateParams = static_cast<UpdateParam*>(updatePtr);
		TargetParamArray& tpa = updateParams->targets;
		for(uint32 i = 0; i < numColorTargets; ++i)
			color[i]->Reset(rc, dimensions, tpa[i].format);
		if (depth)
			depth->Reset(rc, dimensions, updateParams->depth.format);
		UpdateImpl(rc, updateParams);
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
} /* namespace nextar */
