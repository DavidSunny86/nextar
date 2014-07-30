/*
 * MaterialLoaderImplv10.cpp
 *
 *  Created on: 18-Jul-2014
 *      Author: obhi
 */
#define NEX_MATERIAL_STRINGS_DEFINE
#include <CommonTypes.h>
#include <MaterialCompilerImplv10.h>
#include <ScriptStrings.h>
#include <MaterialTemplate.h>
#include <MaterialScript.h>

namespace MaterialCompiler {

MaterialCompilerImplv1_0::MaterialCompilerImplv1_0() {

}

MaterialCompilerImplv1_0::~MaterialCompilerImplv1_0() {

}

void MaterialCompilerImplv1_0::Configure(const Config&) {
}

void MaterialCompilerImplv1_0::Load(InputStreamPtr& input, AssetLoader& material) {
	ScriptParser scriptParser;
	auto request = static_cast<MaterialTemplate::StreamRequest*>(material.GetRequestPtr());
	MaterialScript s(request);
	String scriptName = request->GetName();
	scriptParser.ParseScript(&s, scriptName, input);
}

} /* namespace MaterialLoader */
