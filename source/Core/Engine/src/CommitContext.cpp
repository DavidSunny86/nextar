/*
 * CommitContext.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <RenderSystem.h>
#include <CommitContext.h>
#include <RenderTarget.h>
#include <MultiRenderTarget.h>
#include <RenderTexture.h>
#include <RenderBuffer.h>

namespace nextar {

CommitContext::CommitContext() {
	_Reset();
}

CommitContext::~CommitContext() {
	// TODO Auto-generated destructor stub
}

void CommitContext::_Reset() {

	frameNumber = 0;
	targetDimension.combined = 0;
	visibiles = nullptr;
	lightSystem = nullptr;
	viewport = nullptr;
	primitive = nullptr;
	material = nullptr;
	viewMatrix = nullptr;
	viewProjectionMatrix = nullptr;
	projectionMatrix = nullptr;
	invProjectionMatrix = nullptr;
	invViewProjectionMatrix = nullptr;
	light = nullptr;
	albedoMap = nullptr;
	specularMap = nullptr;
	normalMap = nullptr;
	depthMap = nullptr;
	frameTime = 0.0f;
	frameTimer = nullptr;
	lastRenderTarget = nullptr;
	viewRenderTarget = nullptr;

	for (auto &b : paramBuffers)
		b = nullptr;

	/*sunLightIntensity.x = 0.4f;
	sunLightIntensity.y = 0.4f;
	sunLightIntensity.z = 0.4f;
	sunLightIntensity.w = 1.0f;
	sunLightPosition.x = 250;
	sunLightPosition.y = 1000;
	sunLightPosition.z = 250;
	sunLightPosition.w = 0;
	sunLightColor = Color::White;*/
}

nextar::RenderTargetName CommitContext::ParseTargetName(
		const String& target) {
	RenderTargetName lastTarget = RenderTargetName::RT_NONE;
	if (target == "last")
		lastTarget = RenderTargetName::LAST_RT_MT;
	else if (target.compare(0, 5, "last.") == 0) {
		if (target.compare(5, 5, "depth") == 0)
			lastTarget = RenderTargetName::LAST_RT_DEPTH;
		else if (target.compare(5, 6, "color-") == 0) {
			lastTarget = (RenderTargetName) ((RenderTargetName::LAST_RT_COLOR_0
				+ Convert::ToULong(&target.c_str()[11])));
		}
	} else if (target == "viewport")
		lastTarget = RenderTargetName::VIEW_RT_MT;
	else if (target.compare(0, 9, "viewport.") == 0) {
		if (target.compare(9, 5, "depth"))
			lastTarget = RenderTargetName::VIEW_RT_DEPTH;
		else if (target.compare(9, 6, "color-") == 0) {
			lastTarget = (RenderTargetName) ((RenderTargetName::VIEW_RT_COLOR_0
				+ Convert::ToULong(&target.c_str()[15])));
		}
	}

	return lastTarget;
}

nextar::RenderTarget* CommitContext::GetTargetByName(
		RenderTargetName toLastSubTarget) {
	NEX_ASSERT(toLastSubTarget >= LAST_RT_MT && toLastSubTarget < RT_NONE);
	switch (toLastSubTarget) {
	case LAST_RT_MT:
		return lastRenderTarget;
	case LAST_RT_DEPTH:
		NEX_ASSERT(lastRenderTarget);
		if (lastRenderTarget->GetRenderTargetType()
				== RenderTargetType::MULTI_RENDER_TARGET)
			return static_cast<MultiRenderTarget*>(lastRenderTarget)->GetDepthAttachmentPtr();
		else
			//@todo  this is possibly a depth texture, but we need to verify
			return lastRenderTarget;
	case LAST_RT_COLOR_0:
	case LAST_RT_COLOR_1:
	case LAST_RT_COLOR_3:
	case LAST_RT_COLOR_4:
	case LAST_RT_COLOR_5:
		NEX_ASSERT(lastRenderTarget);
		if (lastRenderTarget->GetRenderTargetType()
				== RenderTargetType::MULTI_RENDER_TARGET)
			return static_cast<MultiRenderTarget*>(lastRenderTarget)->GetAttachmentPtr(
					toLastSubTarget - LAST_RT_COLOR_0);
		else
			//@todo  this is possibly a color texture, but we need to verify
			return lastRenderTarget;
	case VIEW_RT_MT:
		return viewRenderTarget;
	case VIEW_RT_DEPTH:
		NEX_ASSERT(viewRenderTarget);
		if (viewRenderTarget->GetRenderTargetType()
				== RenderTargetType::MULTI_RENDER_TARGET)
			return static_cast<MultiRenderTarget*>(viewRenderTarget)->GetDepthAttachmentPtr();
		else
			//@todo  this is possibly a depth texture, but we need to verify
			return viewRenderTarget;
	case VIEW_RT_COLOR_0:
	case VIEW_RT_COLOR_1:
	case VIEW_RT_COLOR_3:
	case VIEW_RT_COLOR_4:
	case VIEW_RT_COLOR_5:
		NEX_ASSERT(viewRenderTarget);
		if (viewRenderTarget->GetRenderTargetType()
				== RenderTargetType::MULTI_RENDER_TARGET)
			return static_cast<MultiRenderTarget*>(viewRenderTarget)->GetAttachmentPtr(
					toLastSubTarget - LAST_RT_COLOR_0);
		else
			//@todo  this is possibly a color texture, but we need to verify
			return viewRenderTarget;
	}
	return nullptr;
}

} /* namespace nextar */
