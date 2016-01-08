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
#include <DebugDisplay.h>

namespace nextar {

/************************************************************************/
/* ForwardRenderSystem                                                 */
/************************************************************************/
ForwardRenderSystem::ForwardRenderSystem() {
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, CreateResources, this);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

ForwardRenderSystem::~ForwardRenderSystem() {
}

void ForwardRenderSystem::PrepareMaterials() {
}

void ForwardRenderSystem::Commit(CommitContext& context) {
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
