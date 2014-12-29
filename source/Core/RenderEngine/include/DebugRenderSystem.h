/*
 * DebugRenderSystem.h
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_
#define CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_

namespace nextar {

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderSystem.h>

class DebugRenderSystem: public nextar::RenderSystem {
public:
	DebugRenderSystem();
	virtual ~DebugRenderSystem();
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_ */
