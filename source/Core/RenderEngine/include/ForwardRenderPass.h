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
#include <BaseRenderPass.h>
#include <RenderTexture.h>
#include <MultiRenderTarget.h>
#include <MultiRenderTarget.h>

namespace nextar {


class _NexRenderAPI ForwardRenderPass: public nextar::BaseRenderPass {
public:

	typedef MetaType<ForwardRenderPass>::Type MetaType;

	ForwardRenderPass(const Meta* meta);
	virtual ~ForwardRenderPass();

	virtual void Commit(CommitContext& context);

	virtual void CreateResources();
	virtual void DestroyResources();

protected:

	void PrepareMaterials();

};

} /* namespace nextar */
#endif /* ForwardRenderPass_H_ */
