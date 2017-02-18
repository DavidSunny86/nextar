/*
 * CmdMaterial.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <BaseMaterialPass.h>
#include <CmdMaterial.h>
#include <RenderScriptContext.h>
#include <DeferredRenderPass.h>
#include <MaterialTemplate.h>

namespace RenderSystemScript {

bool CmdMaterial::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->_resolver);
	String value;
	SharedComponent::ID id = SharedComponent::NullID;
	URL location;

	if (it.HasNext(value))
		location = URL(value);
	if (it.HasNext(value))
		id = SharedComponent::ToID(value);
	else
		id = Asset::ToID(location);

	MaterialAssetPtr m = LoadMaterial(id, location);
	if (c->_passType.find("Compositor") != String::npos) {
		BaseMaterialPass* pass = static_cast<BaseMaterialPass*>(c->_pass);
		pass->SetMaterial( m );
	} else if (c->_passType.find("Deferred") != String::npos) {
		DeferredRenderPass* pass = static_cast<DeferredRenderPass*>(c->_pass);
		pass->SetLightMaterial(m);
	}

	return true;
}

MaterialAssetPtr CmdMaterial::LoadMaterial(const SharedComponent::ID& id, 
	const URL& url) {

	MaterialTemplatePtr mtlTemplatePtr = MaterialTemplate::Traits::Instance(id, url);
	if (mtlTemplatePtr->RequestLoad()) {
		return mtlTemplatePtr->GetMaterial();
	}
	return MaterialAssetPtr();
}

} /* namespace RenderSystemScript */
