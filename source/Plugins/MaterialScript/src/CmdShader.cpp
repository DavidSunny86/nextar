/*
 * CmdShader.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdShader.h>
#include <MaterialScriptContext.h>

namespace MaterialScript {

bool CmdShader::BeginExecute(CommandContext* pContext, const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	MaterialScriptContext* c = static_cast<MaterialScriptContext*>(pContext);
	String value;
	ConstMultiStringHelper::Iterator it =  h.Iterate();
	SharedComponent::ID id = SharedComponent::NullID;
	URL location;
	if (it.HasNext(value))
		location = URL(value, "fxscript", FileSystem::ArchiveProjectData + "/Scripts/Effects");

	if (it.HasNext(value))
		id = SharedComponent::ToID(value);
	else
		id = Asset::ToID(location);

	c->material->SetShader(id, location);
	return true;
}

} /* namespace MaterialScript */
