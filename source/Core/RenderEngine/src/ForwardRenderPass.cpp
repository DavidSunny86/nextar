/*
* ForwardRenderPass.cpp
*
*  Created on: 17-Nov-2013
*      Author: obhi
*/

#include <RenderEngineHeaders.h>
#include <ForwardRenderPass.h>
#include <MultiRenderTarget.h>
#include <RenderTexture.h>

namespace nextar {

/************************************************************************/
/* ForwardRenderPass                                                 */
/************************************************************************/
ForwardRenderPass::ForwardRenderPass() : RenderPass()  {
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, CreateResources, this);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

ForwardRenderPass::~ForwardRenderPass() {
}

RenderPass* ForwardRenderPass::CreateInstance() {
	return NEX_NEW(ForwardRenderPass());
}

void ForwardRenderPass::PrepareMaterials() {
}

void ForwardRenderPass::Commit(CommitContext& context) {

	context.sunLightIntensity = Vector4(1, 1, 1, 1);
	context.sunLightPosition = Vector4(0, 400, 0, 1);
	context.sunLightColor = Color(0.8f, 0.8f, 0.4f, 1);

	BeginRender(context);
	VisibilitySet& visibles = *context.visibiles;
	RenderQueueList& layerList = visibles.GetRenderQueues();
	for (auto &layer : layerList) {
		if (Test(layer.flags & RenderQueueFlags::FORWARD)) {
			for (auto &prim : layer.visibles) {
				RenderPrimitive(context, prim.first, prim.second);
			}
		}
	}
	EndRender(context);
}

void ForwardRenderPass::DestroyResources(void* renderSystem) {
	ForwardRenderPass* pRenderSys = reinterpret_cast<ForwardRenderPass*>(renderSystem);
}


void ForwardRenderPass::CreateResources(void* renderSystem) {
	ForwardRenderPass* pRenderSys = reinterpret_cast<ForwardRenderPass*>(renderSystem);
	if (pRenderSys)
		pRenderSys->PrepareMaterials();
}

} /* namespace nextar */
