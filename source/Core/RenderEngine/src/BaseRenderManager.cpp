
#include <RenderEngine.h>
#include <BaseRenderManager.h>

namespace nextar {

	BaseRenderManager::BaseRenderManager() {
	}

	BaseRenderManager::~BaseRenderManager() {
	}

	void BaseRenderManager::Configure(const Config&) {
		// @todo
	}

	void BaseRenderManager::RequestObjectCreate(ContextObject* traits) {
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

	void BaseRenderManager::RequestObjectDestroy(ContextObject* traits) {
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

	void BaseRenderManager::RequestObjectUpdate(ContextObject* traits, uint32 updateMsg,
			ContextObject::ContextParamPtr params) {
		if (primaryContext) {
			primaryContext->UpdateObject(traits, updateMsg, params);
		} else {
			RenderContextList::iterator it = activeContexts.begin();
			RenderContextList::iterator en = activeContexts.end();
			for(; it != en; ++it) {
				(*it)->UpdateObject(traits, updateMsg, params);
			}
		}
	}

	void BaseRenderManager::RegisterRenderContext(RenderContextPtr& ptr) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		if (!primaryContext && !usingMultiGpuSetup)
			primaryContext = ptr;
		else {
			activeContexts.push_back(ptr);
		}
	}

	void BaseRenderManager::RenderFrame(uint32 frameNumber) {
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

	void BaseRenderManager::RenderAllTargets(RenderContext* rc, uint32 frame, bool callPresent) {

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

	void BaseRenderManager::PresentSwapChains(RenderContext* rc) {
		RenderTargetList& rtList = rc->GetRenderTargetList();
		for (auto &rt : rtList) {
			rt->Present(rc);
		}
	}

}
