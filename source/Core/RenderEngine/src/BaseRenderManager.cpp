#include <NexRenderEngine.h>
#include <BaseRenderManager.h>
#include <Viewport.h>
#include <RenderTarget.h>
#include <DeferredRenderPass.h>
#include <DebugRenderPass.h>
#include <ForwardRenderPass.h>
#include <CompositorRenderPass.h>
#include <RenderSystemImpl.h>
#include <ApplicationContext.h>


namespace nextar {

BaseRenderManager::BaseRenderManager() {
#if NEX_MULTIGPU_BUILD
	emptySlot = 0;
#endif
}

BaseRenderManager::~BaseRenderManager() {
}

void BaseRenderManager::ConfigureImpl(const NameValueMap& c) {
	// @todo
	CreateDefaultRenderPassFactories();
	CreateRenderQueues(c);
	RegisterAutoParams();
	RegisterRenderSystemConfig(c);
}

void BaseRenderManager::CreateResources() {
	RenderManager::CreateResources();
	CreateAndLoadRenderSystems();
}

void BaseRenderManager::DestroyResources() {
	SaveAndCloseRenderSystems();
	RenderManager::DestroyResources();
}

void BaseRenderManager::RegisterRenderSystemConfig(const NameValueMap& c) {
	auto it = c.find("RenderSystems");
	if (it != c.end()) {
		renderSystemConfigs = StringUtils::TokenizeToMultiString((*it).second, ",");
	}
}

ContextID BaseRenderManager::RequestObjectCreate(ContextObject::Type type, uint32 hint) {
#if NEX_MULTIGPU_BUILD
	RenderContextList::iterator it = activeContexts.begin();
	RenderContextList::iterator en = activeContexts.end();
	for (; it != en; ++it) {
		ContextID out = (*it)->RegisterObject(traits);
		NEX_ASSERT(out == emptySlot);
	}
	return emptySlot++;
#else
	return primaryContext->RegisterObject(type, hint);
#endif
}

void BaseRenderManager::RequestObjectDestroy(ContextID traits) {

#if NEX_MULTIGPU_BUILD
		RenderContextList::iterator it = activeContexts.begin();
		RenderContextList::iterator en = activeContexts.end();
		for (; it != en; ++it) {
			(*it)->UnregisterObject(traits);
		}
		--emptySlot;
#else
	if (primaryContext) {
		primaryContext->UnregisterObject(traits);
	}
#endif

}

void BaseRenderManager::RequestObjectUpdate(ContextObject* traits,
		uint32 updateMsg, ContextObject::ContextParamPtr params) {
#if NEX_MULTIGPU_BUILD
		RenderContextList::iterator it = activeContexts.begin();
		RenderContextList::iterator en = activeContexts.end();
		for (; it != en; ++it) {
			(*it)->UpdateObject(traits, updateMsg, params);
		}
#else
	if (primaryContext)
		primaryContext->UpdateObject(traits, updateMsg, params);
#endif
}

void BaseRenderManager::RegisterRenderContext(RenderContextPtr& ptr) {
	NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
#if NEX_MULTIGPU_BUILD
	activeContexts.push_back(ptr);
#else
	if (!primaryContext) {
		primaryContext = ptr;
	}

#endif
}

void BaseRenderManager::Close() {
	CloseImpl();
		
#if NEX_MULTIGPU_BUILD
	RenderDriverList::iterator it = renderDrivers.begin();
	RenderDriverList::iterator en = renderDrivers.end();
	for (; it != en; ++it) {
		(*it)->Close();
	}
	renderDrivers.clear();
#else
	if(primaryDriver)
		primaryDriver->Close();
#endif
}

RenderDriverPtr BaseRenderManager::AsyncCreateDriver(DriverCreationParams& params) {
	RenderDriverPtr renderDrvPtr = CreateDriverImpl(params);
	if (renderDrvPtr) {

		if (params.createDefaultContext)
			renderDrvPtr->AsyncCreateContext(params.defaultContextParams);
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
#if NEX_MULTIGPU_BUILD
		renderDrivers.push_back(renderDrvPtr);
#else
		primaryDriver = renderDrvPtr;
#endif
	}
	return renderDrvPtr;
}

RenderDriverPtr BaseRenderManager::AsyncGetDriver(uint32 params) {
	NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
#if NEX_MULTIGPU_BUILD
	return renderDrivers[params];
#else
	return primaryDriver;
#endif
}

void BaseRenderManager::RenderFrame(const FrameTimer& frameTimer) {
	/**
	 * Handle culling step followed by rendering step.
	 */
#if NEX_MULTIGPU_BUILD
	RenderContextList::iterator it = activeContexts.begin();
	RenderContextList::iterator en = activeContexts.end();
	for (; it != en; ++it) {
		RenderAllTargets((*it), frameNumber, !renderSettings.syncPresent);
	}
#else
	if (!primaryContext) 
		return;
	RenderAllTargets(primaryContext, frameTimer,
						!renderSettings.syncPresent);
	
#endif

	if (renderSettings.syncPresent) {
#if NEX_MULTIGPU_BUILD
		RenderContextList::iterator it = activeContexts.begin();
		RenderContextList::iterator en = activeContexts.end();
		for (; it != en; ++it) {
			PresentSwapChains((*it));
		}
#else
		PresentSwapChains(primaryContext);
#endif
	}
}

void BaseRenderManager::RenderAllTargets(RenderContext* rc, const FrameTimer& frame,
		bool callPresent) {
#if NEX_MULTIGPU_BUILD
#error Not implemented
#else
	rc->BeginFrame(frame);
	RenderTargetList& rtList = rc->GetRenderTargetList();
	for (auto &rt : rtList) {

		Viewport::Iterator it = rt->GetViewports();
		for (; it; ++it) {
			(*it)->Render(rc, frame);
		}

		if (callPresent)
			rt->Present(rc);
	}
	rc->EndFrame();
#endif
}

void BaseRenderManager::PresentSwapChains(RenderContext* rc) {
	RenderTargetList& rtList = rc->GetRenderTargetList();
	for (auto &rt : rtList) {
		rt->Present(rc);
	}
}

void BaseRenderManager::CreateRenderQueues(const NameValueMap& section) {

	// @todo We run this only when configuration is not present
	bool queuesAdded = false;
	auto it = section.find("RenderQueueCount");
	if (it != section.end()) {
		uint32 l = Convert::ToULong((*it).second);
		for (uint32 i = 0; i < l; ++i) {
			String qname = String("RQ#") + Convert::ToString(i);
			auto it2 = section.find(qname);
			if (it2 != section.end()) {
				StringVector v = StringUtils::Tokenize((*it2).second, ",");
				if (v.size() > 1) {
					StringID nameId = Convert::ToULong(v[0]);
					uint32 priority = Convert::ToULong(v[1]);
					RenderQueueFlags flags = RenderQueueFlags::SORT_ENABLED | RenderQueueFlags::DEFERRED;
					if (v.size() > 2) {
						flags = (RenderQueueFlags)Convert::ToULong(v[2]);
					}
					AddRenderQueue(nameId, priority, flags);
					queuesAdded = true;
				}
			}
		}
	} 
	
	if (!queuesAdded) {
		AddRenderQueue(NamedObject::AsyncStringID("Background"), 110, RenderQueueFlags::BACKGROUND);
		AddRenderQueue(NamedObject::AsyncStringID("Deferred"), 111, RenderQueueFlags::DEFERRED | RenderQueueFlags::SORT_ENABLED);
		AddRenderQueue(NamedObject::AsyncStringID("Forward"), 112, RenderQueueFlags::FORWARD | RenderQueueFlags::SORT_ENABLED);
		AddRenderQueue(NamedObject::AsyncStringID("Transparent"), 113, RenderQueueFlags::TRANSLUCENCY | RenderQueueFlags::SORT_ENABLED);
		AddRenderQueue(NamedObject::AsyncStringID("Overlay"), 114, RenderQueueFlags::OVERLAY);
	}
}

void BaseRenderManager::CreateDefaultRenderPassFactories()  {
	AddRenderPassFactory("Compositor", &CompositorRenderPass::CreateInstance);
	AddRenderPassFactory("Deferred", &DeferredRenderPass::CreateInstance);
	AddRenderPassFactory("Forward", &ForwardRenderPass::CreateInstance);
	AddRenderPassFactory("Debug", &DebugRenderPass::CreateInstance);
}

RenderSystemPtr BaseRenderManager::GetRenderSystem(StringID name) {
	for (auto& e : renderSystems) {
		if (e->GetID() == name)
			return e;
	}
	return RenderSystemPtr();
}

void BaseRenderManager::CreateRenderSystem(const String& configName) {
	URL firstUrl(FileSystem::ArchiveProjectData_Name, "Configs/" + configName + ".rsys");
	bool bCompiled = true;
	InputStreamPtr input = FileSystem::Instance().OpenRead(firstUrl);
	if (!input) {
		URL secondUrl(FileSystem::ArchiveProjectData_Name, "Scripts/Configs/" + configName + ".rss");
		input = FileSystem::Instance().OpenRead(secondUrl);
		bCompiled = false;
	}

	if (input) {
		RenderSystemImplPtr rsys = Assign(NEX_NEW(RenderSystemImpl()));
		renderSystems.push_back(rsys);
		if (!bCompiled)
			rsys->SetName(configName);
		rsys->Load(input, bCompiled ? "RSYS" : "RSS");
		rsys->CreateResources();
	}
}

void BaseRenderManager::SaveAndCloseRenderSystems() {
	for (auto& e : renderSystems) {
		RenderSystemImplPtr rsys = e;
		rsys->DestroyResources();
	}
	renderSystems.clear();
}

void BaseRenderManager::CreateAndLoadRenderSystems() {
	ConstMultiStringHelper h(renderSystemConfigs);
	auto it = h.Iterate();
	String config;
	while (it.HasNext(config)) {
		CreateRenderSystem(config);
	}
}

}
