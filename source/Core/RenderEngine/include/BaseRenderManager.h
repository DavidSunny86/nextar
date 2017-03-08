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

	virtual RenderSystemPtr CreateRenderSystem(const String& configName, Size initialViewDimensions);

	inline const StringUtils::WordList& GetShaderOptions(Pass::ProgramStage::Type stage) {
		return shaderOptions[stage];
	}

protected:

	virtual void DestroyResources();
	virtual void CreateResources();

	virtual void CloseImpl() = 0;

	void RenderAllTargets(RenderContext* rc, const FrameTimer& frameTimer, bool callPresent);
	void PresentSwapChains(RenderContext* rc);
	void CreateRenderQueues(const NameValueMap&);
	void RegisterAutoParams();
	void RegisterRenderSystemConfig(const NameValueMap&);
	void CreateDefaultRenderPassFactories();
	void RegisterRenderScriptStreamer();
	void UnregisterRenderScriptStreamer();
	void RegisterGlobalShaderOptions(const NameValueMap&);
	/* The first registered context is always the primary context, other
	 * contexts are present if usingMultiGpuSetup is true */
	RenderDriverPtr primaryDriver;
	RenderContextPtr primaryContext;
	// materials will be regenrated upon modification to render
	// system, this is not supported right now
	// uint32 materialModification;
	// Render systems
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
