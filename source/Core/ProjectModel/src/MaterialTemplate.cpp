/*
 * MaterialTemplate.cpp
 *
 *  Created on: 25-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <MaterialTemplate.h>
#include <ShaderTemplate.h>
#include <ShaderParamAgent.h>

namespace nextar {

/*********************************
 * MaterialTemplate
 ********************************/
MaterialTemplate::MaterialTemplate(const StringID name, const StringID factory) : AssetTemplate(name, factory)
,layer(Layer::NORMAL) {
}

MaterialTemplate::~MaterialTemplate() {
}

StreamNotification MaterialTemplate::NotifyAssetLoadedImpl(nextar::StreamRequest* request) {
	if(!shader) {
		Error("Shader failed to load");
		// request is complete
		return StreamNotification::NOTIFY_COMPLETED;
	}

	//shader->AppendCompilerOptions(definedParams, enableOptions, compilationOptions);
	ShaderAssetPtr shaderPtr =
		shader->GetShaderUnit(definedParams, enableOptions);
	material = MaterialAsset::Traits::Instance(assetId);
	if (!material->AsyncIsLoaded()) {
		InputStreamPtr dummy;
		MaterialFromTemplate loader(this, shaderPtr);
		StreamInfo info(&loader, dummy);
		material->RequestLoad(info);
	}
	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
}

StreamNotification MaterialTemplate::NotifyAssetSavedImpl(nextar::StreamRequest* request) {
	if (material)
		material->SetAssetLocator(GetAssetLocator());
	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
}

void MaterialTemplate::NotifyAssetUnloaded() {
	AssetTemplate::NotifyAssetUnloaded();
}

void MaterialTemplate::NotifyAssetUpdated() {
	AssetTemplate::NotifyAssetUpdated();
}

void MaterialTemplate::SetShader(const ShaderTemplatePtr& shader) {
	this->shader = shader;
	paramValues.clear();
}

void MaterialTemplate::EnableOptions(
	const StringUtils::WordList& options) {
	StringUtils::PushBackWordList(enableOptions, options);
}

void MaterialTemplate::SetCompilationOptions(
		const StringUtils::WordList& options) {
	StringUtils::PushBackWordList(compilationOptions, options);
}

void MaterialTemplate::AddParam(const String& name, const String& value) {
	paramValues[name] = value;
	StringUtils::PushBackWord(definedParams, name);
}

void MaterialTemplate::UnloadImpl() {
}

StreamRequest* MaterialTemplate::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(StreamRequest(this));
}

void MaterialTemplate::SetMaterialID(const MaterialAsset::ID& id) {
	assetId = id;
}

uint32 MaterialTemplate::GetClassID() const {
	return CLASS_ID;
}

uint32 MaterialTemplate::GetProxyID() const {
	return MaterialAsset::CLASS_ID;
}

String MaterialTemplate::GetPoxyAssetExtension() const {
	return "nexmtl";
}

/********************************************
 * MaterialTemplate::StreamRequest
 *********************************************/
MaterialTemplate::StreamRequest::StreamRequest(
		MaterialTemplate* materialTemplate) : AssetStreamRequest(materialTemplate) {
}

void nextar::MaterialTemplate::StreamRequest::SetShader(
		SharedComponent::ID& shader, const URL& location) {
	MaterialTemplate* materialTemplate = static_cast<MaterialTemplate*>(
			GetStreamedObject());
	ShaderTemplatePtr shaderPtr = ShaderTemplate::Traits::Instance(shader, location);
	materialTemplate->SetShader(shaderPtr);
	GetMetaInfo().AddDependency(shaderPtr);
}

void nextar::MaterialTemplate::StreamRequest::EnableOptions(const StringUtils::WordList& options) {
	MaterialTemplate* materialTemplate = static_cast<MaterialTemplate*>(
		GetStreamedObject());
	materialTemplate->EnableOptions(options);
}

void nextar::MaterialTemplate::StreamRequest::SetCompilationOptions(
		const StringUtils::WordList& options) {
	MaterialTemplate* materialTemplate = static_cast<MaterialTemplate*>(
				GetStreamedObject());
	materialTemplate->SetCompilationOptions(options);
}

void MaterialTemplate::StreamRequest::SetMaterialID(
		const MaterialTemplate::ID& id) {
	MaterialTemplate* materialTemplate = static_cast<MaterialTemplate*>(
					GetStreamedObject());
	materialTemplate->SetMaterialID(id);
}

void nextar::MaterialTemplate::StreamRequest::AddParam(const String& name,
		const String& value) {
	MaterialTemplate* materialTemplate = static_cast<MaterialTemplate*>(
				GetStreamedObject());
	materialTemplate->AddParam(name, value);
}

void nextar::MaterialTemplate::StreamRequest::SetLayer(uint8 l) {
	MaterialTemplate* materialTemplate = static_cast<MaterialTemplate*>(
				GetStreamedObject());
	materialTemplate->SetLayer((Layer)l);
}

/********************************************
 * MaterialTemplate::MaterialFromTemplate
 *********************************************/
MaterialTemplate::MaterialFromTemplate::MaterialFromTemplate(MaterialTemplate* _parent,
		const ShaderAssetPtr& _shader) : parent(_parent),
				shader(_shader) {
}

void MaterialTemplate::MaterialFromTemplate::Load(InputStreamPtr& stream, AssetLoader& loader) {
	MaterialAsset::MaterialLoadRequest* request = static_cast<MaterialAsset::MaterialLoadRequest*>(
			loader.GetRequestPtr());

	auto paramValues = parent->paramValues;
	request->SetShader(shader);
	request->SetLayer(parent->GetLayer());
	auto paramContext = shader->GetParamTableItem(ParameterContext::CTX_MATERIAL);
	uint32 size = paramContext.totalParamBufferSize;
	for(auto it = paramContext.beginIt; it != paramContext.endIt; ++it) {
		NEX_ASSERT( (*it).autoName == AutoParamName::AUTO_CUSTOM_CONSTANT );
	}
	ParameterBuffer* params = request->PrepareParamBuffer(paramContext);
	size_t offset = 0;
	for(auto it = paramContext.beginIt; it != paramContext.endIt; ++it) {
		NEX_ASSERT((*it).name);
		auto pv = paramValues.find(*(*it).name);
		if (pv != paramValues.end()) {
			auto agent = ShaderParamAgent::GetAgent((*it).type);
			if (agent)
				agent->SetParamValue(offset, params, (*it), (*pv).second);
		}
		offset += (*it).maxSize;
	}
	// assets
	ParameterBuffer::AssetList& l = params->GetAssets();
	for (auto& e : l) {
		request->GetMetaInfo().AddDependency(e);
	}
	/* Find relevant params and set
	 * */
	request->SetCompleted(true);
}

} /* namespace nextar */
