/*
 * MaterialTemplate.h
 *
 *  Created on: 25-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALTEMPLATE_H_
#define MATERIALTEMPLATE_H_

#include <NexProjectModel.h>
#include <AssetTemplate.h>
#include <MaterialAsset.h>
#include <ShaderTemplate.h>

namespace nextar {

/*
 *
 */
class _NexProjectAPI MaterialTemplate: public AssetTemplate {
	NEX_LOG_HELPER(MaterialTemplate);
public:
	enum Type {
		CLASS_ID = Component::CLASS_MATERIAL_TEMPLATE,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	typedef AssetTraits<MaterialTemplate> Traits;
	typedef FactoryTraits<MaterialTemplate> FactoryTraits;

	class _NexProjectAPI StreamRequest: public AssetStreamRequest {
		NEX_LOG_HELPER(MaterialTemplate::StreamRequest);
	public:
		StreamRequest(MaterialTemplate* materialTemplate);

		void SetMaterialID(const MaterialAsset::ID& id);
		void SetShader(SharedComponent::ID& shader, const URL& location);
		void EnableOptions(const StringUtils::WordList& options);
		void SetCompilationOptions(const StringUtils::WordList& options);
		void AddParam(const String& name, const String& value);
		void SetLayer(uint8 layer);
	};

	MaterialTemplate(const StringID, const StringID);
	virtual ~MaterialTemplate();

	/* notify loaded/unloaded */
	virtual void NotifyAssetUnloaded();
	virtual void NotifyAssetUpdated();
	virtual StreamNotification NotifyAssetLoadedImpl(nextar::StreamRequest* request) override;
	virtual StreamNotification NotifyAssetSavedImpl(nextar::StreamRequest* request) override;

	void SetMaterialID(const MaterialAsset::ID& id);
	void SetShader(const ShaderTemplatePtr& shader);
	void EnableOptions(const StringUtils::WordList& options);
	void SetCompilationOptions(const StringUtils::WordList& options);
	void AddParam(const String& name, const String& value);

	virtual void UnloadImpl();
	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);

	void SetLayer(Layer layer) {
		this->layer = layer;
	}

	Layer GetLayer() const {
		return layer;
	}

	MaterialAssetPtr GetMaterial() const {
		return material;
	}

	ShaderTemplatePtr GetShaderTemplate() const {
		return shader;
	}

	const String& GetOptions() const {
		return compilationOptions;
	}

	// load a shader
	virtual uint32 GetClassID() const;
	virtual uint32 GetProxyID() const;
	virtual String GetPoxyAssetExtension() const;

	inline MaterialAsset::ID GetMaterialID() const {
		return assetId;
	}

protected:

	class MaterialFromTemplate : public AssetLoaderImpl {
	public:

		MaterialFromTemplate(MaterialTemplate* parent,
				const ShaderAssetPtr& shader);

		virtual void Load(InputStreamPtr&, AssetLoader&);

	protected:

		MaterialTemplate* parent;
		ShaderAssetPtr shader;
	};

	friend class MaterialFromTemplate;

	typedef NameValueMap ParamValueMap;

	Layer layer;
	StringUtils::WordList definedParams;
	StringUtils::WordList enableOptions;
	StringUtils::WordList compilationOptions;
	MaterialAsset::ID assetId;
	ParamValueMap paramValues;
	ShaderTemplatePtr shader;
	MaterialAssetPtr material;
};

} /* namespace nextar */

#endif /* MATERIALTEMPLATE_H_ */
