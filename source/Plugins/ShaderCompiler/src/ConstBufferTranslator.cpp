/*
 * ConstBufferTranslator.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#include <ConstBufferTranslator.h>
#include <ScriptStrings.h>
#include <ConstBufferGLSL.h>
#include <ConstBufferHLSL.h>

namespace ShaderCompiler {

ConstBufferTranslator::ConstBufferTranslator() {
	translators[GLSL_TRANSLATOR] = NEX_NEW(ConstBufferGLSL());
	translators[HLSL_TRANSLATOR] = NEX_NEW(ConstBufferHLSL());
}

ConstBufferTranslator::~ConstBufferTranslator() {
	for(auto& t : translators) {
		NEX_DELETE(t);
	}
}

void ConstBufferTranslator::BeginBuffer(const String& name) {
	for(auto& t : translators) {
		t->BeginBuffer(name);
	}
}

void ConstBufferTranslator::AddParam(ParamDataType dataType, const String& name,
		uint32 arrayCount) {
	for(auto& t : translators) {
		t->AddParam(dataType, name, arrayCount);
	}
}

void ConstBufferTranslator::EndBuffer(ShaderScript* script) {
	for(auto& t : translators) {
		t->EndBuffer(script);
	}
}

void ConstBufferTranslator::Translate(ShaderScript* script,
		const String& name, nextar::InputStreamPtr stream) {
	ScriptParser scriptParser;
	ConstBufferListener listener(this, script);
	scriptParser.ParseScript(&listener, name, stream);
}

/**************************************
 * ConstBufferListener
 */
void ConstBufferTranslator::ConstBufferListener::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void ConstBufferTranslator::ConstBufferListener::EnterRegion(ScriptParser::RegionContext& ctx) {
	const String& name = ctx.GetName();
	ctx.ParseStatements(this);
}

void ConstBufferTranslator::ConstBufferListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_CBUFFER)) {
		String name;
		StringUtils::NextWord(ctx.GetParamList(), name);
		StringPair semantic = StringUtils::Split(name, ':');
		if (semantic.second != StringUtils::Null) {
			script->GetRequest()->AddSemanticBinding(semantic.first,
					Helper::GetAutoParam(semantic.second)
					);
		}

		ConstBufferCommandListener sl(script, translator);
		translator->BeginBuffer(name);
		ctx.ParseBlock(&sl);
		translator->EndBuffer(script);
	}
}

void ConstBufferTranslator::ConstBufferCommandListener::EnterBlock(
		ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void ConstBufferTranslator::ConstBufferCommandListener::EnterStatement(
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
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			StringPair nameSemantic = StringUtils::Split(value, ':');
			if (nameSemantic.second != StringUtils::Null)
				script->GetRequest()->AddSemanticBinding(nameSemantic.first,
								Helper::GetAutoParam(nameSemantic.second)
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
		}
	}
}

} /* namespace ShaderCompiler */
