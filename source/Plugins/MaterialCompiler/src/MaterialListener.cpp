/*
 * MaterialListener.cpp
 *
 *  Created on: 26-Jul-2014
 *      Author: obhi
 */
#include <CommonTypes.h>
#include <MaterialScript.h>
#include <MaterialListener.h>
#include <ScriptStrings.h>

namespace MaterialCompiler {

CommandNamePair MaterialListener::commands[] = {
//{ "AutoParam", &AutoParamCmd::command },
		{ _SS(CMD_ENABLE_OPTIONS), &OptionsCmd_Execute },
		{ _SS(CMD_LAYER), &LayerCmd_Execute },
		{ _SS(CMD_PARAM_VALUE), &ParamValueCmd_Execute },
		{ _SS(CMD_SHADER), &ShaderCmd_Execute },
};

const size_t MaterialListener::commandCount = sizeof(MaterialListener::commands)
		/ sizeof(MaterialListener::commands[0]);

void MaterialListener::EnterBlock(
		ScriptParser::BlockContext& block) {
	block.ParseStatements(this);
}


void MaterialListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(MaterialListener::commands,
			MaterialListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(script, ctx);
	// todo else throw error command not supported
}


void MaterialListener::ShaderCmd_Execute(void* state,
		ScriptParser::StatementContext& ctx) {
	MaterialTemplate::StreamRequest* material =
				(reinterpret_cast<MaterialScript*>(state)->GetRequest());
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	ConstMultiStringHelper::Iterator it =  ConstMultiStringHelper::It(paramContext);
	String value;
	SharedComponent::ID id = SharedComponent::NullID;
	URL location;
	if (it.HasNext(value))
		location = URL(value);

	if (it.HasNext(value))
		id = SharedComponent::ToID(value);
	else
		id = Asset::ToID(location);
		
	material->SetShader(id, location);
}

void MaterialListener::OptionsCmd_Execute(void* state,
		ScriptParser::StatementContext& ctx) {
	MaterialTemplate::StreamRequest* material =
				(reinterpret_cast<MaterialScript*>(state)->GetRequest());
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	material->SetCompilationOptions(paramContext);
}

void MaterialListener::ParamValueCmd_Execute(void* state,
		ScriptParser::StatementContext& ctx) {
	MaterialTemplate::StreamRequest* material =
				(reinterpret_cast<MaterialScript*>(state)->GetRequest());
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(ctx.GetParamList());
	String name, valueTmp;
	if (it.HasNext(name)) {
		return;
	}
	String value;
	while( (it.HasNext(valueTmp)) ) {
		StringUtils::PushBackWord(value, valueTmp);
	}
	if (value.length()) {
		material->AddParam(name, value);
	}
}

void MaterialListener::LayerCmd_Execute(void* state,
		ScriptParser::StatementContext& ctx) {
	MaterialTemplate::StreamRequest* material =
				(reinterpret_cast<MaterialScript*>(state)->GetRequest());
	String layer = ConstMultiStringHelper(ctx.GetParamList()).Get(0);
	uint8 layerValue = (uint8)Layer::NORMAL;
	if (layer == _SS(ARG_NORMAL))
		layerValue = (uint8)Layer::NORMAL;
	else if (layer == _SS(ARG_LOWEST))
		layerValue = (uint8)Layer::LOWEST;
	else if (layer == _SS(ARG_HIGHEST))
		layerValue = (uint8)Layer::HIGHEST;
	else
		layerValue = (uint8)Convert::ToULong(layer);
	material->SetLayer(layerValue);
}

} /* namespace MaterialCompiler */
