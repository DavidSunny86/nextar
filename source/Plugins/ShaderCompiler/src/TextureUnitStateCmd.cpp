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

CommandNamePair TextureUnitStateListener::commands[] = {
	{ _SS(CMD_SAMPLER), &SamplerCmd_Execute },
	{ _SS(CMD_UNIT), &UnitCmd_Execute }
};

const size_t TextureUnitStateListener::commandCount = sizeof(TextureUnitStateListener::commands) 
/ sizeof(TextureUnitStateListener::commands[0]);

CommandNamePair SamplerListener::commands[] = {
	{ _SS(CMD_ADDRESS_U), &AddressUCmd_Execute },
	{ _SS(CMD_ADDRESS_V), &AddressVCmd_Execute },
	{ _SS(CMD_ADDRESS_W), &AddressWCmd_Execute },
	{ _SS(CMD_ANISOTROPY), &AnisotropyCmd_Execute },
	{ _SS(CMD_BORDER_COLOR), &BorderColorCmd_Execute },
	{ _SS(CMD_COMPARE), &CompareCmd_Execute },
	{ _SS(CMD_LOD_BIAS), &LodBiasCmd_Execute },
	{ _SS(CMD_MAG_FILTER), &MagFilterCmd_Execute },
	{ _SS(CMD_MAX_LOD), &MaxLodCmd_Execute },
	{ _SS(CMD_MIN_FILTER), &MinFilterCmd_Execute },
	{ _SS(CMD_MIN_LOD), &MinLodCmd_Execute },
};

const size_t SamplerListener::commandCount = sizeof(SamplerListener::commands) 
/ sizeof(SamplerListener::commands[0]);

void TextureUnitStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void TextureUnitStateListener::EnterStatement(
		ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(TextureUnitStateListener::commands,
			TextureUnitStateListener::commandCount, ctx.GetCommand());

	if (cmd) {
		cmd(CommandDelegateBlock::TEXTURE_STATE_BLOCK, shader,
			ctx);
	}
}

void TextureUnitStateListener::SamplerCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::TEXTURE_STATE_BLOCK);
	SamplerListener sampler;
	ShaderTemplate::LoadStreamRequest* shader =
		reinterpret_cast<ShaderTemplate::LoadStreamRequest*>(parentParam);

	const StringUtils::WordList& paramContext = ctx.GetParamList();
	ConstMultiStringHelper helper(paramContext);
	String name = helper.Get(0);

	ctx.ParseBlock(&sampler);
	if (!ctx.IsErrorBitSet()) {
		shader->AddSampler(name, sampler.params);
	}
}

void TextureUnitStateListener::UnitCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::TEXTURE_STATE_BLOCK);
	ShaderTemplate::LoadStreamRequest* shader =
		(static_cast<ShaderTemplate::LoadStreamRequest*>(parentParam));

	String samplerName;
	String unitName;

	const StringUtils::WordList& paramContext = ctx.GetParamList();
	ConstMultiStringHelper::Iterator it = ConstMultiStringHelper::It(paramContext);
	if(!it.HasNext(samplerName)) {
		Error("Missing sampler name!");
		return;
	}
	if(!it.HasNext(unitName)) {
		Error("Missing unit name!");
		return;
	}
	StringPair nameSemantic = StringUtils::Split(unitName, ':');
	AutoParamName apn = AutoParamName::AUTO_INVALID_PARAM;
	if (nameSemantic.second != StringUtils::Null)
		apn = Helper::GetAutoParam(nameSemantic.second);

	unitName = std::move(nameSemantic.first);

	if (apn != AutoParamName::AUTO_INVALID_PARAM)
		shader->AddSemanticBinding(unitName,
			Helper::GetAutoParam(nameSemantic.second)
		);
	else {
		String uiName = ctx.GetTaggedParamVal(_SS(TAG_UI), it);
		String desc = ctx.GetTaggedParamVal(_SS(TAG_DESC), it);
		shader->AddParam(unitName, uiName, desc, ParamDataType::PDT_TEXTURE);
	}

	shader->AddTextureUnit(unitName, samplerName);
}


void SamplerListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void SamplerListener::EnterStatement(
	ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(SamplerListener::commands,
		SamplerListener::commandCount, ctx.GetCommand());

	if (cmd) {
		cmd(CommandDelegateBlock::TEXTURE_STATE_BLOCK, &params,
			ctx);
	}
}

void SamplerListener::AddressUCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));

	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.uAddress = Helper::GetTextureAddressMode(value);
	}
}

void SamplerListener::AddressVCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.vAddress = Helper::GetTextureAddressMode(value);
	}
}

void SamplerListener::AddressWCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.wAddress = Helper::GetTextureAddressMode(value);
	}
}

void SamplerListener::AnisotropyCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.maxAnisotropy = (uint32)Convert::ToULong(value);
	}
}

void SamplerListener::BorderColorCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.borderColor = Convert::ToColor(value);
	}
}

void SamplerListener::LodBiasCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.lodBias = Convert::ToFloat(value);
	}
}

void SamplerListener::MagFilterCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.magFilter = Helper::GetMagFilter(value);
	}
}

void SamplerListener::MaxLodCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.maxLod = Convert::ToFloat(value);
	}
}

void SamplerListener::MinFilterCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.minFilter = Helper::GetMinFilter(value);
	}
}

void SamplerListener::MinLodCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.minLod = Convert::ToFloat(value);
	}
}

void SamplerListener::CompareCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& statement) {

	TextureUnitParams& texUnitParams = *(static_cast<TextureUnitParams*>(parentParam));
	String value = ConstMultiStringHelper(statement.GetParamList()).Get(0);
	if (value.length()) {
		texUnitParams.comparisonFunc = Helper::GetTextureCompareFunc(value);
	}
}

} /* namespace ShaderCompiler */
