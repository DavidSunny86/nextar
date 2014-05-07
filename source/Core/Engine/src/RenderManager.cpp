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

	void RenderManager::AddRenderQueue(const String& name, uint16 priority, uint16 flags) {
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

	void RenderManager::RenderFrame(uint32 frameNumber) {
		/**
		 * Handle culling step followed by rendering step.
		 */
		if (primaryContext) {
			RenderAllTargets(primaryContext, frameNumber, !renderSettings.syncPresent);
		} else {
			RenderContextList::iterator it = activeContexts.begin();
			RenderContextList::iterator en = activeContexts.end();
			for(; it != en; ++it) {
				RenderAllTargets((*it), frameNumber, !renderSettings.syncPresent);
			}
		}

		if (renderSettings.syncPresent) {
			if (primaryContext) {
				PresentSwapChains(primaryContext);
			} else {

				RenderContextList::iterator it = activeContexts.begin();
				RenderContextList::iterator en = activeContexts.end();
				for(; it != en; ++it) {
					PresentSwapChains((*it));
				}
			}
		}
	}

	void RenderManager::RenderAllTargets(RenderContext* rc, uint32 frame, bool callPresent) {
		
		rc->BeginFrame(frame);
		RenderTargetList& rtList = rc->GetRenderTargetList();
		for (auto &rt : rtList) {

			Viewport::Iterator it = rt->GetViewports();
			for(; it; ++it) {
				(*it)->Render(rc, frame);
			}

			if (callPresent) 
				rt->Present(rc);
		}
		rc->EndFrame();
	}

	void RenderManager::PresentSwapChains(RenderContext* rc) {
		RenderTargetList& rtList = rc->GetRenderTargetList();
		for (auto &rt : rtList) {
			rt->Present(rc);
		}
	}

} /* namespace nextar */
