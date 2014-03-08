/*
 * DeferredRenderSystem.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#include "DeferredRenderSystem.h"

namespace nextar {

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
		VisibilityLayerList& layerList = visibles.GetLayerList();

		MaterialAsset* currentMtl = nullptr;
		ShaderAsset* currentShader = nullptr;
		for(auto &layer : layerList) {
			if (layer.flags & LayerFlags::DEFERRED) {
				for(auto &prim : layer.visibles) {
					MaterialAsset* material = prim.second->GetMaterial();
					ShaderAsset* shader = material->GetShader();
					uint16 updateParamFlag = UpdateFrequency::PER_OBJECT_INSTANCE;
					context.primitive = &prim;
					if (currentMtl != material) {
						context.material = currentMtl = material;
						updateParamFlag |= UpdateFrequency::PER_MATERIAL;
					}
					if (currentShader != shader) {
						renderCtx->SwitchShader(0, currentShader = shader);
						updateParamFlag |= UpdateFrequency::PER_FRAME|UpdateFrequency::PER_PASS;
					}

					shader->UpdateParams(renderCtx, context, updateParamFlag);
					renderCtx->Draw(prim.second->GetStreamData());
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
		Image imageObj(image);
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
		renderTarget->NotifyUpdated(&params);

		depth = static_cast<RenderTexture*>(renderTarget->GetDepthAttachment());
		normalGloss = static_cast<RenderTexture*>(renderTarget->GetAttachment(0));
		albedoSpecular = static_cast<RenderTexture*>(renderTarget->GetAttachment(1));

	}

} /* namespace nextar */
