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
}

GBuffer::~GBuffer() {
}

void GBuffer::Destroy() {

	depth.Clear();
	albedoSpecular.Clear();
	normalMap.Clear();
	renderTarget.Clear();
}

void GBuffer::Setup(Size dimensions) {
	if (!renderTarget) {
		renderTarget = Assign(NEX_NEW(MultiRenderTarget()));
		MultiRenderTarget::CreateParam params;
		params.dimensions = dimensions;
		params.useDepth = true;
		params.numColorTargets = 2;
		/* normal & gloss */
		params.targets[0].useAsTexture = true;
		params.targets[0].format = PixelFormat::RGBA8;
		/* albedo & specular */
		params.targets[1].useAsTexture = true;
		params.targets[1].format = PixelFormat::RG16;
		/* depth */
		params.depth.useAsTexture = true;
		params.depth.format = PixelFormat::D24S8;
		renderTarget->Create(params);

		depth = renderTarget->GetDepthAttachment();
		normalMap = renderTarget->GetAttachment(0);
		albedoSpecular = renderTarget->GetAttachment(1);
	}
}

/************************************************************************/
/* DeferredRenderSystem                                                 */
/************************************************************************/
DeferredRenderSystem::DeferredRenderSystem() {
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, CreateMaterials, this);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyBuffers, this);
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

void DeferredRenderSystem::PrepareMaterials() {
	if (!lightMaterial) {
		URL lightMaterialPath(FileSystem::ArchiveEngineData, "Materials/DeferredLights.asset");
		lightMaterial = Asset::AsyncLoad(lightMaterialPath);
	}
}

void DeferredRenderSystem::Commit(CommitContext& context) {
	if (gbufferDimension.combined != context.targetDimension.combined) {
		PrepareMaterials();
		gbuffer.Setup(context.targetDimension);
		PrepareGeometryBuffer();
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
					Pass& pass = context.shader->GetPass(0);
					context.passNumber = pass.GetID();
					context.pass = static_cast<Pass::View*>(context.renderContext->GetView(&pass));
					context.paramBuffers[(uint32)ParameterContext::CTX_PASS] = &shader->GetParameters();
					context.pass->SwitchAndUpdateParams(context);
				}
				context.primitive = prim.second;
				if (context.material != material) {
					context.material = material;
					context.materialNumber++;
					context.paramBuffers[(uint32)ParameterContext::CTX_MATERIAL] = context.material->GetParameters();
					context.pass->UpdateParams(context, ParameterContext::CTX_MATERIAL, context.materialNumber);
				}

				context.primitive = prim.second;
				context.paramBuffers[(uint32)ParameterContext::CTX_OBJECT] = prim.second->GetParameters();
				context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, prim.first);
				context.renderContext->Draw(prim.second->GetStreamData(), context);
			}
		}
	}
	context.renderContext->EndRender();
	context.primitive = nullptr;
	/* light pass */
	LightSystem* lightSys = context.lightSystem;
	LightList& ls = lightSys->GetLights();
	/* etc */
	PixelBox image;
	gbuffer.normalMap->Capture(context.renderContext, image, FrameBuffer::FRONT);
	Image imageObj(std::move(image));
	/* Display Image */
	if (context.debugDisplay) {
		Box2D box(0, 0, 0.25f, 0.25f);
		context.debugDisplay->Register(box, Color::Red, gbuffer.normalMap);
		Box2D box2(0.25, 0, 0.5f, 0.25f);
		context.debugDisplay->Register(box2, Color::Red, gbuffer.albedoSpecular);
	}

	for(auto& lightPair : ls) {
		auto light = lightPair.second;
		switch(light->GetLightType()) {
		case Light::Type::OMNI:
			RenderLight(light, (uint32)Light::PassIndex::OMNI, lightPair.first, context);
			break;
		}
	}
}

void DeferredRenderSystem::RenderLight(Light* light, uint32 passIdx, uint32 updateId, CommitContext& context) {
	VisiblePrimitive* lightVol = light->GetLightVolume();
	MaterialAsset* material = lightVol->GetMaterial();
	if (material)
		passIdx = 0;
	else if (!(material = lightMaterial))
		return;
	ShaderAsset* shader = material->GetShader();
	context.light = light;
	if (context.shader != shader) {
		context.shader = shader;
		// deferred pass at 0
		Pass& pass = context.shader->GetPass(passIdx);
		context.passNumber = pass.GetID();
		context.pass = static_cast<Pass::View*>(context.renderContext->GetView(&pass));
		context.paramBuffers[(uint32)ParameterContext::CTX_PASS] = &shader->GetParameters();
		context.pass->SwitchAndUpdateParams(context);
	}
	context.primitive = lightVol;
	if (context.material != material) {
		context.material = material;
		context.materialNumber++;
		context.paramBuffers[(uint32)ParameterContext::CTX_MATERIAL] = context.material->GetParameters();
		context.pass->UpdateParams(context, ParameterContext::CTX_MATERIAL, context.materialNumber);
	}

	context.primitive = lightVol;
	context.paramBuffers[(uint32)ParameterContext::CTX_OBJECT] = lightVol->GetParameters();
	context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, updateId);
	context.renderContext->Draw(lightVol->GetStreamData(), context);
	context.light = nullptr;
	context.primitive = nullptr;
}

void DeferredRenderSystem::DestroyBuffers(void* renderSystem) {
	DeferredRenderSystem* pRenderSys = reinterpret_cast<DeferredRenderSystem*>(renderSystem);
	if (pRenderSys)
		pRenderSys->DestroyBuffer();
}


void DeferredRenderSystem::CreateMaterials(void* renderSystem) {
	DeferredRenderSystem* pRenderSys = reinterpret_cast<DeferredRenderSystem*>(renderSystem);
	if (pRenderSys)
		pRenderSys->PrepareMaterials();
}

} /* namespace nextar */
