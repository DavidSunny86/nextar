/*
 * DebugRenderSystem.cpp
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#include <DebugRenderSystem.h>
#include <DebugDisplay.h>

namespace nextar {

DebugRenderSystem::DebugRenderSystem() {
	// TODO Auto-generated constructor stub

}

DebugRenderSystem::~DebugRenderSystem() {
	// TODO Auto-generated destructor stub
}

void DebugRenderSystem::Commit(CommitContext& context) {

	context.renderContext->BeginRender(&context.renderTargetInfo);
	VisiblePrimitiveList& primitives = context.debugDisplay->GetPrimitives(context);
	for (auto &prim : primitives) {
		//prim->material
	}
	context.renderContext->EndRender();
}

} /* namespace nextar */
