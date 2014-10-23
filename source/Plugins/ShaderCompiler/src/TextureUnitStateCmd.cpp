/*
 * TextureUnitStateCmd.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <EngineHeaders.h>
#include <TextureUnitState.h>
#include <TextureUnitStateCmd.h>
#include <ShaderAsset.h>
#include <ScriptStrings.h>
#include <ShaderScript.h>

namespace ShaderCompiler {

TextureUnitStateCmd TextureUnitStateCmd::command;
UnitCmd UnitCmd::command;

/**************************************************************
 * TextureUnitState
 **************************************************************/
void TextureUnitStateCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegate::PASS_BLOCK) {
		ShaderScript* script = reinterpret_cast<ShaderScript*>(parentParam);
		TextureUnitStateListener textureUnitState(script->GetRequest());
		ctx.ParseBlock(&textureUnitState);
	} else {
		ctx.Error("TextureState block needs to be inside Shader declaration.");
	}
}

void TextureUnitStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void TextureUnitStateListener::EnterStatement(
		ScriptParser::StatementContext& ctx) {
	//CommandDelegate* cmd = Helper::FindCommand(DepthStencilStateListener::commands,
	//		DepthStencilStateListener::commandCount, ctx.command);
	if (ctx.GetCommand() == _SS(CMD_UNIT)) {
		UnitCmd::command.Execute(CommandDelegate::TEXTURE_STATE_BLOCK, shader,
				ctx);
	}
}

void UnitCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::TEXTURE_STATE_BLOCK);
	TextureUnitParams texUnitParams;
	ShaderTemplate::LoadStreamRequest* shader =
			(static_cast<ShaderTemplate::LoadStreamRequest*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	String name;
	URL defaultTexturePath;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		name = value;
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.minFilter = Helper::GetMinFilter(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.magFilter = Helper::GetMagFilter(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.uAddress = Helper::GetTextureAddressMode(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.vAddress = Helper::GetTextureAddressMode(value);
	} else {
		texUnitParams.vAddress = texUnitParams.uAddress;
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.wAddress = Helper::GetTextureAddressMode(value);
	}
	else {
		texUnitParams.vAddress = texUnitParams.uAddress;
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.comparisonFunc = Helper::GetTextureCompareFunc(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.maxAnisotropy = (uint32)Convert::ToULong(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.minLod = Convert::ToFloat(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.maxLod = Convert::ToFloat(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.lodBias = Convert::ToFloat(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		texUnitParams.borderColor = Convert::ToColor(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		defaultTexturePath = URL(value);
	}
	// todo else throw error command not supported
	if (!ctx.IsErrorBitSet()) {
		shader->AddTextureUnit(name, texUnitParams, defaultTexturePath);
	}
}

} /* namespace ShaderCompiler */
