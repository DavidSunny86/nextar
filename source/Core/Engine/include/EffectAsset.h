/*
 * EffectAsset.h
 *
 *  Created on: 04-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_EFFECTASSET_H_
#define CORE_ENGINE_INCLUDE_EFFECTASSET_H_

#include <Asset.h>
#include <ShaderOptions.h>
#include <ShaderUnit.h>
#include <RenderState.h>
#include <RenderPass.h>
#include <Material.h>

namespace nextar {

/*
 * Effect files are stored in 2 parts, one that is reloaded, one that is persistent
 * The reloaded part is the source, that is requested every time a material
 * has to be loaded.
 * If the effect is already loaded, a reload request is automatically registered.
 * The reload will load only the part of the file which is required for the shader
 * to be compiled with new shader options.
 * */
class _NexEngineAPI EffectAsset: public Asset {

	NEX_LOG_HELPER(EffectAsset);

public:
	enum {
		CLASS_ID = Asset::CLASS_ASSET_EFFECT,
	};

	enum Flags {
		WAS_LOADED = Asset::LAST_FLAG << 0,
		LAST_FLAG = Asset::LAST_FLAG << 1,
	};

	typedef AssetTraits<EffectAsset> Traits;
	typedef FactoryTraits<EffectAsset> FactoryTraits;
	typedef ShaderUnit::TextureDescMap TextureDescMap;
	typedef ShaderUnit::VarToAutoParamMap VarToAutoParamMap;
	typedef ShaderUnit::ShaderData ShaderData;
	typedef ShaderUnit::SamplerDesc SamplerDesc;

	class _NexEngineAPI SourceLoadRequest: public AssetStreamRequest {
	public:
		SourceLoadRequest(EffectAsset*);
		// source
		void AddTags(const StringUtils::WordList& tags);
		void AddTag(hash_t tag) {
			_data.tags.push_back(tag);
		}
		ShaderData& GetData() {
			return _data;
		}
		void SetProgramSource(PipelineStage::Name stage, String&& src);
		void SetSemanticMap(VarToAutoParamMap&& m);
		void AddSamplerUnit(TextureUnitParams& unit,
				StringUtils::WordList&& boundUnitNames);
		void AddAutoNameMapping(const String& varName, AutoParamName name);
		

	protected:
		ShaderData _data;
	};

	class _NexEngineAPI FullLoadRequest: public SourceLoadRequest {
		NEX_LOG_HELPER(ShaderAsset::FullLoadRequest)
		;
	public:
		FullLoadRequest(EffectAsset*);
		~FullLoadRequest();

		/* Pass related */
		inline void SetRasterState(const RasterState& state) {
			this->state.raster = state;
			this->state.raster.UpdateHash();
		}
		inline void SetBlendState(const BlendState& state) {
			this->state.blend = state;
			this->state.blend.UpdateHash();
		}
		inline void SetDepthStencilState(const DepthStencilState& state) {
			this->state.dethStencil = state;
			this->state.dethStencil.UpdateHash();
		}
		inline void SetRenderQueueFlags(uint32 flags) {
			renderQueueFlags = flags;
		}

	protected:
		uint32 renderQueueFlags;
		RenderState::Params state;
	};


	EffectAsset(const StringID name, const StringID factory);
	virtual ~EffectAsset();

	inline bool WasLoaded() const {
		return (flags & WAS_LOADED);
	}

	void ResolveMaterial(const StringUtils::WordList& options, Material& m);
	void ResolveMaterialSingle(const StringUtils::WordList& options,
		const StringUtils::WordList& renderPassOptions, Material& m);

	void AsyncAcquireData();
	void AsyncReleaseData();

protected:

	virtual StreamRequest* CreateStreamRequestImpl(bool load) override;

	bool _CanCompile(const ShaderData& data, const RenderPass::Info& info);
	int32 _Resolve(const StringUtils::WordList& options);
	
	ShaderData* _GetShaderData();
	int32 _FindUnit(const String& id, hash_t h);
	int32 _CreateUnit(const String& id, hash_t h, const ShaderOptions&);

	spinlock_mutex _lock;
	atomic_uint acquireRef;
	SourceLoadRequest* loadRequest;
	ShaderUnitList shaderUnits;
	RenderStatePtr renderState;
};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_EFFECTASSET_H_ */
