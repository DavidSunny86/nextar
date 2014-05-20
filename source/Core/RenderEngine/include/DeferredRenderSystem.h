/*
 * DeferredRenderSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef DEFERREDRENDERSYSTEM_H_
#define DEFERREDRENDERSYSTEM_H_

#include <RenderEngine.h>
#include <EngineHeaders.h>
#include <RenderSystem.h>

namespace nextar {

	class _NexRenderAPI GBuffer : public AllocGeneral {
	public:

		void Create();
		void Setup(Size dimensions);


		RenderTexture* depth;
		RenderTexture* albedoSpecular;
		RenderTexture* normalGloss;
		MultiRenderTargetPtr renderTarget;
	};

	class _NexRenderAPI DeferredRenderSystem: public nextar::RenderSystem {
	public:
		DeferredRenderSystem();
		virtual ~DeferredRenderSystem();

		virtual void Commit(CommitContext& context, RenderContextPtr& renderCtx);

	protected:

		void PrepareGeometryBuffer();

		RenderInfo gbufferRI;
		Size gbufferDimension;
		GBuffer gbuffer;
	};

} /* namespace nextar */
#endif /* DEFERREDRENDERSYSTEM_H_ */
