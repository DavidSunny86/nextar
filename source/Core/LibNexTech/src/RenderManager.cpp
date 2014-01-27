/*
 * RenderManager.cpp
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "VertexElement.h"
#include "RenderManager.h"
#include "RenderContext.h"

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(RenderManager);
	RenderManager::RenderManager() : usingMultiGpuSetup(false),
			maxRenderLayer(0) {
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

	void RenderManager::RegisterRenderContext(RenderContextPtr& ptr) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		if (!primaryContext && !usingMultiGpuSetup)
			primaryContext = ptr;
		else {
			activeContexts.push_back(ptr);
		}
	}

	void RenderManager::NotifyObjectCreated(ContextObject* traits) {
		if (primaryContext) {
			primaryContext->RegisterObject(traits);
		} else {
			RenderContextList::iterator it = activeContexts.begin();
			RenderContextList::iterator en = activeContexts.end();
			for(; it != en; ++it) {
				(*it)->RegisterObject(traits);
			}
		}
	}

	void RenderManager::NotifyObjectDestroyed(ContextObject* traits) {
		if (primaryContext) {
			primaryContext->UnregisterObject(traits);
		} else {
			RenderContextList::iterator it = activeContexts.begin();
			RenderContextList::iterator en = activeContexts.end();
			for(; it != en; ++it) {
				(*it)->UnregisterObject(traits);
			}
		}
	}

	void RenderManager::NotifyObjectUpdated(ContextObject* traits, ContextObject::UpdateParamPtr params) {
		if (primaryContext) {
			primaryContext->UpdateObject(traits, params);
		} else {
			RenderContextList::iterator it = activeContexts.begin();
			RenderContextList::iterator en = activeContexts.end();
			for(; it != en; ++it) {
				(*it)->UpdateObject(traits, params);
			}
		}
	}

	void RenderManager::AddRenderLayer(const String& name, uint16 priority, uint16 flags) {
		if (renderLayers.size() >= 255) {
			Error("Render layer index must be less than 255");
			return;
		}
		renderLayers.emplace_back(flags, priority, name);
		std::sort(renderLayers.begin(), renderLayers.end());
	}

	const RenderLayerList& RenderManager::GetRenderLayerInfo() const {
		return renderLayers;
	}

	void RenderManager::RenderFrame(uint32 frameNumber) {
		/**
		 * Handle culling step followed by rendering step.
		 */
	}
} /* namespace nextar */
