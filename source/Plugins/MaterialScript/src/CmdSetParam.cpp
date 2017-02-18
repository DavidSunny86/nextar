/*
 * CmdParamValue.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdSetParam.h>
#include <MaterialScriptContext.h>

namespace MaterialScript {

bool CmdSetParam::BeginExecute(CommandContext* pContext, const ASTCommand* command) const {
	MaterialScriptContext* c = static_cast<MaterialScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;
	String name;
	if (it.HasNext(name)) {
		String value, valueTmp;
		while( (it.HasNext(valueTmp)) ) {
			StringUtils::PushBackWord(value, valueTmp);
		}
		if (value.length()) {
			c->material->AddParam(name, value);
		}
	}
	return true;
}

} /* namespace MaterialScript */
