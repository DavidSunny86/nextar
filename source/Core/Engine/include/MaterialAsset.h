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

	class StreamRequest;

	typedef AssetTraits<MaterialAsset> Traits;
	typedef FactoryTraits<MaterialAsset> FactoryTraits;

	class _NexEngineAPI StreamRequest: public AllocGeneral, public AssetStreamRequest {
		NEX_LOG_HELPER(MaterialAsset::StreamRequest)
		;
	public:

		StreamRequest(Asset*);
		~StreamRequest();

		void SetShader(ShaderAssetPtr& shader);
		void SetShader(const ShaderAsset::ID& id, const URL& location);
		void SetParamBufferSize(uint32 paramBufferSize);
		void SetParamValue(uint32 offset, const void* data, size_t amount);
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
	virtual bool NotifyAssetLoadedImpl();
	virtual void NotifyAssetUnloaded();
	virtual void NotifyAssetUpdated();
	virtual bool NotifyAssetSavedImpl();

	virtual void UnloadImpl();

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void DestroyStreamRequestImpl(nextar::StreamRequest*&, bool load =
			true);

	void PrepareMaterial(MaterialAsset::StreamRequest*);
	void SetParamBufferSize(size_t size);
	void SetParamData(const void* data, size_t offset, size_t size);
	void SetParamData(const TextureUnit* data, size_t offset);
	void SetParameterBuffer(ParameterBuffer&& buff);
	void SetShader(ShaderAssetPtr& shader);

	// used as sort key
	uint8 layerMask;
	ShaderAssetPtr shader;
	//ConstantBufferPtr materialParameters;
	// todo
	ParameterBuffer materialParamData;

	friend class StreamRequest;
};

} /* namespace nextar */
#endif /* MATERIAL_H_ */
