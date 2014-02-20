/*
 * Material.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "ShaderAsset.h"

namespace nextar {

	/****************************************
	 * Property
	 ****************************************/
	class Property {

	private:
		/* No instantiation */
		Property();
		Property(const Property& );
		Property& operator = (const Property&);

	protected:

		uint16 type;
		uint16 offset;
		uint8 buffer[];

		void ConstructFromType();
		void DestructFromType();
		void SetValue(MaterialAsset*, const ShaderVariant&);

		friend class MaterialAsset;
	};
	
	/****************************************
	 * Material::Factory
	 ****************************************/
	Material::Factory::Factory(const String& name) : Asset::Factory(name) {
	}

	Component* Material::Factory::AsyncCreate(uint32 type, const String& name) {
		if (type == Material::CLASS_ID)
			return NEX_NEW MaterialAsset(name);
		return 0;
	}

	/****************************************
	 * Material
	 ****************************************/
	MaterialAsset::MaterialAsset(const String& name) : Asset(name) {
	}

	MaterialAsset::~MaterialAsset() {
		_RelievePropertyBuffer();
	}

	MaterialAsset* MaterialAsset::Instance(MaterialAsset::Factory* factory, const String& name, const URL& location) {
		MaterialAsset* material = factory->AsyncCreate(Material::CLASS_ID, name);
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

	void MaterialAsset::_RelievePropertyBuffer() {
		uint8* begin = &propertyBuffer[0];
		uint8* end = begin + propertyBuffer.size();
		while (begin < end) {
			Property& p = *(reinterpret_cast<Property*>(begin));
			p.DestructFromType();
			begin += p.offset;
		}
	}

	void MaterialAsset::_PrepareMaterial(MaterialAsset::StreamRequest* req) {
		_PreparePropertyBuffer();
		_PrepareParamData(req->params);
	}

	void MaterialAsset::_PrepareParamData(const ShaderParamMap& paramMap) {
		uint8* buffer = &propertyBuffer[0];
		/*** todo for Sun Feb 16: adjust for passes ***/
		const ShaderAsset::ShaderParamInfoList& l = shader->GetShaderParams();
		for(auto p : l) {
			Property& prop = *(reinterpret_cast<Property*>(buffer));
			prop.ConstructFromType();
			ShaderParamMap::const_iterator it = paramMap.find(p.name);
			if (it != paramMap.end()) {
				prop.SetValue(this, (*it).second);
			}
			buffer += prop.offset;
		}
	}

	void MaterialAsset::_PreparePropertyBuffer() {

		const ShaderAsset::ShaderParamInfoList& l = shader->GetShaderParams();
		size_t shaderParamBufferSize = shader->GetShaderParamBufferSize() + (l.size()*2*sizeof(uint16));
		/* clear existing buffer of pointer references */
		_RelievePropertyBuffer();
		if (shaderParamBufferSize != propertyBuffer.size()) {
			propertyBuffer.resize(shaderParamBufferSize);
		}

		//std::fill(propertyBuffer.begin(), propertyBuffer.end(), 0);
		uint8* buffer = &propertyBuffer[0];
		for(auto p : l) {
			Property& prop = *(reinterpret_cast<Property*>(buffer));
			prop.type = p.type;
			prop.offset = ShaderAsset::ParamSizeFromType((ParamDataType)p.type)*p.count+2*sizeof(uint16);
			buffer += prop.offset;
		}
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

	/*****************************************************/
	/* Material::StreamRequest							 */
	/*****************************************************/
	MaterialAsset::StreamRequest::StreamRequest(Asset *asset) : AssetStreamRequest(asset) {
	}

	MaterialAsset::StreamRequest::~StreamRequest() {
	}

	void MaterialAsset::StreamRequest::SetShader(const String& name, const String& options, const URL& location) {
		// construct the name, it will be of the form
		// Factory:Group.Name
		std::pair<String, String> strPair = StringUtils::Split(name);
		if(!strPair.first.length()) {
			strPair.first.swap(strPair.second);
			strPair.first = "Default";
		}
		strPair.second += ':';

		size_t pos = 0;
		while(pos < options.length()) {
			size_t comma = options.find_first_of(',', pos);
			uint8 index = (uint8)Convert::ToULong(options.substr(pos,comma));
			strPair.second += char(index+1);
			if (comma != String::npos)
				pos = comma+1;
			else
				break;
		}

		Asset* material = static_cast<Asset*>(GetStreamedObject());
		Asset* shader = nullptr;
		Component::Factory* factory =
				ComponentFactoryArchive::Instance().AsyncFindFactory(
						ShaderAsset::CLASS_ID, strPair.first);
		RenderManager::Instance().AsyncGetShaderGroups();
		if (factory) {
			shader = Bind(factory->AsyncCreate(strPair.second));
			if (!shader) {
				shader = Bind(ShaderAsset::Instance(
						static_cast<AssetManager*>(manager), strPair.second, location));
			}
		}

		if(!shader) {
			Warn("Failed to load shader for material: " + shader->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		GetMetaInfo().AddDependency(shader);
	}

	void MaterialAsset::StreamRequest::AddParameter(const String& name, TextureBase*& swapValue) {
		ShaderVariant& sv = params[name];
		sv = swapValue;
		if (sv.type == (uint16)ParamDataType::PDT_TEXTURE) {
			for(uint16 i = 0; i < sv.count; ++i) {
				TextureBase* tb = sv.GetTextureUnit(i).texture;
				if (tb && tb->IsTextureAsset()) {
					GetMetaInfo().AddDependency(static_cast<TextureAsset*>(tb));
				}
			}
		}
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
