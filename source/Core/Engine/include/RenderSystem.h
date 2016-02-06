/*
 * RenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERSYSTEM_H_
#define RENDERSYSTEM_H_

#include <RenderContext.h>
#include <RenderPass.h>

namespace nextar {

class _NexEngineAPI RenderSystem: public AllocGraphics {
public:
		

	RenderSystem(const Config&);
	virtual ~RenderSystem();
	
	virtual void Commit(CommitContext& context) = 0;

protected:
		
};

typedef vector<RenderPass*>::type RenderPassList;

} /* namespace nextar */
#endif /* RENDERSYSTEM_H_ */
