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
	materialName = mtl->GetID();
}

void MaterialScript::EnterRegion(ScriptParser::RegionContext& ctx) {
	ctx.ParseStatements(this);
}

void MaterialScript::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void MaterialScript::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_MATERIAL)) {
		String name;
		StringUtils::NextWord(ctx.GetParamList(), name);
		SharedComponent::ID id = SharedComponent::ToID(name);
		if (id.name == this->materialName) {
			material->SetMaterialID(id);
			MaterialListener ml(this);
			ctx.ParseBlock(&ml);
		} else
			ctx.ParseBlock(nullptr);
	}
}

} /* namespace MaterialCompiler */
