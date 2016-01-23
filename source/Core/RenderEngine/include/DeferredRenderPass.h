/*
 * DeferredRenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef DEFERREDRENDERSYSTEM_H_
#define DEFERREDRENDERSYSTEM_H_

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderPass.h>
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
	RenderTexturePtr albedoMap;
	RenderTexturePtr normalMap;
	RenderTexturePtr specularAndGlossMap;
	MultiRenderTargetPtr renderTarget;
};

class _NexRenderAPI DeferredRenderPass: public nextar::RenderPass {
public:

	DeferredRenderPass(const Config& c);
	virtual ~DeferredRenderPass();

	virtual void Commit(CommitContext& context);

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	inline void DestroyBuffer() {
		gbufferRI.rt = nullptr;
		gbuffer.Destroy();
		lightMaterial.Clear();
	}

	static RenderPass* CreateInstance(const Config& c);

protected:
		
	void RenderLight(Light* light, uint32 passIdx, uint32 updateId, CommitContext& context);
	void PrepareGeometryBuffer();
	void PrepareMaterials();
		
	MaterialAssetPtr lightMaterial;
	
	RenderInfo gbufferRI;
	Size gbufferDimension;
	GBuffer gbuffer;
};

} /* namespace nextar */
#endif /* DEFERREDRENDERSYSTEM_H_ */
