/*
 * Material.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"

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
		void SetValue(Material*, const ShaderVariant&);

		friend class Material;
	};
	
	/****************************************
	 * MaterialManater
	 ****************************************/
	MaterialManager::MaterialManager(const String& name) : AssetManager(name) {
	}

	MaterialManager::~MaterialManager() {
	}

	Component* MaterialManager::AsyncCreateImpl(int type, const String& name) {
		if (type == Asset::COMPONENT_ASSET_MATERIAL)
			return NEX_NEW Material(this, name);
		return 0;
	}

	/****************************************
	 * Material
	 ****************************************/
	Material::Material(MaterialManager* manager, const String& name) : Asset(manager, name) {
	}

	Material::~Material() {
		_RelievePropertyBuffer();
	}

	Material* Material::Instance(AssetManager* manager, const String& name, const URL& location) {
		Material* material = static_cast<Material*>(manager->AsyncFindOrCreate(Material::TYPE, name));
		if(material)
			material->SetAssetLocator(location);
		return material;
	}

	int Material::GetType() const {
		return TYPE;
	}

	void Material::NotifyAssetLoaded() {
		/* loaded only when dependencies are resolved */
		Material::StreamRequest* req = static_cast<Material::StreamRequest*>(GetStreamRequest());
		_PrepareMaterial(req);
		Asset::NotifyAssetLoaded();
	}

	void Material::NotifyAssetUnloaded() {
		// @todo
		Asset::NotifyAssetUnloaded();
	}

	void Material::NotifyAssetUpdated() {
		// @todo
		Asset::NotifyAssetUpdated();
	}

	void Material::_RelievePropertyBuffer() {
		uint8* begin = &propertyBuffer[0];
		uint8* end = begin + propertyBuffer.size();
		while (begin < end) {
			Property& p = *(reinterpret_cast<Property*>(begin));
			p.DestructFromType();
			begin += p.offset;
		}
	}

	void Material::_PrepareMaterial(Material::StreamRequest* req) {
		_PreparePropertyBuffer();
		_PrepareParamData(req->params);
	}

	void Material::_PrepareParamData(const ShaderParamMap& paramMap) {
		uint8* buffer = &propertyBuffer[0];

		const Shader::ShaderParamInfoList& l = shader->GetShaderParams();
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

	void Material::_PreparePropertyBuffer() {

		const Shader::ShaderParamInfoList& l = shader->GetShaderParams();
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
			prop.offset = Shader::ParamSizeFromType((ParamDataType)p.type)*p.count+2*sizeof(uint16);
			buffer += prop.offset;
		}
	}

	void Material::LoadImpl(nextar::StreamRequest* request, bool) {
		Loader loader(request);
		loader.Serialize();
	}

	void Material::UnloadImpl(nextar::StreamRequest* req, bool isStreamed) {
		// @todo
	}

	nextar::StreamRequest* Material::CreateStreamRequestImpl(bool load) {
		return NEX_NEW Material::StreamRequest(this);
	}

	void Material::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		Material::StreamRequest* req = static_cast<Material::StreamRequest*>(request);
		NEX_DELETE req;
		request = nullptr;
	}

	/*****************************************************/
	/* Material::StreamRequest							 */
	/*****************************************************/
	Material::StreamRequest::StreamRequest(Asset *asset) : AssetStreamRequest(asset) {
	}

	Material::StreamRequest::~StreamRequest() {
	}

	void Material::StreamRequest::SetShader(const String& name, const String& options, const URL& location) {
		// construct the name
		std::pair<String, String> strPair = StringUtils::Split(name);
		if(!strPair.second.length()) {
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

		ComponentManager* manager =
				ComponentManagerArchive::Instance().AsyncFindManager(Shader::TYPE, strPair.first);
		if (manager) {
			shader = Bind(manager->AsyncFind(strPair.second));
			if (!shader) {
				shader = Bind(Shader::Instance(
						static_cast<AssetManager*>(manager), strPair.second, location));
			}

		}
		Asset* shader = static_cast<Asset*>(GetStreamedObject());
		if(!shader) {
			Warn("Failed to load shader for material: " + shader->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		GetMetaInfo().AddDependency(shader);
	}

	void Material::StreamRequest::AddParameter(const String& name, ShaderVariant&& swapValue) {
		ShaderVariant& sv = params[name];
		sv = swapValue;
		if (sv.type == (uint16)ParamDataType::PDT_TEXTURE) {
			for(uint16 i = 0; i < sv.count; ++i) {
				TextureBase* tb = sv.GetTextureUnit(i).texture;
				if (tb && tb->IsTextureAsset()) {
					GetMetaInfo().AddDependency(static_cast<Texture*>(tb));
				}
			}
		}
	}

	/*****************************************************/
	/* Material::Loader       							 */
	/*****************************************************/
	NEX_IMPLEMENT_FACTORY(Material::Loader);
	Material::Loader::Loader(nextar::StreamRequest* material) : requestPtr(material) {
	}

	Material::Loader::~Loader() {
	}

	void Material::Loader::Serialize() {
		Material* materialPtr = static_cast<Material*>(requestPtr->GetStreamedObject());
		const URL& location = materialPtr->GetAssetLocator();
		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		Material::LoaderImpl* impl = GetImpl(ext);
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
