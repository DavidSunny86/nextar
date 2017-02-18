/*
 * CmdTextureState.cpp
 *
 *  Created on: 06-Mar-2016
 *      Author: obhi
 */

#include "CmdTextureState.h"
#include <ShaderScriptContext.h>

namespace ShaderScript {


bool ShaderScript::CmdTextureState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	return true;
}

void ShaderScript::CmdTextureState::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
}

bool ShaderScript::CmdUnit::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String samplerName;
	String unitName;
	if(!it.HasNext(unitName)) {
		Error("Missing unit name!");
		return false;
	}
	if(!it.HasNext(samplerName)) {
		Error("Missing sampler name!");
		return false;
	}
	StringPair nameSemantic = StringUtils::Split(unitName, ':');
	AutoParamName apn = AutoParamName::AUTO_INVALID_PARAM;
	if (nameSemantic.second != StringUtils::Null)
		apn = Helper::GetAutoParam(nameSemantic.second);
	unitName = std::move(nameSemantic.first);

	ParameterContext context = ParameterContext::CTX_UNKNOWN;
	if (apn != AutoParamName::AUTO_INVALID_PARAM)
		c->shader->AddSemanticBinding(unitName, apn);
	else {
		ShaderTemplate::ParameterDesc desc;
				
		desc.activateOption = it.GetTaggedVal("activate");
		desc.description = it.GetTaggedVal("desc");
		desc.type = ParamDataType::PDT_TEXTURE;
		String contextName = it.GetTaggedVal("context");
		StringUtils::ToLower(contextName);
		context = desc.context = Helper::GetContextFromName(contextName);
		
		c->shader->AddParam(unitName, desc);
	}
	ShaderTemplate::TextureUnitDesc desc;
	desc.context = context;
	desc.samplerName = std::move(samplerName);
	c->shader->AddTextureUnit(unitName, desc);
	return true;
}

bool ShaderScript::CmdSampler::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	if (it.HasNext(value)) {
		c->samplerName = value;
		TextureUnitParams params;

		if(it.HasNext(value)) {
			if (value != "advanced") {
				StringUtils::StringPair sp = StringUtils::Split(value, '-');
				if (sp.first == "linear") {
					params.minFilter = TextureMinFilter::TF_MIN_LINEAR_MIPMAP_LINEAR;
					params.magFilter = TextureMagFilter::TF_MAG_LINEAR;
				} else if (sp.first == "nearest") {
					params.minFilter = TextureMinFilter::TF_MIN_NEAREST_MIPMAP_NEAREST;
					params.magFilter = TextureMagFilter::TF_MAG_NEAREST;
				}
				
				params.uAddress = 
				params.vAddress = 
				params.wAddress = Helper::GetTextureAddressMode(sp.second);
			}
		}
		c->textureState = params;
	} else {
		c->samplerName = StringUtils::Null;
		return false;
	}


	return true;
}

void ShaderScript::CmdSampler::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	if (c->samplerName.length() > 0)
		c->shader->AddSampler(c->samplerName, c->textureState);
}

bool ShaderScript::CmdAddress::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.uAddress = Helper::GetTextureAddressMode(value);
	}
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.vAddress = Helper::GetTextureAddressMode(value);
	}
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.wAddress = Helper::GetTextureAddressMode(value);
	}
	return true;
}

bool ShaderScript::CmdAnisotropy::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	if (it.HasNext(value)) {
		c->textureState.maxAnisotropy = (uint32)Convert::ToULong(value);
	}
	return true;
}

bool ShaderScript::CmdTextureLod::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.minLod = Convert::ToFloat(value);
	}
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.maxLod = Convert::ToFloat(value);
	}
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.lodBias = Convert::ToFloat(value);
	}

	return true;
}

bool ShaderScript::CmdCompare::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.comparisonFunc = Helper::GetTextureCompareFunc(value);
	}

	return true;
}

bool ShaderScript::CmdFilter::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);

	String value;
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.minFilter = Helper::GetMinFilter(value);
	}
	if (it.HasNext(value) && !Helper::IsDefault(value)) {
		c->textureState.magFilter = Helper::GetMagFilter(value);
	}
	return true;
}

} /* namespace ShaderScript */
