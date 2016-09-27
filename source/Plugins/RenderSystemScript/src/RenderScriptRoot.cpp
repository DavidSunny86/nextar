/*
 * RenderScriptRoot.cpp
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#include <RenderScriptRoot.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

RenderScriptRoot::RenderScriptRoot() : RegionHandler(&_instance) {
}

RenderScriptRoot::~RenderScriptRoot() {

}

bool RenderScriptRoot::BeginExecute(CommandContext* pContext, const ASTRegion* region, bool isText) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	c->_rsys.BeginConfig(true);
	return true;
}

void RenderScriptRoot::EndExecute(CommandContext* pContext, const ASTRegion* command, bool isText) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	c->_rsys.EndConfig();
}

} /* namespace RenderSystemScript */
