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
#include <EffectAsset.h>
#include <ParameterBuffer.h>
#include <Material.h>

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

	enum Mask : uint32 {
		BACKGROUND = 1 << 0,
		TRANSLUCENT = 1 << 3,
		OVERLAY = 1 << 4,
		SHADOW_CASTER = 1 << 5,
		SHADOW_RECEIVER = 1 << 6,
		ALL = 0xffffffff
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

		void SetEffect(EffectAssetPtr& shader);
		void SetEffect(const EffectAsset::ID& id, const URL& location);
		void SetParamValue(uint32 offset, const void* data, uint32 amount);
		void SetTextureValue(uint32 offset, const TextureUnit* texture);
		void SetParameterBuffer(ParameterBuffer&& buff);
		void SetLayer(Layer layer);
		void SetMask(uint32 mask);
		void SetShaderOptions(StringUtils::WordList&& options);

	protected:

		StringUtils::WordList shaderOptions;
		friend class MaterialAsset;
	};

	MaterialAsset(const StringID name, const StringID factory);
	virtual ~MaterialAsset();

	inline const EffectAssetPtr& GetEffect() const {
		return effect;
	}

	inline EffectAssetPtr& GetEffect() {
		return effect;
	}

	inline uint8 GetLayer() const {
		return layer;
	}

	inline void SetLayer(uint8 layer) {
		this->layer = layer;
	}

	inline uint8 GetMask() const {
		return mask;
	}

	inline void SetMask(uint32 layer) {
		this->mask = layer;
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
	ParameterBuffer* PrepareParamBuffer(const ParamEntryTableItem& table);
	void SetParamData(const void* data, uint32 offset, uint32 size);
	void SetParamData(const TextureUnit* data, uint32 offset);
	void SetParameterBuffer(ParameterBuffer&& buff);
	void SetEffect(EffectAssetPtr& shader);

	// used as sort key
	uint8 layer;
	// mask
	uint32 mask;
	// shader asset
	EffectAssetPtr effect;
	// material
	Material material;

	friend class MaterialLoadRequest;
};

} /* namespace nextar */
#endif /* MATERIAL_H_ */
