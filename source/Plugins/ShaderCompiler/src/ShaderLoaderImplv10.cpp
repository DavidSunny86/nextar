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
#include <ShaderAsset.h>
#include <RenderManager.h>
#include <ShaderScript.h>

namespace ShaderCompiler {

/**************************************************************
 * ShaderLoaderImplv1_0
 **************************************************************/
ShaderLoaderImplv1_0::ShaderLoaderImplv1_0() {
}

ShaderLoaderImplv1_0::~ShaderLoaderImplv1_0() {
}

void ShaderLoaderImplv1_0::Configure(const Config&) {
	switch (RenderManager::Instance().GetProgramLanguage()) {
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
	ShaderScript s(std::cref(languageContext),
			static_cast<ShaderAsset::StreamRequest*>(shader.GetRequestPtr()));
	ShaderAsset* shaderPtr =
			static_cast<ShaderAsset*>(shader.GetRequestPtr()->streamedObject);
	String scriptName = shaderPtr->GetAssetLocator().ToString();
	scriptParser.ParseScript(&s, scriptName, input);
}

} /* namespace nextar */
