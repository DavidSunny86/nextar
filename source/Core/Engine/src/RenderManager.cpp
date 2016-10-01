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
#include <Geometry.h>
#include <PluginRegistry.h>
#include <EngineApplicationContext.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(RenderManager);

RenderManager::RenderManager() {
	VertexSemantic::BuildSemanticMap();
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, CreateResources, this);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

RenderManager::~RenderManager() {
	VertexSemantic::ClearSemanticMap();
}

bool RenderManager::QueryService(const Config& config) {
	RenderManager* service = PluginRegistry::Instance().QueryService<RenderManager>();
	if (!service)
		return false;
	service->Configure(config);
	return true;
}

void RenderManager::Destroy() {
	NEX_DELETE(this);
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
	fullScreenQuad.Clear();
	defaultTexture.Clear();
}

void RenderManager::CreateResources() {
	URL defaultTexturePath(FileSystem::ArchiveEngineData_Name, "Textures/Default.png");
	TextureAsset::ID id(StringUtils::DefaultID);
	defaultTexture = TextureAsset::Traits::Instance(id, defaultTexturePath);
	defaultTexture->RequestLoad();
	//@urgent load default texture
	//defaultTexture = Asset::AssetLoad(defaultTexturePath);
	GenerateStreamDataForQuad();
}

void RenderManager::Configure(const Config& config) {
	const NameValueMap& nv = config.GetSectionMap("RenderManager");
	ConfigureImpl(nv);
	ApplicationContext::Instance().DispatchEvent(EngineApplicationContext::EVENT_RENDERMANAGER_CREATED);
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

RenderPass* RenderManager::CreateRenderPass(const String& name) {
	auto it = renderSystemFactories.find(name);
	if (it != renderSystemFactories.end() && (*it).second)
		return (*it).second();
	return nullptr;
}

void RenderManager::AddRenderStreamer(const String& name,
		RenderSystem::Streamer* streamer) {
	renderSystemStreamers[name] = streamer;
}

void RenderManager::RemoveRenderStreamer(const String& name) {
	renderSystemStreamers.erase(name);
}

RenderSystem::Streamer* RenderManager::GetRenderStreamer(const String& name) {
	auto it = renderSystemStreamers.find(name);
	if (it != renderSystemStreamers.end()) {
		return (*it).second;
	}
	return nullptr;
}

void RenderManager::GenerateStreamDataForQuad() {
	Geometry quadData = Geometry::CreateQuad(1, 1);
	VertexBufferPtr vertexBuffer = Assign(NEX_NEW(VertexBuffer(GpuBuffer::NEVER_RELEASED)));

	uint32 stride = ((sizeof(float) * 2));
	uint32 vbsize = (uint32)quadData.points.size() * stride;

	void* pVData = NEX_ALLOC(vbsize, MEMCAT_GENERAL);
	float* pos = (float*)pVData;
	for (uint32 i = 0; i < (uint32)quadData.points.size(); ++i) {
		auto &p = quadData.points[i];

		*pos++ = p.x;
		*pos++ = p.y;
	}

	vertexBuffer->CreateBuffer(vbsize, stride, reinterpret_cast<const uint8*>(pVData));

	StreamData* stream = &fullScreenQuad;
	stream->flags = StreamData::DELETE_BINDING;
	stream->type = PrimitiveType::PT_TRI_LIST;
	stream->vertices.count = (uint32)quadData.points.size();
	stream->vertices.start = 0;
	stream->vertices.layout = VertexLayout::GetCommonLayout(VertexLayoutType::POSITION2D_0).GetPtr();
	stream->vertices.binding = NEX_NEW(VertexBufferBinding());
	stream->vertices.binding->SetBufferCount(1);
	stream->vertices.binding->BindBuffer(0, vertexBuffer, 0);
	stream->indices.start = 0;
	stream->indices.count = 0;
	stream->instanceCount = 1;

	NEX_FREE(pVData, MEMCAT_GENERAL);
}

} /* namespace nextar */
