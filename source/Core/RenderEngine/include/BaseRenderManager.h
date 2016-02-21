/*
 * RenderManager.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef BASERENDERMANAGER_H_
#define BASERENDERMANAGER_H_

#include <NexRenderEngine.h>
#include <RenderManager.h>
#include <FrameTimer.h>

namespace nextar {

// Blank implementation for now.
class _NexRenderAPI BaseRenderManager: public RenderManager {
	NEX_LOG_HELPER(BaseRenderManager)
	;
public:

	BaseRenderManager();
	virtual ~BaseRenderManager();

	/* */
	virtual void ConfigureImpl(const NameValueMap&) override;
	virtual RenderDriverPtr AsyncCreateDriver(DriverCreationParams&);
	virtual RenderDriverPtr AsyncGetDriver(uint32 index);
	virtual void Close();

	virtual ContextID RequestObjectCreate(ContextObject::Type, uint32 hint);
	virtual void RequestObjectDestroy(ContextID);
	virtual void RequestObjectUpdate(ContextObject*, uint32 updateMsg,
			ContextObject::ContextParamPtr);

	virtual void RegisterRenderContext(RenderContextPtr&);
	virtual void RenderFrame(const FrameTimer&) override;

	virtual void CreateRenderSystem(const String& configName);
	virtual RenderSystemPtr GetRenderSystem(StringID name);
protected:

	virtual void DestroyResources();
	virtual void CreateResources();

	virtual void CloseImpl() = 0;

	void CreateAndLoadRenderSystems();
	void SaveAndCloseRenderSystems();
	void RenderAllTargets(RenderContext* rc, const FrameTimer& frameTimer, bool callPresent);
	void PresentSwapChains(RenderContext* rc);
	void CreateRenderQueues(const NameValueMap&);
	void RegisterAutoParams();
	void RegisterRenderSystemConfig(const NameValueMap&);
	void CreateDefaultRenderPassFactories();

	typedef list<RenderSystemPtr>::type RenderSystemList;

	/* The first registered context is always the primary context, other
	 * contexts are present if usingMultiGpuSetup is true */
	RenderDriverPtr primaryDriver;
	RenderContextPtr primaryContext;
	// Render systems
	RenderSystemList renderSystems;

	String renderSystemConfigs;
#if NEX_MULTIGPU_BUILD
	typedef vector<RenderDriverPtr>::type RenderDriverList;
	typedef vector<RenderContextPtr>::type RenderContextList;
	typedef vector<ContextObject*>::type GpuObjectTraitsList;

	ContextID emptySlot;
	RenderContextList activeContexts;
	RenderDriverList renderDrivers;
	/* List of objects needs readying */
	GpuObjectTraitsList objectsToReady;
#endif
};

} /* namespace nextar */
#endif /* BASERENDERMANAGER_H_ */
