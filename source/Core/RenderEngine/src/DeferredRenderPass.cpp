/*
 * DeferredRenderPass.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#include <RenderEngineHeaders.h>
#include <DeferredRenderPass.h>
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
	albedoMap.Clear();
	normalMap.Clear();
	specularAndGlossMap.Clear();
	renderTarget.Clear();
}

void GBuffer::Setup(Size dimensions) {
	if (!renderTarget) {
		renderTarget = Assign(NEX_NEW(MultiRenderTarget()));
		MultiRenderTarget::CreateParams params;
		params.dimensions = dimensions;
		params.useDepth = true;
		params.numColorTargets = 3;
		/* albedo & alpha */
		params.targets[0].useAsTexture = true;
		params.targets[0].format = PixelFormat::RGBA8;
		/* normal map */
		params.targets[1].useAsTexture = true;
		params.targets[1].format = PixelFormat::RG16;
		/* specular map */
		params.targets[2].useAsTexture = true;
		params.targets[2].format = PixelFormat::RGBA8;
		/* depth */
		params.depth.useAsTexture = true;
		params.depth.format = PixelFormat::D24S8;
		renderTarget->Create(params);

		depth = renderTarget->GetDepthAttachment();
		albedoMap = renderTarget->GetAttachment(0);
		normalMap = renderTarget->GetAttachment(1);
		specularAndGlossMap = renderTarget->GetAttachment(2);
	}
}

/************************************************************************/
/* DeferredRenderPass                                                 */
/************************************************************************/
DeferredRenderPass::DeferredRenderPass() : BaseRenderPass() {
}

DeferredRenderPass::~DeferredRenderPass() {
}


RenderPass* DeferredRenderPass::CreateInstance() {
	return NEX_NEW(DeferredRenderPass());
}


void DeferredRenderPass::PrepareGeometryBuffer() {
	gbufferRI.rt = gbuffer.renderTarget;
	gbufferRI.info.clearColor[0] = Color::Black;
	gbufferRI.info.clearColor[1] = Color(0.5f);
	gbufferRI.info.clearColor[2] = Color::Black;
	gbufferRI.info.clearStencil = 0;
	gbufferRI.info.clearDepth = 1.0f;
}

void DeferredRenderPass::PrepareMaterials() {
	if (!lightMaterial) {
		URL lightMaterialPath(FileSystem::ArchiveEngineData_Name, "Materials/Assets/DeferredLights.asset");
		lightMaterial = Asset::AssetLoad(lightMaterialPath);
	}
}

void DeferredRenderPass::Commit(CommitContext& context) {
	if (gbufferDimension.combined != context.targetDimension.combined) {
		PrepareMaterials();
		gbuffer.Setup(context.targetDimension);
		PrepareGeometryBuffer();
		gbufferDimension = context.targetDimension;
	}
	/* geometry pass */
	VisibilitySet& visibles = *context.visibiles;
	context.renderContext->BeginRender(&gbufferRI, ClearFlags::CLEAR_ALL);
	RenderQueueList& layerList = visibles.GetRenderQueues();
	for (auto &layer : layerList) {
		if (Test(layer.flags & RenderQueueFlags::DEFERRED)) {
			for (auto &prim : layer.visibles) {
				RenderPrimitive(context, prim.first, prim.second);
			}
		}
	}
	context.renderContext->EndRender();
	context.primitive = nullptr;
	/* light pass */
	LightSystem* lightSys = context.lightSystem;
	LightList& ls = lightSys->GetLights();
	/* etc */
	//PixelBox image;
	//gbuffer.normalMap->Capture(context.renderContext, image, FrameBuffer::FRONT);
	//Image imageObj(std::move(image));
	/* Display Image */
	if (DebugDisplay::InstancePtr()) {
		Box2D box(0, 0, 0.25f, 0.25f);
		DebugDisplay::Instance().Register(box, Color::White, Vec4ASet(0, 0, 1, 1), gbuffer.albedoMap, false, -1);
		Box2D box2(0.25f, 0, 0.5f, 0.25f);
		DebugDisplay::Instance().Register(box2, Color::White, Vec4ASet(0, 0, 1, 1), gbuffer.normalMap, false, -1);
		Box2D box3(0.5f, 0.0f, 0.75f, 0.25f);
		DebugDisplay::Instance().Register(box3, Color::White, Vec4ASet(0, 0, 1, 1), gbuffer.specularAndGlossMap, false, -1);
		Box2D box4(0.75f, 0.0f, 1.0f, 0.25f);
		DebugDisplay::Instance().Register(box4, Color::White, Vec4ASet(0, 0, 1, 1), gbuffer.depth, false, -1);
	}

	context.albedoMap = gbuffer.albedoMap;
	context.normalMap = gbuffer.normalMap;
	context.specularMap = gbuffer.specularAndGlossMap;
	context.depthMap = gbuffer.depth;
	
	/*
	context.renderTargetInfo.info.clearFlags = ClearFlags::CLEAR_NONE;
	context.renderContext->BeginRender(&context.renderTargetInfo);
	
	for(auto& lightPair : ls) {
		auto light = lightPair.second;
		switch(light->GetLightType()) {
		case Light::Type::OMNI:
			RenderLight(light, (uint32)Light::PassIndex::OMNI, lightPair.first, context);
			break;
		}
	}
	context.renderContext->EndRender();
	*/
	// @urgent This step might be unnecessary once we have a compositor system in place
	/*if (context.renderTargetInfo.rt->GetRenderTargetType() == RenderTargetType::BACK_BUFFER) {
		// transfer the depth buffer to back-buffer-depth
		context.renderContext->Copy(gbuffer.depth, FrameBuffer::DEPTH,
				context.renderTargetInfo.rt, FrameBuffer::DEPTH);
	}*/
}

void DeferredRenderPass::RenderLight(Light* light, uint32 passIdx, uint32 updateId, CommitContext& context) {
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

void DeferredRenderPass::DestroyResources() {
	DestroyBuffer();
}


void DeferredRenderPass::CreateResources() {
	PrepareMaterials();
}

} /* namespace nextar */
