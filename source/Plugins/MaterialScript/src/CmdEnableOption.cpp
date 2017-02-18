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
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String optionList;
	String option;
	while (it.HasNext(option))
		StringUtils::PushBackWord(optionList, option);
	c->material->EnableOptions(optionList);
	return true;
}

} /* namespace MaterialScript */
