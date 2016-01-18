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

	ForwardRenderSystem(const Config& c);
	virtual ~ForwardRenderSystem();

	virtual void Commit(CommitContext& context);

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	static RenderSystem* CreateInstance(const Config& c);

protected:
		
	void PrepareMaterials();
};

} /* namespace nextar */
#endif /* ForwardRenderSystem_H_ */
