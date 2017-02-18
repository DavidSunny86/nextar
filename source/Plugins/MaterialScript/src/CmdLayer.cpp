/*
 * CmdLayer.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdLayer.h>
#include <MaterialScriptContext.h>

namespace MaterialScript {

bool CmdLayer::BeginExecute(CommandContext* pContext, const ASTCommand* command) const {
	MaterialScriptContext* c = static_cast<MaterialScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	SharedComponent::ID id = SharedComponent::NullID;
	uint8 layerValue = (uint8)Layer::NORMAL;
	String layer;
	if (it.HasNext(layer)) {
		if (layer == "normal")
			layerValue = (uint8)Layer::NORMAL;
		else if (layer == "lowest")
			layerValue = (uint8)Layer::LOWEST;
		else if (layer == "highest")
			layerValue = (uint8)Layer::HIGHEST;
		else
			layerValue = (uint8)Convert::ToULong(layer);
	}
	c->material->SetLayer(layerValue);

	return true;
}

} /* namespace MaterialScript */
