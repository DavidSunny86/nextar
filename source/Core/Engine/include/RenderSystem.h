/*
 * RenderSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERSYSTEM_H_
#define RENDERSYSTEM_H_

#include <RenderContext.h>
#include <VisibilitySet.h>

namespace nextar {

class _NexEngineAPI RenderSystem: public AllocGraphics {
public:

	typedef RenderSystem* (*CreateInstance) (const Config&);

	RenderSystem(const Config&);
	virtual ~RenderSystem();

	virtual void Commit(CommitContext& context) = 0;
};

typedef vector<RenderSystem*>::type RenderSystemList;
} /* namespace nextar */
#endif /* RENDERSYSTEM_H_ */
