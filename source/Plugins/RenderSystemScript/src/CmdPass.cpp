/*
 * CmdPass.cpp
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#include <BaseRenderPass.h>
#include <CmdPass.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {


bool CmdPass::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	const ASTParameter& params = command->GetParameters();
	ConstMultiStringHelper h(params.AsString());
	// create the pass handler here
	c->_passType = h.Get(0);
	c->_pass = static_cast<BaseRenderPass*>(
			c->_rsys.AddPass(c->_passType));

	if (h.Length() > 1)
		c->_pass->SetName(h.Get(1));
	else
		c->_pass->SetName(c->_passType);
	c->_numUnresolvedTextures = 0;
	return true;
}

void CmdPass::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	if (c->_numUnresolvedTextures > 0) {
		CompositorRenderPass* pass = static_cast<CompositorRenderPass*>(c->_pass);
		pass->AddTexturesToResolve(c->_unresolvedTextures, c->_numUnresolvedTextures);
	}
	c->_pass = nullptr;
}

} /* namespace RenderSystemScript */
