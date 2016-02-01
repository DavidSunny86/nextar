/*
 * ForwardRenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef ForwardRenderPass_H_
#define ForwardRenderPass_H_

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderPass.h>
#include <RenderTexture.h>
#include <MultiRenderTarget.h>
#include <MultiRenderTarget.h>

namespace nextar {


class _NexRenderAPI ForwardRenderPass: public nextar::RenderPass {
public:

	ForwardRenderPass(const Config& c);
	virtual ~ForwardRenderPass();

	virtual void Commit(CommitContext& context);

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	static RenderPass* CreateInstance(const Config& c);

protected:
	
	void Render(CommitContext& context);
	void PrepareMaterials();

};

} /* namespace nextar */
#endif /* ForwardRenderPass_H_ */
