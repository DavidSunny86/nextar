/*
 * MaterialScript.cpp
 *
 *  Created on: 26-Jul-2014
 *      Author: obhi
 */
#include <CommonTypes.h>
#include <MaterialScript.h>
#include <ScriptStrings.h>
#include <MaterialListener.h>

namespace MaterialCompiler {

MaterialScript::MaterialScript(MaterialTemplate::StreamRequest* request) :
		material(request) {
	auto mtl = static_cast<MaterialTemplate*>(GetRequest()->GetStreamedObject());
	mtl->GetID(materialId);
}

void MaterialScript::EnterRegion(ScriptParser::RegionContext& ctx) {
	ctx.ParseStatements(this);
}

void MaterialScript::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void MaterialScript::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_MATERIAL)) {
		ConstMultiStringHelper h(ctx.GetParamList());
		String name = h.Get(0);
		SharedComponent::ID id = SharedComponent::ToID(name);
		if (id.name == this->materialId.name &&
			id.group == this->materialId.group) {
			material->SetMaterialID(id);
			MaterialListener ml(this);
			ctx.ParseBlock(&ml);
		} else
			ctx.ParseBlock(nullptr);
	}
}

} /* namespace MaterialCompiler */
