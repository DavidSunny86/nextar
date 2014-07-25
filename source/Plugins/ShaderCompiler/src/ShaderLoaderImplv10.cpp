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
}

void ShaderLoaderImplv1_0::Load(InputStreamPtr& input, AssetLoader& shader) {
	ScriptParser scriptParser;
	ShaderScript s(
			static_cast<ShaderAsset::StreamRequest*>(shader.GetRequestPtr()));
	ShaderAsset* shaderPtr =
			static_cast<ShaderAsset*>(shader.GetRequestPtr()->streamedObject);
	String scriptName = shaderPtr->GetAssetLocator().ToString();
	scriptParser.ParseScript(&s, scriptName, input);
}

} /* namespace nextar */
