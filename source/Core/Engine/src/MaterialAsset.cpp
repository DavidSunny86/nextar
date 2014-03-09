/*
 * Material.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include "NexEngine.h"
#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "ShaderAsset.h"
#include "ComponentGroupArchive.h"

namespace nextar {
	/****************************************
	 * Material::Factory
	 ****************************************/
	MaterialAsset::Factory::Factory(const StringID name) : Asset::Factory(name) {
	}

	Component* MaterialAsset::Factory::AsyncCreate(uint32 type, const StringID name) {
		if (type == MaterialAsset::CLASS_ID)
			return NEX_NEW MaterialAsset(name);
		return 0;
	}

	/****************************************
	 * Material
	 ****************************************/
	MaterialAsset::MaterialAsset(const StringID name) : Asset(name), layerMask(0),
			parameterDef(nullptr), renderLayer(0) {
	}

	MaterialAsset::~MaterialAsset() {
		_RelievePropertyBuffer();
	}

	MaterialAssetPtr MaterialAsset::Instance(MaterialAsset::Factory* factory, const StringID name, const URL& location) {
		MaterialAssetPtr material = Assign(static_cast<MaterialAsset*>(factory->AsyncCreate(MaterialAsset::CLASS_ID, name)));
		if(material)
			material->SetAssetLocator(location);
		return material;
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
		_VerifyParameterBuffer();
		memoryCost += parameters.size();
		parameterDef = &shader->GetParameterList();
	}

	void MaterialAsset::_VerifyParameterBuffer() {
		if(parameters.size() != shader->GetParameterBufferSize())
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	void MaterialAsset::_PreparePropertyBuffer() {
	}

	void MaterialAsset::LoadImpl(nextar::StreamRequest* request, bool) {
		Loader loader(request);
		loader.Serialize();
	}

	void MaterialAsset::UnloadImpl(nextar::StreamRequest* req, bool isStreamed) {
		// todo
	}

	nextar::StreamRequest* MaterialAsset::CreateStreamRequestImpl(bool load) {
		return NEX_NEW MaterialAsset::StreamRequest(this);
	}

	void MaterialAsset::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		MaterialAsset::StreamRequest* req = static_cast<MaterialAsset::StreamRequest*>(request);
		NEX_DELETE req;
		request = nullptr;
	}

	void MaterialAsset::SetParamBufferSize(uint32 size) {
		parameters.resize(size);
	}

	void MaterialAsset::SetParamDataByOffset(uint32 offset, const void* data, size_t amt) {
		NEX_ASSERT(offset+amt <= parameters.size());
		std::memcpy((parameters.data() + offset), data, amt);
	}

	void MaterialAsset::SetParamTextureByOffset(uint32 offset, const TextureUnit* data, size_t count) {
		SetParamDataByOffset(offset, data, count*sizeof(TextureUnit));
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

		Asset* shader = static_cast<Asset*>(groupPtr->AsyncFind(name));

		if (shader == nullptr) {
			Component::Factory* factoryPtr =
					ComponentFactoryArchive::Instance().AsyncFindFactory(
							ShaderAsset::CLASS_ID, factory);
			shader = ShaderAsset::Instance(factoryPtr, name, location, groupPtr);
		}

		if(!shader) {
			Warn("Failed to load shader for material: " + Convert::ToString(shader->GetID()));
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		GetMetaInfo().AddDependency(shader);
	}

	void MaterialAsset::StreamRequest::SetParamBufferSize(uint32 paramBufferSize) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->SetParamBufferSize(paramBufferSize);
	}

	void MaterialAsset::StreamRequest::SetParamValue(uint32 offset, const void* data, size_t amount) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->SetParamDataByOffset(offset, data, amount);
	}

	void MaterialAsset::StreamRequest::SetTextureValue(uint32 offset, const TextureUnit* tu, uint32 count) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->SetParamTextureByOffset(offset, tu, count);
		for(uint16 i = 0; i < count; ++i) {
			TextureBase* tb = tu[i].texture;
			if (tb && tb->IsTextureAsset()) {
				GetMetaInfo().AddDependency(static_cast<TextureAsset*>(tb));
			}
		}
	}

	void MaterialAsset::StreamRequest::SetRenderLayer(uint8 l) {
		MaterialAsset* material = static_cast<MaterialAsset*>(GetStreamedObject());
		material->SetRenderLayer(l);
	}

	/*****************************************************/
	/* Material::Loader       							 */
	/*****************************************************/
	NEX_IMPLEMENT_FACTORY(MaterialAsset::Loader);
	MaterialAsset::Loader::Loader(nextar::StreamRequest* material) : requestPtr(material) {
	}

	MaterialAsset::Loader::~Loader() {
	}

	void MaterialAsset::Loader::Serialize() {
		MaterialAsset* materialPtr = static_cast<MaterialAsset*>(requestPtr->GetStreamedObject());
		const URL& location = materialPtr->GetAssetLocator();
		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		MaterialAsset::LoaderImpl* impl = GetImpl(ext);
		if (!impl) {
			Error("No material compiler registered.");
			NEX_THROW_GracefulError(EXCEPT_MISSING_PLUGIN);
		}

		InputStreamPtr input = FileSystem::Instance().OpenRead(location);

		if (input) {
			impl->Load(input, *this);
		} else {
			Error(
					String("Could not open material file: ")
							+ materialPtr->GetAssetLocator().ToString());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOCATE_ASSET);
		}
	}

} /* namespace nextar */
