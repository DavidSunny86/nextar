/*
 * CommitContext.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef COMMITCONTEXT_H_
#define COMMITCONTEXT_H_

#include <NexBase.h>
#include <ShaderParam.h>
#include <ParameterBuffer.h>
#include <Pass.h>
#include <RenderConstants.h>

namespace nextar {


class CommitContext: public AllocGeneral {
public:

	typedef std::pair<uint32, const ParameterBuffer*> ParamContext;
	
	CommitContext();
	virtual ~CommitContext();

	void _Reset();

	uint32 frameNumber;
	uint32 viewNumber;
	uint32 passNumber;
	uint32 materialNumber;
	float frameTime;
	Size targetDimension;

	Vector4 sunLightIntensity;
	Vector4 sunLightPosition;
	Color sunLightColor;

	Color color;

	typedef array<const ParameterBuffer*, (uint32)ParameterContext::CTX_COUNT>::type ParamBufferList;
	ParamBufferList paramBuffers;
	// rename the
	ParamContext paramContext;
	ParameterGroup* paramGroup;
	void* groupDataPtr;
	const FrameTimer* frameTimer;
		
	TextureBase* albedoMap;
	TextureBase* normalMap;
	TextureBase* specularMap;
	TextureBase* depthMap;

	Light* light; // for deferred
	Pass::View* pass;
	ShaderAsset* shader;
	VisibilitySet* visibiles;
	LightSystem* lightSystem;
	//RenderTarget* renderTarget;
	Viewport* viewport;
	VisiblePrimitive* primitive;
	MaterialAsset* material;
	RenderContext* renderContext;
	RenderTarget* viewRenderTarget;
	RenderTarget* lastRenderTarget;

	// camera and planes
	Camera* camera;

	const Matrix4x4* viewMatrix;
	const Matrix4x4* viewProjectionMatrix;
	const Matrix4x4* projectionMatrix;
	const Matrix4x4* invProjectionMatrix;
	const Matrix4x4* invViewProjectionMatrix;

	static inline RenderTargetName ParseTargetName(const String& target) {
		RenderTargetName lastTarget = RenderTargetName::RT_NONE;
		size_t p;
		if (target == "last")
			lastTarget = RenderTargetName::LAST_RT_MT;
		else if ((p = target.find_first_of("last.")) != String::npos) {
			String type = target.substr(p+5);
			if(type == "depth")
				lastTarget = RenderTargetName::LAST_RT_DEPTH;
			else if ((p=type.find_first_of("color-")) != String::npos) {
				lastTarget = RenderTargetName::LAST_RT_COLOR_0 + Convert::ToULong(type.substr(p+6));
			}
		} else if (target == "viewport")
			lastTarget = RenderTargetName::VIEW_RT_MT;
		else if ((p = target.find_first_of("viewport.")) != String::npos) {
			String type = target.substr(p+9);
			if(type == "depth")
				lastTarget = RenderTargetName::VIEW_RT_DEPTH;
			else if ((p=type.find_first_of("color-")) != String::npos) {
				lastTarget = RenderTargetName::VIEW_RT_COLOR_0 + Convert::ToULong(type.substr(p+6));
			}
		}

		return lastTarget;
	}

	inline RenderTarget* GetTargetByName(RenderTargetName toLastSubTarget) {
		NEX_ASSERT(toLastSubTarget >= LAST_RT_MT && toLastSubTarget < RT_NONE);
		switch(toLastSubTarget) {
		case LAST_RT_MT:
			return lastRenderTarget;
		case LAST_RT_DEPTH:
			NEX_ASSERT(lastRenderTarget);
			if(lastRenderTarget->GetRenderTargetType() == RenderTargetType::MULTI_RENDER_TARGET)
				return static_cast<MultiRenderTarget*>(lastRenderTarget)->GetDepthAttachmentPtr();
			else //@todo  this is possibly a depth texture, but we need to verify
				return lastRenderTarget;
		case LAST_RT_COLOR_0:
		case LAST_RT_COLOR_1:
		case LAST_RT_COLOR_3:
		case LAST_RT_COLOR_4:
		case LAST_RT_COLOR_5:
			NEX_ASSERT(lastRenderTarget);
			if(lastRenderTarget->GetRenderTargetType() == RenderTargetType::MULTI_RENDER_TARGET)
				return static_cast<MultiRenderTarget*>(lastRenderTarget)->GetAttachmentPtr(toLastSubTarget - LAST_RT_COLOR_0);
			else //@todo  this is possibly a color texture, but we need to verify
				return lastRenderTarget;
		case VIEW_RT_MT:
			return viewRenderTarget;
		case VIEW_RT_DEPTH:
			NEX_ASSERT(viewRenderTarget);
			if(viewRenderTarget->GetRenderTargetType() == RenderTargetType::MULTI_RENDER_TARGET)
				return static_cast<MultiRenderTarget*>(viewRenderTarget)->GetDepthAttachmentPtr();
			else //@todo  this is possibly a depth texture, but we need to verify
				return viewRenderTarget;
		case VIEW_RT_COLOR_0:
		case VIEW_RT_COLOR_1:
		case VIEW_RT_COLOR_3:
		case VIEW_RT_COLOR_4:
		case VIEW_RT_COLOR_5:
			NEX_ASSERT(viewRenderTarget);
			if(viewRenderTarget->GetRenderTargetType() == RenderTargetType::MULTI_RENDER_TARGET)
				return static_cast<MultiRenderTarget*>(viewRenderTarget)->GetAttachmentPtr(toLastSubTarget - LAST_RT_COLOR_0);
			else //@todo  this is possibly a color texture, but we need to verify
				return viewRenderTarget;
		}
		return nullptr;
	}
};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
