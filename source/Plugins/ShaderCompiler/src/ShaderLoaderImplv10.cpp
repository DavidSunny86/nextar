/*
 * ShaderLoaderImplv10.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <NexBase.h>
#include <Shader.h>
#include <CommonTypes.h>
#include <ShaderListener.h>
#include <ShaderLoaderImplv10.h>
#include <ScriptParser.h>

namespace ShaderCompiler {

	/**************************************************************
	 * ShaderLoaderImplv1_0
	 **************************************************************/
	ShaderLoaderImplv1_0::ShaderLoaderImplv1_0() {
	}

	ShaderLoaderImplv1_0::~ShaderLoaderImplv1_0() {
	}

	void ShaderLoaderImplv1_0::Load(InputStreamPtr& input, ShaderAsset::Loader& shader) {
		ScriptParser scriptParser;
		ShaderLoaderImplv1_0::Script s(shader.GetRequestPtr());
		ShaderAsset* shaderPtr = static_cast<ShaderAsset*>(shader.GetRequestPtr()->streamedObject);
		scriptParser.ParseScript(&s, shaderPtr->GetName(), input);
	}

	void ShaderLoaderImplv1_0::Script::EnterScript(ScriptParser::ScriptContext& block) {
		block.ParseStatements(this);
	}

	void ShaderLoaderImplv1_0::Script::EnterStatement(ScriptParser::StatementContext& ctx) {
		if (ctx.GetCommand() == "Shader") {
			String name;
			StringUtils::NextWord(ctx.GetParamList(), name);
			ShaderCompiler::ShaderListener sl(shader, name);
			ctx.ParseBlock(&sl);
		}
	}

} /* namespace nextar */
