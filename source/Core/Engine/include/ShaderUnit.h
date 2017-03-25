/*
 * ShaderUnit.h
 *
 *  Created on: 04-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_SHADERUNIT_H_
#define CORE_ENGINE_INCLUDE_SHADERUNIT_H_

#include <NexBase.h>
#include <ShaderParam.h>
#include <ParameterBuffer.h>


namespace nextar {


class _NexEngineAPI ShaderUnit :
public ContextObject,
public AllocGeneral {

public:

	struct SamplerDesc {
		StringUtils::WordList unitsBound;
		TextureUnitParams texUnitParams;
	};

	typedef vector<SamplerDesc>::type TextureDescMap;
	typedef map<hash_t, AutoParamName>::type VarToAutoParamMap;

	struct ShaderData {
		VarToAutoParamMap autoNames;
		TextureDescMap textureStates;
		String programSources[PipelineStage::EFFECT_STAGE_COUNT];
	};

	struct CompiledOutput {
		std::atomic_flag generated;
	};

	struct CompileParams {

		const ShaderData* _data;
		const ShaderOptions* _options;
		CompiledOutput _outData;
	};

	ShaderUnit(hash_t hash);
	
	class View : public ContextObject::View {
	public:

		virtual void Compile(RenderContext*,
			const ShaderUnit::CompileParams&) = 0;
		// Bind texture to a shader parameter. The number of units must match the desc->numUnits count.
		virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc,
			TextureBase* t) = 0;

		virtual void Update(nextar::RenderContext*, uint32 msg,
			ContextParamPtr);

	};

protected:
	friend class EffectAsset;

	hash_t optionHash;
	// required when parameter is looked up by name
	ParamEntryTable paramLookup;
	// parameter entry by context
	ParamEntryTableItem paramsPerContext[(uint32)ParameterContext::CTX_COUNT];

#ifdef NEX_DEBUG
	inline void SetId(const StringUtils::WordList& id) {
		_id = id;
	}

	inline bool VerifyId(const String& id) {
		return (this->_id == id) != 0;
	}
	// for debug only possibly
	StringUtils::WordList _id;
#endif

};

typedef vector<ShaderUnit>::type ShaderUnitList;

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_SHADERUNIT_H_ */
