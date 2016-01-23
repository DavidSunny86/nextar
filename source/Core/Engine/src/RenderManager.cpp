/*
 * RenderManager.cpp
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <VertexElement.h>
#include <RenderManager.h>
#include <RenderContext.h>
#include <RenderTarget.h>
#include <Viewport.h>
#include <VideoMode.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(RenderManager);RenderManager ::RenderManager() {
	VertexSemantic::BuildSemanticMap();
}

RenderManager::~RenderManager() {
	VertexSemantic::ClearSemanticMap();
}

void RenderManager::DestroyResources(void* renderSystem) {
	RenderManager* pRenderSys = reinterpret_cast<RenderManager*>(renderSystem);
	if (pRenderSys)
		pRenderSys->DestroyResources();
}


void RenderManager::CreateResources(void* renderSystem) {
	RenderManager* pRenderSys = reinterpret_cast<RenderManager*>(renderSystem);
	if (pRenderSys)
		pRenderSys->CreateResources();
}

void RenderManager::DestroyResources() {
	defaultTexture.Clear();
}

void RenderManager::CreateResources() {
	URL defaultTexturePath(FileSystem::ArchiveEngineData_Name, "Textures/DefaultTexture.jpg");
	TextureAsset::ID id(StringUtils::DefaultID);
	defaultTexture = TextureAsset::Traits::Instance(id, defaultTexturePath);
	defaultTexture->RequestLoad();
	//@urgent load default texture
	//defaultTexture = Asset::AssetLoad(defaultTexturePath);
}

void RenderManager::Configure(const Config& config) {
	const NameValueMap& nv = config.GetSectionMap("RenderManager");
	ConfigureImpl(nv);
}

void RenderManager::AddRenderQueue(const StringID name, uint16 priority,
		RenderQueueFlags flags) {
	if (renderQueues.size() >= 255) {
		Error("Render queue index must be less than 255");
		return;
	}
	RenderQueueDesc desc;
	desc.SetID(name);
	desc.priority = priority;
	desc.flags = flags;
	renderQueues.push_back(desc);
	std::sort(renderQueues.begin(), renderQueues.end());
}

void RenderManager::AddRenderPass(const String& name, const Config& cfg) {
	auto it = renderSystemFactories.find(name);
	if (it != renderSystemFactories.end() && (*it).second)
		AddRenderPass( (*it).second(cfg) );
}

void RenderManager::AddRenderPass(RenderPass* rs) {
	renderSystems.push_back(rs);
}

void RenderManager::RemoveRenderPass(RenderPass* rs) {
	BestErase<RenderPassList>(renderSystems, rs);
}

} /* namespace nextar */
