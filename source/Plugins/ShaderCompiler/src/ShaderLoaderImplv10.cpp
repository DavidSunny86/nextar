/*
 * ShaderLoaderImplv10.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <NexBase.h>
#include <ShaderAsset.h>
#include <CommonTypes.h>
#include <ShaderListener.h>
#include <ShaderLoaderImplv10.h>
#include <ScriptParser.h>
#include <ScriptStrings.h>

namespace ShaderCompiler {

	/**************************************************************
	 * ShaderLoaderImplv1_0
	 **************************************************************/
	ShaderLoaderImplv1_0::ShaderLoaderImplv1_0() {
	}

	ShaderLoaderImplv1_0::~ShaderLoaderImplv1_0() {
	}

	void ShaderLoaderImplv1_0::Configure(const Config&) {
		switch(RenderManager::Instance().GetProgramLanguage()) {
		case RenderManager::SPP_GLSL:
			languageContext = _SS(LANG_GLSL);
			break;
		case RenderManager::SPP_HLSL:
			languageContext = _SS(LANG_HLSL);
			break;
		}
	}

	void ShaderLoaderImplv1_0::Load(InputStreamPtr& input, AssetLoader& shader) {
		ScriptParser scriptParser;
		ShaderLoaderImplv1_0::Script s(shader.GetRequestPtr());
		ShaderAsset* shaderPtr = static_cast<ShaderAsset*>(shader.GetRequestPtr()->streamedObject);
		scriptParser.ParseScript(&s, shaderPtr->GetName(), input);
	}

	void ShaderLoaderImplv1_0::Script::EnterScript(ScriptParser::ScriptContext& block) {
		block.ParseRegions(this);
	}

	void ShaderLoaderImplv1_0::Script::EnterRegion(ScriptParser::RegionContext& ctx) {
		const String& name = ctx.GetName();
		String::size_type pos;
		if (name == _SS(SHADER_REGION)) {
			ctx.ParseStatements(this);
		} else if ((pos=name.find(languageContext)) != String::npos) {

		}
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
