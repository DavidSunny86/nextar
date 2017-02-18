/*
 * CmdRenderTo.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CmdRenderTo.h>
#include <BaseRenderPass.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

bool CmdRenderTo::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->_resolver);

	String target;
	it.HasNext(target);
	RenderTargetName lastTarget = CommitContext::ParseTargetName(target);
	BaseRenderPass* pass = c->_pass;
	if (target != StringUtils::Null && lastTarget == RenderTargetName::RT_NONE)
		pass->SetTarget(c->_rsys.GetTarget(StringUtils::GetStringID(target)));
	else
		pass->SetTarget(lastTarget);
	return true;
}

} /* namespace RenderSystemScript */
