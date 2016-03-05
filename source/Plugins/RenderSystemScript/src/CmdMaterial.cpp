/*
 * CmdMaterial.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <BaseMaterialPass.h>
#include <CmdMaterial.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

bool CmdMaterial::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	ConstMultiStringHelper::Iterator it =  ConstMultiStringHelper::It(command->GetParameters().AsString());
	String value;
	SharedComponent::ID id = SharedComponent::NullID;
	URL location;

	if (it.HasNext(value))
		location = URL(value);
	if (it.HasNext(value))
		id = SharedComponent::ToID(value);
	else
		id = Asset::ToID(location);

	if (c->_passType.find("Compositor") != String::npos) {
		BaseMaterialPass* pass = static_cast<BaseMaterialPass*>(c->_pass);
		MaterialAssetPtr m = MaterialAsset::Traits::Instance(id, location);
		pass->SetMaterial( m );
	}

	return true;
}

} /* namespace RenderSystemScript */
