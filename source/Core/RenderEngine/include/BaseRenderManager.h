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
	class _NexRenderAPI BaseRenderManager : public RenderManager {
		NEX_LOG_HELPER(BaseRenderManager);
	public:

		BaseRenderManager();
		virtual ~BaseRenderManager();

		/* */
		virtual void Configure(const Config&);
		
		virtual void RequestObjectCreate(ContextObject*);
		virtual void RequestObjectDestroy(ContextObject*);
		virtual void RequestObjectUpdate(ContextObject*, uint32 updateMsg, ContextObject::ContextParamPtr);

		virtual void RegisterRenderContext(RenderContextPtr&);

		virtual void RenderFrame(uint32 frameNumber);

	protected:

		void RenderAllTargets(RenderContext* rc, uint32 frame, bool callPresent);
		void PresentSwapChains(RenderContext* rc);

		/* The first registered context is always the primary context, other
		 * contexts are present if usingMultiGpuSetup is true */
		RenderContextPtr primaryContext;
		RenderContextList activeContexts;

	};
} /* namespace nextar */
#endif /* BASERENDERMANAGER_H_ */
