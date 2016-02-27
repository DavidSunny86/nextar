/*
 * CmdCleanDepth.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CmdCleanDepth.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

void CmdCleanDepth::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	ConstMultiStringHelper h(command->GetParameters().AsString());
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	float cleanDepthValue = 1.0f;
	if (h.Length() > 0)
		cleanDepthValue = Convert::ToFloat(h.Get(0));
	c->_pass->SetCleanDepth(true, cleanDepthValue);
}

} /* namespace RenderSystemScript */
