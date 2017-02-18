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
#include <BaseRenderPass.h>
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

class _NexRenderAPI DeferredRenderPass: public nextar::BaseRenderPass {
public:

	typedef MetaType<DeferredRenderPass>::Type MetaType;

	DeferredRenderPass(const Meta* meta);
	virtual ~DeferredRenderPass();

	virtual void Commit(CommitContext& context);


	inline void DestroyBuffer() {
		gbufferRI.rt = nullptr;
		gbuffer.Destroy();
		lightMaterial.Clear();
	}

	virtual void CreateResources();
	virtual void DestroyResources();

	void SetLightMaterial(MaterialAssetPtr& lightMaterial);
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
