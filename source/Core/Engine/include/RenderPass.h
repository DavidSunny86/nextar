/*
 * RenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include <RenderContext.h>
#include <VisibilitySet.h>

namespace nextar {

class _NexEngineAPI RenderPass: public AllocGraphics {
public:

	typedef RenderPass* (*CreateInstance) (const Config&);

	RenderPass(const Config&);
	virtual ~RenderPass();

	virtual void Commit(CommitContext& context) = 0;
};

typedef vector<RenderPass*>::type RenderPassList;

} /* namespace nextar */
#endif /* RENDERPASS_H_ */
