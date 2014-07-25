/*
 * MaterialLoaderImplv10.cpp
 *
 *  Created on: 18-Jul-2014
 *      Author: obhi
 */
#define NEX_MATERIAL_STRINGS_DEFINE
#include <CommonTypes.h>
#include <MaterialLoaderImplv10.h>
#include <ScriptStrings.h>

namespace MaterialLoader {

MaterialLoaderImplv1_0::MaterialLoaderImplv1_0() {

}

MaterialLoaderImplv1_0::~MaterialLoaderImplv1_0() {

}

void MaterialLoaderImplv1_0::Configure(const Config&) {
}

void MaterialLoaderImplv1_0::Load(InputStreamPtr& input, AssetLoader& shader) {
	ScriptParser scriptParser;
	MaterialScript s(static_cast<MaterialAsset::StreamRequest*>(shader.GetRequestPtr()));
	MaterialAsset* materialPtr =
			static_cast<MaterialAsset*>(shader.GetRequestPtr()->streamedObject);
	String scriptName = materialPtr->GetAssetLocator().ToString();
	scriptParser.ParseScript(&s, scriptName, input);
}

} /* namespace MaterialLoader */
