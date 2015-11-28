/*
 * ForwardRenderSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef ForwardRenderSystem_H_
#define ForwardRenderSystem_H_

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderSystem.h>
#include <RenderTexture.h>
#include <MultiRenderTarget.h>

namespace nextar {


class _NexRenderAPI ForwardRenderSystem: public nextar::RenderSystem {
public:

	ForwardRenderSystem();
	virtual ~ForwardRenderSystem();

	virtual void Commit(CommitContext& context);

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	inline void DestroyBuffer() {
		gbufferRI.rt = nullptr;
		gbuffer.Destroy();
		lightMaterial.Clear();
	}

protected:
		
	void RenderLight(Light* light, uint32 passIdx, uint32 updateId, CommitContext& context);
	void PrepareGeometryBuffer();
	void PrepareMaterials();
		
	Size gbufferDimension;
	GBuffer gbuffer;
};

} /* namespace nextar */
#endif /* ForwardRenderSystem_H_ */
