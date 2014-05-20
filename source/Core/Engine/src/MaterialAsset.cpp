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
	MaterialAsset::MaterialAsset(const StringID name) : Asset(name), layerMask(0),
			renderLayer(0) {
	}

	MaterialAsset::~MaterialAsset() {
	}

	uint32 MaterialAsset::GetClassID() const {
		return CLASS_ID;
	}

	void MaterialAsset::NotifyAssetLoaded() {
		/* loaded only when dependencies are resolved */
		MaterialAsset::StreamRequest* req = static_cast<MaterialAsset::StreamRequest*>(GetStreamRequest());
		_PrepareMaterial(req);
		Asset::NotifyAssetLoaded();
	}

	void MaterialAsset::NotifyAssetUnloaded() {
		// todo
		Asset::NotifyAssetUnloaded();
	}

	void MaterialAsset::NotifyAssetUpdated() {
		// todo
		Asset::NotifyAssetUpdated();
	}

	void MaterialAsset::_PrepareMaterial(MaterialAsset::StreamRequest* req) {
		ConstParamEntryTableItem item = shader->GetParamEntryForMaterial();
		materialParamData.SetParamEntryTable(item);
	}

	void MaterialAsset::UnloadImpl(nextar::StreamRequest* req, bool isStreamed) {
		// todo
	}

	nextar::StreamRequest* MaterialAsset::CreateStreamRequestImpl(bool load) {
		return NEX_NEW(MaterialAsset::StreamRequest(this));
	}

	void MaterialAsset::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		MaterialAsset::StreamRequest* req = static_cast<MaterialAsset::StreamRequest*>(request);
		NEX_DELETE(req);
		request = nullptr;
	}
	
	void MaterialAsset::_SetParamBufferSize(size_t size) {
		materialParamData.Prepare(nullptr, size);
	}

	void MaterialAsset::_SetParamData(const void* data, size_t offset, size_t size) {
		materialParamData.SetData(data, offset, size);
	}

	/*****************************************************/
	/* Material::StreamRequest							 */
	/*****************************************************/
	MaterialAsset::StreamRequest::StreamRequest(Asset *asset) : AssetStreamRequest(asset) {
	}

	MaterialAsset::StreamRequest::~StreamRequest() {
	}

	void MaterialAsset::StreamRequest::SetShader(StringID name,
			const URL& location,
			StringID group,
			StringID factory) {
		// construct the name, it will be of the form
		// Factory:Group.Name
		Asset* material = static_cast<Asset*>(GetStreamedObject());
		SharedComponent::Group* groupPtr = ComponentGroupArchive::Instance().AsyncFindOrCreate(group);
		NEX_ASSERT(groupPtr);

		AssetPtr shader = groupPtr->AsyncFind(name);

		if (shader == nullptr) {
			Component::Factory* factoryPtr =
					ComponentFactoryArchive::Instance().AsyncFindFactory(
							ShaderAsset::CLASS_ID, factory);
			shader = ShaderAsset::Traits::Instance(name, location, factoryPtr, groupPtr);
		}

		if(!shader) {
			Warn("Failed to load shader for material: " + Convert::ToString(shader->GetID()));
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		GetMetaInfo().AddDependency(shader);
	}

	void MaterialAsset::StreamRequest::SetParamBufferSize(uint32 paramBufferSize) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->_SetParamBufferSize(paramBufferSize);
	}

	void MaterialAsset::StreamRequest::SetParamValue(uint32 offset, const void* data, size_t amount) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->_SetParamData(data, offset, amount);
	}

	void MaterialAsset::StreamRequest::SetTextureValue(uint32 offset, const TextureUnit* tu) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->_SetParamData(tu, offset, sizeof(TextureUnit));
		// add dependency
		if(tu->texture && tu->texture->IsTextureAsset()) {
			Asset* asset = static_cast<TextureAsset*>(tu->texture);
			GetMetaInfo().AddDependency(asset);
		}
	}

	void MaterialAsset::StreamRequest::SetRenderLayer(uint8 l) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->SetRenderLayer(l);
	}

} /* namespace nextar */
