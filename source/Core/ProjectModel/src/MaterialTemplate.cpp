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

bool MaterialTemplate::NotifyAssetLoadedImpl() {
	if(!shader) {
		Error("Shader failed to load");
		// request is complete
		return true;
	}
	ShaderAssetPtr shaderPtr =
			shader->GetShaderUnit(compilationOptions);
	material = MaterialAsset::Traits::Instance(assetId);
	if (!material->IsLoaded()) {
		auto streamRequest = static_cast<AssetStreamRequest*>(
			material->GetStreamRequest(true));
		MaterialFromTemplate loader(this, shaderPtr);
		streamRequest->SetManualLoader(&loader);
		material->Load(false);
	}
	return true;
}

bool MaterialTemplate::NotifyAssetSavedImpl() {
	if (material)
		material->SetAssetLocator(GetAssetLocator());
	return true;
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

void MaterialTemplate::SetCompilationOptions(
		const StringUtils::WordList& options) {
	compilationOptions = options;
}

void MaterialTemplate::AddParam(const String& name, const String& value) {
	paramValues[name] = value;
}

void MaterialTemplate::UnloadImpl() {
}

StreamRequest* MaterialTemplate::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(StreamRequest(this));
}

void MaterialTemplate::SetMaterialID(const MaterialAsset::ID& id) {
	assetId = id;
}

void MaterialTemplate::DestroyStreamRequestImpl(nextar::StreamRequest*& streamRequest,
		bool load) {
	if (streamRequest) {
		MaterialTemplate::StreamRequest* req = 
			static_cast<MaterialTemplate::StreamRequest*>(streamRequest);
		NEX_DELETE(req);
	}
	streamRequest = nullptr;
}

uint32 MaterialTemplate::GetClassID() const {
	return CLASS_ID;
}

uint32 MaterialTemplate::GetProxyID() const {
	return MaterialAsset::CLASS_ID;
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
	MaterialAsset::StreamRequest* request = static_cast<MaterialAsset::StreamRequest*>(
			loader.GetRequestPtr());

	auto paramValues = parent->paramValues;
	request->SetShader(shader);
	request->SetLayer(parent->GetLayer());
	auto paramContext = shader->GetParamTableItem(ParameterContext::CTX_MATERIAL);
	uint32 size = paramContext.totalParamBufferSize;
	for(auto it = paramContext.beginIt; it != paramContext.endIt; ++it) {
		NEX_ASSERT( (*it).autoName == AutoParamName::AUTO_CUSTOM_CONSTANT );
	}
	request->SetParamBufferSize(size);
	size_t offset = 0;
	for(auto it = paramContext.beginIt; it != paramContext.endIt; ++it) {
		auto pv = paramValues.find((*it).name);
		if (pv != paramValues.end()) {
			auto agent = ShaderParamAgent::GetAgent((*it).type);
			if (agent)
				agent->SetParamValue(offset, request, (*it), (*pv).second);
		}
		offset += (*it).maxSize;
	}
	/* Find relevant params and set
	 * */
}

} /* namespace nextar */
