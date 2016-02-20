/*
 * CmdRenderTo.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CmdRenderTo.h>
#include <BaseRenderPass.h>

namespace RenderSystemScript {

void CmdRenderTo::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);

	String target = h.Get(0);
	RenderTargetName lastTarget = CommitContext::ParseTargetName(target);
	BaseRenderPass* pass = c->_pass;
	if (target != StringUtils::Null)
		pass->SetTarget(c->_rsys.GetTarget(NamedObject::AsyncStringID(target)));
	else
		pass->SetTarget(lastTarget);
}

} /* namespace RenderSystemScript */
