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

void RenderScriptRoot::BeginDocument(CommandContext* pContext,
		const ASTDocument* pDoc) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	c->_rsys.BeginConfig(true);
}

void RenderScriptRoot::EndDocument(CommandContext* pContext,
		const ASTDocument* pDoc) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	c->_rsys.EndConfig();
}

} /* namespace RenderSystemScript */
