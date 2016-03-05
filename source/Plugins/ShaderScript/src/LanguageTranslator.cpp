/*
 * LanguageTranslator.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#include <LanguageTranslator.h>
#include <ScriptStrings.h>
#include <ShaderScriptContext.h>
#include <TranslatorGLSL.h>
#include <TranslatorHLSL.h>

namespace ShaderScript {

LanguageTranslator::LanguageTranslator() {
	translators[GLSL_TRANSLATOR] = TranslatorGLSL::InstancePtr();
	translators[HLSL_TRANSLATOR] = TranslatorHLSL::InstancePtr();
}

LanguageTranslator::~LanguageTranslator() {
}

void LanguageTranslator::AddMacro(ShaderScriptContext* script, const String& name) {
	for (auto& t : translators) {
		t->AddMacro(script, name);
	}
}

void LanguageTranslator::AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage) {
	for (auto& t : translators) {
		t->AddPredefs(script, stage);
	}
}

void LanguageTranslator::BeginBuffer(ShaderScriptContext* script, const String& name) {
	for(auto& t : translators) {
		t->BeginBuffer(script, name);
	}
}

void LanguageTranslator::AddParam(ShaderScriptContext* script,
		ParamDataType dataType, const String& name,
		uint32 arrayCount) {
	for(auto& t : translators) {
		t->AddParam(script, dataType, name, arrayCount);
	}
}

void LanguageTranslator::EndBuffer(ShaderScriptContext* script) {
	for(auto& t : translators) {
		t->EndBuffer(script);
	}
}

void LanguageTranslator::TranslateConstantBuffer(ShaderScriptContext* script,
		const String& name, nextar::InputStreamPtr stream) {
	NeoCommandInterpreter::Execute("ConstBufferScript", stream, script);
}

void LanguageTranslator::TranslateMacro(ShaderScriptContext* script, const String& name) {
	AddMacro(script, name);	
}


bool LanguageTranslator::CmdConstBuffer::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String name;

	bool determineContext = true;
	auto it = h.Iterate();
	if (it.HasNext(name)) {
		StringPair semantic = StringUtils::Split(name, ':');
		if (semantic.second != StringUtils::Null) {
			AutoParamName apn = Helper::GetAutoParam(semantic.second);
			if (apn != AutoParamName::AUTO_INVALID_PARAM) {
				c->shader->AddSemanticBinding(semantic.first, apn);
				determineContext = false;
				name = std::move(semantic.first);
			}
		}
	}

	if (determineContext) {
		String context;
		String contextStr = _SS(TAG_CONTEXT);
		while ((it.HasNext(context))) {
			if (!context.compare(0, contextStr.length(), contextStr)) {
				StringUtils::StringPair p = StringUtils::Split(context);
				name += "__";
				StringUtils::ToLower(p.second);
				name += ShaderParameter::GetContextKey(Helper::GetContextFromName(p.second));
				break;
			}
		}
	}

	c->cbIsAutoParam = !determineContext;
	LanguageTranslator::Instance().BeginBuffer(c, name);


	return true;
}

void LanguageTranslator::CmdConstBuffer::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	LanguageTranslator::Instance().EndBuffer(c);
}

bool LanguageTranslator::CmdVar::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String name;

	auto it = h.Iterate();

	ParamDataType dataType;
	String value;
	if (it.HasNext(value)) {
		dataType = ShaderParameter::MapParamType(value);
	}
	if (dataType == ParamDataType::PDT_UNKNOWN)
		Error(value + " is unknown data type.");
	if (it.HasNext(value)) {
		StringPair nameSemantic = StringUtils::Split(value, ':');
		AutoParamName apn = AutoParamName::AUTO_INVALID_PARAM;
		if (nameSemantic.second.length()>0)
			apn = Helper::GetAutoParam(nameSemantic.second);

		if (apn != AutoParamName::AUTO_INVALID_PARAM)
			c->shader->AddSemanticBinding(nameSemantic.first, apn);

		size_t pos;
		uint32 arrayCount = 1;
		String name;
		if ( (pos=nameSemantic.first.find_first_of('[')) != String::npos) {
			//'01234567
			//'name[10]'
			if (nameSemantic.first.back() == ']' && pos < nameSemantic.first.length()-1) {
				pos++;
				arrayCount =
						Convert::ToULong(nameSemantic.first.substr(
								pos, nameSemantic.first.length()-1-pos));
				if (arrayCount == 0)
					arrayCount = 1;
			} else {
				Error("Variable name is not in good format: " + nameSemantic.first);
				return false;
			}
			name = nameSemantic.first.substr(0, pos);
			StringUtils::Trim(name);
		} else {

			name = std::move(nameSemantic.first);
			std::replace(name.begin(), name.end(), '{', '[');
			std::replace(name.begin(), name.end(), '}', ']');
		}

		LanguageTranslator::Instance().AddParam(dataType, name, arrayCount);
		if (apn == AutoParamName::AUTO_INVALID_PARAM && !c->cbIsAutoParam) {
			String uiName;
			it.HasNext(uiName);
			String desc = StringUtils::GetTaggedVal(_SS(TAG_DESC), it);
			c->shader->AddParam(name, uiName, desc, dataType);
		}
	}
	return true;
}

} /* namespace ShaderCompiler */

