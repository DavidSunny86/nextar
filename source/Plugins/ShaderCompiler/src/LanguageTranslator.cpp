/*
 * LanguageTranslator.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#include <LanguageTranslator.h>
#include <ScriptStrings.h>
#include <TranslatorGLSL.h>
#include <TranslatorHLSL.h>

namespace ShaderCompiler {

LanguageTranslator::LanguageTranslator() {
	translators[GLSL_TRANSLATOR] = NEX_NEW(TranslatorGLSL());
	translators[HLSL_TRANSLATOR] = NEX_NEW(TranslatorHLSL());
}

LanguageTranslator::~LanguageTranslator() {
	for(auto& t : translators) {
		NEX_DELETE(t);
	}
}

void LanguageTranslator::AddMacro(ShaderScript* script, const String& name) {
	for (auto& t : translators) {
		t->AddMacro(script, name);
	}
}

void LanguageTranslator::AddPredefs(Pass::ProgramStage stage, ShaderScript* script) {
	for (auto& t : translators) {
		t->AddPredefs(stage, script);
	}
}

void LanguageTranslator::BeginBuffer(const String& name) {
	for(auto& t : translators) {
		t->BeginBuffer(name);
	}
}

void LanguageTranslator::AddParam(ParamDataType dataType, const String& name,
		uint32 arrayCount) {
	for(auto& t : translators) {
		t->AddParam(dataType, name, arrayCount);
	}
}

void LanguageTranslator::EndBuffer(ShaderScript* script) {
	for(auto& t : translators) {
		t->EndBuffer(script);
	}
}

void LanguageTranslator::TranslateConstantBuffer(ShaderScript* script,
		const String& name, nextar::InputStreamPtr stream) {
	ScriptParser scriptParser;
	ConstBufferListener listener(this, script);
	scriptParser.ParseScript(&listener, name, stream);
}

void LanguageTranslator::TranslateMacro(ShaderScript* script, const String& name) {
	AddMacro(script, name);	
}

/**************************************
 * ConstBufferListener
 */
void LanguageTranslator::ConstBufferListener::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void LanguageTranslator::ConstBufferListener::EnterRegion(ScriptParser::RegionContext& ctx) {
	const String& name = ctx.GetName();
	ctx.ParseStatements(this);
}

void LanguageTranslator::ConstBufferListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_CBUFFER)) {
		String name;
		bool determineContext = true;
		const StringUtils::WordList& words = ctx.GetParamList();
		StringUtils::TokenIterator it = 0;
		it = StringUtils::NextWord(words, name, it);
		StringPair semantic = StringUtils::Split(name, ':');
		if (semantic.second != StringUtils::Null) {
			AutoParamName apn = Helper::GetAutoParam(semantic.second);
			if (apn != AutoParamName::AUTO_INVALID_PARAM) {
				script->GetRequest()->AddSemanticBinding(semantic.first, apn);
				determineContext = false;
				name = std::move(semantic.first);
			}
		}

		if (determineContext) {
			String context;
			String contextStr = _SS(CMD_CONTEXT);
			while ((it = StringUtils::NextWord(words, context, it)) != String::npos) {
				if (!context.compare(0, contextStr.length(), contextStr)) {
					StringUtils::StringPair p = StringUtils::Split(context);
					name += "__";
					name += p.second;
					break;
				}

			}

		}

		ConstBufferCommandListener sl(script, translator, !determineContext);
		translator->BeginBuffer(name);
		ctx.ParseBlock(&sl);
		translator->EndBuffer(script);
	}
}

void LanguageTranslator::ConstBufferCommandListener::EnterBlock(
		ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void LanguageTranslator::ConstBufferCommandListener::EnterStatement(
		ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_VAR)) {
		StringUtils::TokenIterator it = 0;

		ParamDataType dataType;
		String value;

		const StringUtils::WordList& paramContext = ctx.GetParamList();
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			dataType = ShaderParameter::MapParamType(value);
		}
		if (dataType == ParamDataType::PDT_UNKNOWN)
			ctx.Error(value + " is unknown data type.");

		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			StringPair nameSemantic = StringUtils::Split(value, ':');
			
			
			AutoParamName apn = AutoParamName::AUTO_INVALID_PARAM;
			if (nameSemantic.second.length()>0)
				apn = Helper::GetAutoParam(nameSemantic.second);

			if (apn != AutoParamName::AUTO_INVALID_PARAM)
				script->GetRequest()->AddSemanticBinding(nameSemantic.first,
								apn
								);

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
					ctx.Error("Variable name is not in good format: " + nameSemantic.first);
					return;
				}
				name = nameSemantic.first.substr(0, pos);
				StringUtils::Trim(name);
			} else {

				name = std::move(nameSemantic.first);
				std::replace(name.begin(), name.end(), '{', '[');
				std::replace(name.begin(), name.end(), '}', ']');
			}

			translator->AddParam(dataType, name, arrayCount);
			if (apn == AutoParamName::AUTO_INVALID_PARAM && !cbIsAutoParam) {
				String uiName = ctx.GetTaggedParamVal(_SS(TAG_UI));
				String desc = ctx.GetTaggedParamVal(_SS(TAG_DESC));
				script->GetRequest()->AddParam(name, uiName, desc, dataType);
			}
			
		}
	}
}

} /* namespace ShaderCompiler */
