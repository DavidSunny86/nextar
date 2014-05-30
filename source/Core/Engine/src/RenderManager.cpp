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

	NEX_DEFINE_SINGLETON_PTR(RenderManager);
	RenderManager::RenderManager() : usingMultiGpuSetup(false) {
		VertexSemantic::BuildSemanticMap();
	}

	RenderManager::~RenderManager() {
		VertexSemantic::ClearSemanticMap();
	}

	void RenderManager::Configure(const Config& config) {
		const NameValueMap& nv = config.GetSectionMap("Render");
		ConfigureImpl(nv);
	}

	void RenderManager::Close() {
		CloseImpl();
		RenderDriverList::iterator it = renderDrivers.begin();
		RenderDriverList::iterator en = renderDrivers.end();
		for(; it != en; ++it) {
			(*it)->Close();
		}
		renderDrivers.clear();
	}

	RenderDriverPtr RenderManager::AsyncCreateDriver(DriverCreationParams& params) {
		RenderDriverPtr renderDrvPtr = CreateDriverImpl(params);
		if (renderDrvPtr) {
			if (params.createDefaultContext)
				renderDrvPtr->AsyncCreateContext(params.defaultContextParams);
			NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
			renderDrivers.push_back(renderDrvPtr);
		}
		return renderDrvPtr;
	}


	void RenderManager::AddRenderQueue(const String& name, uint16 priority, RenderQueueFlags flags) {
		if (renderQueues.size() >= 255) {
			Error("Render queue index must be less than 255");
			return;
		}
		RenderQueueDesc desc;
		desc.name = name;
		desc.priority = priority;
		desc.flags = flags;
		renderQueues.push_back(desc);
		std::sort(renderQueues.begin(), renderQueues.end());
	}

	const RenderQueueDescList& RenderManager::GetRenderQueueInfo() const {
		return renderQueues;
	}


	RenderSystemList& RenderManager::GetRenderSystems()	{
		return renderSystems;
	}
	
} /* namespace nextar */
