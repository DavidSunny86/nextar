/*
 * CmdEnabled.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CmdEnabled.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {


bool CmdEnabled::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	ConstMultiStringHelper h(command->GetParameters().AsString());
	auto it = h.Iterate();
	String value;
	if (it.HasNext(value)) {
		c->_pass->SetEnabled(Convert::ToBool(value));
	}
	return true;
}

}
