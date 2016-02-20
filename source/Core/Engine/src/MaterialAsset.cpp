/*
 * Material.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <MaterialAsset.h>
#include <TextureAsset.h>
#include <ShaderAsset.h>
#include <ComponentGroupArchive.h>
#include <ComponentFactoryArchive.h>

namespace nextar {

/****************************************
 * Material
 ****************************************/
MaterialAsset::MaterialAsset(const StringID name, const StringID factory) :
		Asset(name, factory), layerMask(0) {
}

MaterialAsset::~MaterialAsset() {
	Debug("Material destroyed: " + GetAssetLocator().ToString());
}

uint32 MaterialAsset::GetClassID() const {
	return CLASS_ID;
}

StreamNotification MaterialAsset::NotifyAssetLoadedImpl(StreamRequest* request) {
	/* loaded only when dependencies are resolved */
	MaterialAsset::MaterialLoadRequest* req =
			static_cast<MaterialAsset::MaterialLoadRequest*>(request);
	PrepareMaterial(req);
	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
}

StreamNotification MaterialAsset::NotifyAssetSavedImpl(StreamRequest* request) {
	/* loaded only when dependencies are resolved */
	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
}

void MaterialAsset::NotifyAssetUnloaded() {
	// todo
	Asset::NotifyAssetUnloaded();
}

void MaterialAsset::NotifyAssetUpdated() {
	// todo
	Asset::NotifyAssetUpdated();
}

void MaterialAsset::PrepareMaterial(MaterialAsset::MaterialLoadRequest* req) {
	const ParamEntryTableItem& item = shader->GetParamTableItem(ParameterContext::CTX_MATERIAL);
	materialParamData.SetParamEntryTable(item);
}

void MaterialAsset::UnloadImpl() {
	// todo
}

nextar::StreamRequest* MaterialAsset::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(MaterialAsset::MaterialLoadRequest(this));
}

ParameterBuffer* MaterialAsset::PrepareParamBuffer(const ParamEntryTableItem& table) {
	materialParamData.Prepare(table);
	return &materialParamData;
}

void MaterialAsset::SetParamData(const void* data, uint32 offset,
		uint32 size) {
	materialParamData.SetData(data, offset, size);
}

void MaterialAsset::SetParamData(const TextureUnit* data, uint32 offset) {
	materialParamData.SetData(data, offset);
}

void MaterialAsset::SetParameterBuffer(ParameterBuffer&& buff) {
	materialParamData = std::move(buff);
}

void MaterialAsset::SetShader(ShaderAssetPtr& shader) {
	this->shader = shader;
}

/*****************************************************/
/* Material::StreamRequest							 */
/*****************************************************/
MaterialAsset::MaterialLoadRequest::MaterialLoadRequest(Asset *asset) :
		AssetStreamRequest(asset) {
}

MaterialAsset::MaterialLoadRequest::~MaterialLoadRequest() {
}

void MaterialAsset::MaterialLoadRequest::SetShader(ShaderAssetPtr& shader) {
	// construct the name, it will be of the form
	// Factory:Group.Name
	MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
	material->SetShader(shader);
	GetMetaInfo().AddDependency(shader);
}

void MaterialAsset::MaterialLoadRequest::SetShader(const ShaderAsset::ID& id,
		const URL& location) {
	ShaderAssetPtr shader = ShaderAsset::Traits::Instance(id, location);
	if (!shader) {
		Warn("Failed to load shader for material: "
						+ shader->GetNameID());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	} else
		SetShader(shader);
}

ParameterBuffer* MaterialAsset::MaterialLoadRequest::PrepareParamBuffer(const ParamEntryTableItem& table) {
	MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
	return material->PrepareParamBuffer(table);
}

void MaterialAsset::MaterialLoadRequest::SetParamValue(uint32 offset,
		const void* data, uint32 amount) {
	MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
	material->SetParamData(data, offset, amount);
}

void MaterialAsset::MaterialLoadRequest::SetTextureValue(uint32 offset,
		const TextureUnit* tu) {
	MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
	material->SetParamData(tu, offset);
	// add dependency
	if (tu->texture && tu->texture->IsTextureAsset()) {
		Asset* asset = static_cast<TextureAsset*>(tu->texture);
		GetMetaInfo().AddDependency(asset);
	}
}

void MaterialAsset::MaterialLoadRequest::SetParameterBuffer(ParameterBuffer&& buffer) {
	MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
	material->SetParameterBuffer(std::move(buffer));
}

void MaterialAsset::MaterialLoadRequest::SetLayer(Layer l) {
	MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
	material->SetLayerMask((uint8)l);
}

} /* namespace nextar */
