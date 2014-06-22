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
GBuffer::GBuffer() {
	renderTarget = Assign(NEX_NEW(MultiRenderTarget()));
}

GBuffer::~GBuffer() {
}

void GBuffer::Setup(Size dimensions) {
	if (!renderTarget) {
		MultiRenderTarget::CreateParam params;
		params.dimensions = dimensions;
		params.useDepth = true;
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
		renderTarget->Create(params);

		depth = static_cast<RenderTexture*>(renderTarget->GetDepthAttachment());
		normalGloss = static_cast<RenderTexture*>(renderTarget->GetAttachment(0));
		albedoSpecular =
				static_cast<RenderTexture*>(renderTarget->GetAttachment(1));
	}
}

/************************************************************************/
/* DeferredRenderSystem                                                 */
/************************************************************************/
DeferredRenderSystem::DeferredRenderSystem() {
}

DeferredRenderSystem::~DeferredRenderSystem() {
}

void DeferredRenderSystem::PrepareGeometryBuffer() {
	gbufferRI.rt = gbuffer.renderTarget;
	gbufferRI.clearColor = Color::Black;
	gbufferRI.clearStencil = 0;
	gbufferRI.clearDepth = 1.0f;
	gbufferRI.clearFlags = ClearFlags::CLEAR_ALL;
}

void DeferredRenderSystem::Commit(CommitContext& context) {
	if (gbufferDimension.combined != context.targetDimension.combined) {
		gbuffer.Setup(context.targetDimension);
		gbufferDimension = context.targetDimension;
	}
	/* geometry pass */
	VisibilitySet& visibles = *context.visibiles;
	context.renderContext->BeginRender(&gbufferRI);
	RenderQueueList& layerList = visibles.GetRenderQueues();

	for (auto &layer : layerList) {
		if (Test(layer.flags & RenderQueueFlags::DEFERRED)) {
			for (auto &prim : layer.visibles) {
				MaterialAsset* material = prim.second->GetMaterial();
				ShaderAsset* shader = material->GetShader();
				if (context.shader != shader) {
					context.shader = shader;
					// deferred pass at 0
					Pass* pass = context.shader->GetPass(0);
					context.passNumber = pass->GetID();
					context.pass = context.renderContext->GetView(pass);
					context.paramBuffers[ParameterContext::CTX_PASS] = shader->Get
					context.pass->SwitchAndUpdateParams(context);
				}
				context.primitive = prim.second;
				if (context.material != material) {
					context.material = material;
					context.materialNumber++;
					context.paramBuffers[ParameterContext::CTX_MATERIAL] = context.material->GetParameters();
					context.pass->UpdateParams(context, ParameterContext::CTX_MATERIAL, context.materialNumber);
				}

				context.primitive = prim.second;
				context.paramBuffers[ParameterContext::CTX_OBJECT] = prim.second->GetParameters();
				context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, prim.first);
				context.renderContext->Draw(prim.second->GetStreamData(), context);
			}
		}
	}
	context.renderContext->EndRender();
	/* light pass */
	LightSystem* lightSys = context.lightSystem;
	LightList& ls = lightSys->GetLights();
	/* etc */
	PixelBox image;
	gbuffer.normalGloss->Capture(context.renderContext, image, FrameBuffer::FRONT);
	Image imageObj(std::move(image));
	/* Display Image */
	if (context.debugDisplay) {
		Box2D box(0, 0, 0.25f, 0.25f);
		context.debugDisplay->Register(box, Color::Red, gbuffer.normalGloss);
	}
}

} /* namespace nextar */
