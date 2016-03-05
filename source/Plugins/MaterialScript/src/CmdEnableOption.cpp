/*
 * CmdEnableOption.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdEnableOption.h>
#include <MaterialScriptContext.h>

namespace MaterialScript {

bool CmdEnableOption::BeginExecute(CommandContext* pContext, const ASTCommand* command) const {
	MaterialScriptContext* c = static_cast<MaterialScriptContext*>(pContext);
	c->material->SetCompilationOptions(command->GetParameters().AsString());
	return true;
}

} /* namespace MaterialScript */
