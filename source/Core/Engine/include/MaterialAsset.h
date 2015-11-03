/*
 * Material.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Asset.h>
#include <ShaderParam.h>
#include <ShaderAsset.h>
#include <ParameterBuffer.h>

namespace nextar {

struct ParamEntryTableItem;
/* Non automatic parameters for shaders and shader options
 * and the compiled shader technique reference stored here.
 */
class _NexEngineAPI MaterialAsset: public nextar::Asset {
	NEX_LOG_HELPER(MaterialAsset);
public:

	enum Type {
		CLASS_ID = Component::CLASS_ASSET_MATERIAL, 
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	class MaterialLoadRequest;

	typedef AssetTraits<MaterialAsset> Traits;
	typedef FactoryTraits<MaterialAsset> FactoryTraits;

	class _NexEngineAPI MaterialLoadRequest: public AssetStreamRequest {
		NEX_LOG_HELPER(MaterialAsset::MaterialLoadRequest)
		;
	public:

		MaterialLoadRequest(Asset*);
		~MaterialLoadRequest();

		void SetShader(ShaderAssetPtr& shader);
		void SetShader(const ShaderAsset::ID& id, const URL& location);
		void PrepareParamBuffer(const ParamEntryTableItem& table);
		void SetParamValue(uint32 offset, const void* data, uint32 amount);
		void SetTextureValue(uint32 offset, const TextureUnit* texture);
		void SetParameterBuffer(ParameterBuffer&& buff);
		void SetLayer(Layer layer);

	protected:
		friend class MaterialAsset;
	};

	MaterialAsset(const StringID name, const StringID factory);
	virtual ~MaterialAsset();

	inline const ShaderAssetPtr& GetShader() const {
		return shader;
	}

	inline ShaderAssetPtr& GetShader() {
		return shader;
	}

	inline uint8 GetLayerMask() const {
		return layerMask;
	}

	inline void SetLayerMask(uint8 layer) {
		layerMask = layer;
	}


	virtual uint32 GetClassID() const;

	const ParameterBuffer* GetParameters() const {
		return &materialParamData;
	}

protected:
	/* notify loaded/unloaded */
	virtual StreamNotification NotifyAssetLoadedImpl(StreamRequest* request) override;
	virtual StreamNotification NotifyAssetSavedImpl(StreamRequest* request) override;

	virtual void NotifyAssetUnloaded() override;
	virtual void NotifyAssetUpdated() override;


	virtual void UnloadImpl();

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);

	void PrepareMaterial(MaterialAsset::MaterialLoadRequest*);
	void PrepareParamBuffer(const ParamEntryTableItem& table);
	void SetParamData(const void* data, uint32 offset, uint32 size);
	void SetParamData(const TextureUnit* data, uint32 offset);
	void SetParameterBuffer(ParameterBuffer&& buff);
	void SetShader(ShaderAssetPtr& shader);

	// used as sort key
	uint8 layerMask;
	ShaderAssetPtr shader;
	//ConstantBufferPtr materialParameters;
	// todo
	ParameterBuffer materialParamData;

	friend class MaterialLoadRequest;
};

} /* namespace nextar */
#endif /* MATERIAL_H_ */
