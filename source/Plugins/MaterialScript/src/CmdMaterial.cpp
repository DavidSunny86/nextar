/*
 * MaterialCmd.cpp
 *
 *  Created on: 04-Mar-2016
 *      Author: obhi
 */

#include <CmdMaterial.h>
#include <MaterialScriptContext.h>

namespace MaterialScript {

bool CmdMaterial::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	MaterialScriptContext* c = static_cast<MaterialScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String name;
	if (it.HasNext(name)) {
		SharedComponent::ID id = SharedComponent::ToID(name);
		if (id.name == c->materialId.name &&
			id.group == c->materialId.group) {
			c->material->SetMaterialID(id);
			return true;
		}
	} else {
		// log we have a problem
	}
	return false;
}

void CmdMaterial::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
}

} /* namespace MaterialScript */

