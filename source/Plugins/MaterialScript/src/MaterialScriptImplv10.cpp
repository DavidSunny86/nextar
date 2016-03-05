/*
 * MaterialScriptImplv10.cpp
 *
 *  Created on: 03-Mar-2016
 *      Author: obhi
 */

#include <MaterialScriptImplv10.h>
#include <CmdMaterial.h>
#include <CmdLayer.h>
#include <CmdSetParam.h>
#include <CmdEnableOption.h>
#include <CmdShader.h>
#include <MaterialScriptContext.h>

namespace MaterialScript {

MaterialScriptImplv1_0::MaterialScriptImplv1_0() {
}

MaterialScriptImplv1_0::~MaterialScriptImplv1_0() {
}

void MaterialScriptImplv1_0::UnregisterDictionary() {
	CommandDictionaryArchive::Instance().UnregisterDictionary("MaterialScript");
}

void MaterialScriptImplv1_0::RegisterDictionary() {
	CommandDictionary* dict = CommandDictionaryArchive::Instance().RegisterDictionary("MaterialScript",
			&_root);
	dict->RegisterHandler("material", CmdMaterial::InstancePtr());
	dict->RegisterHandler("material.layer", CmdLayer::InstancePtr());
	dict->RegisterHandler("material.set-param", CmdSetParam::InstancePtr());
	dict->RegisterHandler("material.enable-option", CmdEnableOption::InstancePtr());
	dict->RegisterHandler("material.shader", CmdShader::InstancePtr());
}

void MaterialScriptImplv1_0::Configure(const nextar::Config&) {
}

void MaterialScriptImplv1_0::Load(nextar::InputStreamPtr& input,
		nextar::AssetLoader& material) {
	auto request = static_cast<MaterialTemplate::StreamRequest*>(material.GetRequestPtr());
	auto mtl = static_cast<MaterialTemplate*>(request->GetStreamedObject());
	MaterialTemplate::ID materialId;
	mtl->GetID(materialId);

	MaterialScriptContext context(request, materialId);
	NeoCommandInterpreter::Execute("MaterialScript", input, &context);
}

} /* namespace MaterialScript */
