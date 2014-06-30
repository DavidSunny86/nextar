/*
 * RenderManager.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef BASERENDERMANAGER_H_
#define BASERENDERMANAGER_H_

#include <RenderEngine.h>
#include <RenderManager.h>

namespace nextar {

// Blank implementation for now.
class _NexRenderAPI BaseRenderManager: public RenderManager {
	NEX_LOG_HELPER(BaseRenderManager)
	;
public:

	BaseRenderManager();
	virtual ~BaseRenderManager();

	/* */
	virtual void Configure(const Config&);
	virtual RenderDriverPtr AsyncCreateDriver(DriverCreationParams&);
	virtual RenderDriverPtr AsyncGetDriver(uint32 index);
	virtual void Close();

	virtual ContextID RequestObjectCreate(ContextObject::Type, uint32 hint);
	virtual void RequestObjectDestroy(ContextID);
	virtual void RequestObjectUpdate(ContextObject*, uint32 updateMsg,
			ContextObject::ContextParamPtr);

	virtual void RegisterRenderContext(RenderContextPtr&);

	virtual void RenderFrame(uint32 frameNumber);

protected:

	virtual void CloseImpl() = 0;

	void RenderAllTargets(RenderContext* rc, uint32 frame, bool callPresent);
	void PresentSwapChains(RenderContext* rc);

	/* The first registered context is always the primary context, other
	 * contexts are present if usingMultiGpuSetup is true */
	RenderDriverPtr primaryDriver;
	RenderContextPtr primaryContext;
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
