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
#include <ShaderCompilerImplv10.h>
#include <ScriptParser.h>
#include <ScriptStrings.h>
#include <ShaderAsset.h>
#include <RenderManager.h>
#include <ShaderScript.h>

namespace ShaderCompiler {

/**************************************************************
 * ShaderLoaderImplv1_0
 **************************************************************/
ShaderCompilerImplv1_0::ShaderCompilerImplv1_0() {
}

ShaderCompilerImplv1_0::~ShaderCompilerImplv1_0() {
}

void ShaderCompilerImplv1_0::Configure(const Config&) {
}

void ShaderCompilerImplv1_0::Load(InputStreamPtr& input, AssetLoader& shader) {
	ScriptParser scriptParser;
	auto request = static_cast<ShaderTemplate::LoadStreamRequest*>(shader.GetRequestPtr());
	ShaderScript s(request);
	String scriptName = request->GetName();
	bool completeStatus = scriptParser.ParseScript(&s, scriptName, input);
	request->SetCompleted(completeStatus);
}

} /* namespace nextar */
