/*
* ForwardRenderSystem.cpp
*
*  Created on: 17-Nov-2013
*      Author: obhi
*/

#include <RenderEngineHeaders.h>
#include <ForwardRenderSystem.h>
#include <MultiRenderTarget.h>
#include <RenderTexture.h>

namespace nextar {

/************************************************************************/
/* ForwardRenderSystem                                                 */
/************************************************************************/
ForwardRenderSystem::ForwardRenderSystem(const Config& c) : RenderSystem(c)  {
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, CreateResources, this);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

ForwardRenderSystem::~ForwardRenderSystem() {
}

RenderSystem* ForwardRenderSystem::CreateInstance(const Config& c) {
	return NEX_NEW(ForwardRenderSystem(c));
}

void ForwardRenderSystem::PrepareMaterials() {
}

void ForwardRenderSystem::Commit(CommitContext& context) {
	context.sunLightIntensity = Vector4(1, 1, 1, 1);
	context.sunLightPosition = Vector4(0, 400, 0, 1);
	context.sunLightColor = Color(0.8f, 0.8f, 0.4f, 1);

	VisibilitySet& visibles = *context.visibiles;
	RenderQueueList& layerList = visibles.GetRenderQueues();
	for (auto &layer : layerList) {
		if (Test(layer.flags & RenderQueueFlags::FORWARD)) {
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

				context.paramBuffers[(uint32)ParameterContext::CTX_OBJECT] = prim.second->GetParameters();
				context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, prim.first + context.frameNumber);
				context.renderContext->Draw(prim.second->GetStreamData(), context);
			}
		}
	}
}

void ForwardRenderSystem::DestroyResources(void* renderSystem) {
	ForwardRenderSystem* pRenderSys = reinterpret_cast<ForwardRenderSystem*>(renderSystem);
}


void ForwardRenderSystem::CreateResources(void* renderSystem) {
	ForwardRenderSystem* pRenderSys = reinterpret_cast<ForwardRenderSystem*>(renderSystem);
	if (pRenderSys)
		pRenderSys->PrepareMaterials();
}

} /* namespace nextar */
