/*
 * CmdPass.cpp
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#include <BaseRenderPass.h>
#include <CmdPass.h>

namespace RenderSystemScript {


void CmdPass::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	const ASTParameterList& params = command->GetParameters();
	ConstMultiStringHelper h(params.AsString());
	// create the pass handler here
	c->_passType = h.Get(0);
	c->_pass = static_cast<BaseRenderPass*>(
			RenderManager::Instance().CreateRenderPass(c->_passType));
	if (h.Length() > 1)
		c->_pass->SetName(h.Get(1));
	else
		c->_pass->SetName(c->_passType);
	c->_numUnresolvedTextures = 0;
}

void CmdPass::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	if (c->_numUnresolvedTextures > 0) {
		CompositorRenderPass* pass = static_cast<CompositorRenderPass*>(c->_pass);
		pass->AddTexturesToResolve(c->_unresolvedTextures, c->_numUnresolvedTextures);
	}
	if (c->_pass) {
		c->_rsys.AddPass(
				static_cast<RenderScriptContext*>(pContext)->_pass
				);
		c->_pass = nullptr;
	}
}

} /* namespace RenderSystemScript */
