/*
 * DeferredRenderSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef DEFERREDRENDERSYSTEM_H_
#define DEFERREDRENDERSYSTEM_H_

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderSystem.h>
#include <RenderTexture.h>
#include <MultiRenderTarget.h>

namespace nextar {

class _NexRenderAPI GBuffer: public AllocGeneral {
public:

	GBuffer();
	~GBuffer();

	void Setup(Size dimensions);
	void Destroy();

	RenderTexturePtr depth;
	RenderTexturePtr albedoSpecular;
	RenderTexturePtr normalMap;
	MultiRenderTargetPtr renderTarget;
};

class _NexRenderAPI DeferredRenderSystem: public nextar::RenderSystem {
public:

	DeferredRenderSystem();
	virtual ~DeferredRenderSystem();

	virtual void Commit(CommitContext& context);

	static void DestroyBuffers(void* renderSystem);

	inline void DestroyBuffer() {
		gbufferRI.rt = nullptr;
		gbuffer.Destroy();
	}

protected:

	void RenderLight(Light* light, uint32 passIdx, uint32 updateId, CommitContext& context);
	void PrepareGeometryBuffer();

	RenderInfo gbufferRI;
	Size gbufferDimension;
	GBuffer gbuffer;
};

} /* namespace nextar */
#endif /* DEFERREDRENDERSYSTEM_H_ */
