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

	enum {
		CLASS_ID = Asset::CLASS_ASSET_EFFECT,
	};

	enum Flags {
		SOURCES_LOADED = 1 << 0,
	};

	typedef AssetTraits<EffectAsset> Traits;
	typedef FactoryTraits<EffectAsset> FactoryTraits;
	typedef ShaderUnit::TextureDescMap TextureDescMap;
	typedef ShaderUnit::VarToAutoParamMap VarToAutoParamMap;
	typedef ShaderUnit::ShaderData ShaderData;

	class _NexEngineAPI ReloadRequest: public AssetStreamRequest {
		// source
		void SetProgramSource(Pass::ProgramStage::Type stage, String&& src);
		void SetSemanticMap(const Pass::VarToAutoParamMap& m);
		// If called multiple times for the same unit, the texture will be appended in a
		// list and if the sampler is an array
		void AddSamplerUnit(TextureUnitParams& unit,
				const String& boundUnitNames);
		void AddAutoNameMapping(const String& varName, AutoParamName name);
	protected:
		ShaderData _data;
	};

	class _NexEngineAPI StreamRequest: public ReloadRequest {
		NEX_LOG_HELPER(ShaderAsset::StreamRequest)
		;
	public:
		StreamRequest(EffectAsset*);
		~StreamRequest();

		/* Pass related */
		void SetRasterState(const RasterState& state);
		void SetBlendState(const BlendState& state);
		void SetDepthStencilState(const DepthStencilState& state);
		void SetRenderQueueFlags(uint32 flags);

	protected:

		uint32 renderQueueFlags;
		RenderState::Params state;
	};


	void ResolveMaterial(const StringUtils::WordList& options, Material& m);

	void AsyncAcquireData();
	void AsyncReleaseData();


protected:

	ShaderData& _GetShaderData();
	int32 _CreateUnit(const String& id, hash_t h, const ShaderOptions&);

	atomic_uint acquireRef;

	ShaderUnitList shaderUnits;
	RenderStatePtr renderState;
};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_EFFECTASSET_H_ */
