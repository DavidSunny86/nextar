/*
 * DeferredRenderSystem.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#include <RenderEngineHeaders.h>
#include <DeferredRenderSystem.h>
#include <MultiRenderTarget.h>
#include <RenderTexture.h>
#include <DebugDisplay.h>

namespace nextar {

	/************************************************************************/
	/* GBuffer                                                              */
	/************************************************************************/
	GBuffer::~GBuffer() {
	}

	void GBuffer::Create() {
		depth = static_cast<RenderTexture*>(
			RenderManager::Instance().CreateRenderTarget(true));
		albedoSpecular = static_cast<RenderTexture*>(
			RenderManager::Instance().CreateRenderTarget(true));
	}

	/************************************************************************/
	/* DeferredRenderSystem                                                 */
	/************************************************************************/
	DeferredRenderSystem::DeferredRenderSystem() {
		// TODO Auto-generated constructor stub

	}

	DeferredRenderSystem::~DeferredRenderSystem() {
		// TODO Auto-generated destructor stub
	}

	void DeferredRenderSystem::PrepareGeometryBuffer() {
		gbuffer.Create();
		gbufferRI.rt = gbuffer.renderTarget;
		gbufferRI.clearColor = Color::Black;
		gbufferRI.clearStencil = 0;
		gbufferRI.clearDepth = 1.0f;
		gbufferRI.clearFlags = ClearFlags::CLEAR_ALL;
	}

	void DeferredRenderSystem::Commit(CommitContext& context, RenderContextPtr& renderCtx) {
		if (gbufferDimension.combined != context.targetDimension.combined) {
			gbuffer.Setup(context.targetDimension);
			gbufferDimension = context.targetDimension;
		}
		/* geometry pass */
		VisibilitySet& visibles = *context.visibiles;
		renderCtx->BeginRender(&gbufferRI);
		RenderQueueList& layerList = visibles.GetRenderQueues();


		for(auto &layer : layerList) {
			if (Test(layer.flags & RenderQueueFlags::DEFERRED)) {
				for(auto &prim : layer.visibles) {
					MaterialAsset* material = prim.second->GetMaterial();
					ShaderAsset* shader = material->GetShader();
					UpdateFrequency updateParamFlag = UpdateFrequency::PER_OBJECT_INSTANCE;
					context.primitive = prim.second;
					if (context.material != material) {
						context.material = material;
						updateParamFlag |= UpdateFrequency::PER_MATERIAL;
					}
					if (context.shader != shader) {
						renderCtx->SwitchShader(0, context, shader);
						updateParamFlag |= UpdateFrequency::PER_FRAME|UpdateFrequency::PER_PASS|
								UpdateFrequency::PER_VIEW;
					}

					context.pass->UpdateParams(renderCtx, context, updateParamFlag);
					renderCtx->Draw(prim.second->GetStreamData(), context);
				}
			}
		}
		renderCtx->EndRender();
		/* light pass */
		LightSystem* lightSys = context.lightSystem;
		LightList& ls = lightSys->GetLights();
		/* etc */
		PixelBox image;
		gbuffer.normalGloss->Capture(renderCtx, image, FrameBuffer::FRONT);
		Image imageObj(std::move(image));
		/* Display Image */
		if (context.debugDisplay) {
			Box2D box(0, 0, 0.25f, 0.25f);
			context.debugDisplay->Register(box, Color::Red, gbuffer.normalGloss);
		}
	}

	/** GBuffer ************************/
	void GBuffer::Setup(Size dimensions) {
		if (!renderTarget)
			renderTarget = Bind(RenderManager::Instance().CreateMultiRenderTarget());
		/* Clean setup */
		MultiRenderTarget::UpdateParam params;
		params.dimensions = dimensions;
		params.flags = MultiRenderTarget::UpdateParam::INCLUDE_DEPTH;
		params.numColorTargets = 2;
		/* normal & gloss */
		params.targets[0].useAsTexture = true;
		params.targets[0].format = PixelFormat::RGBA8;
		/* albedo & specular */
		params.targets[1].useAsTexture = true;
		params.targets[1].format = PixelFormat::RGBA8;
		/* depth */
		params.depth.useAsTexture = true;
		params.depth.format = PixelFormat::D24S8;
		renderTarget->NotifyUpdated(reinterpret_cast<ContextObject::UpdateParamPtr>(&params));

		depth = static_cast<RenderTexture*>(renderTarget->GetDepthAttachment());
		normalGloss = static_cast<RenderTexture*>(renderTarget->GetAttachment(0));
		albedoSpecular = static_cast<RenderTexture*>(renderTarget->GetAttachment(1));

	}

} /* namespace nextar */
