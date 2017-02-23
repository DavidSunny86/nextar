/*
 * Pass.h
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */

#ifndef PASS_H_
#define PASS_H_

#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterState.h>
#include <TextureUnitState.h>
#include <RenderConstants.h>
#include <ShaderParam.h>
#include <NamedObject.h>
#include <TextureAsset.h>

namespace nextar {

class _NexEngineAPI Pass: public ContextObject,
		public NamedObject,
		public AllocGeneral {
	
	NEX_LOG_HELPER(Pass); 

public:

	enum ProgramStage
		: uint16 {
		STAGE_VERTEX,
		STAGE_HULL,
		STAGE_DOMAIN,
		STAGE_GEOMETRY,
		STAGE_FRAGMENT,
		STAGE_COUNT,
	};

	enum Flags {
		PROGRAM_DATA_INITED = 1 << 5,
	};

	enum Message {
		MSG_PASS_COMPILE, MSG_PASS_UPDATE_PARAMBUFFER_OFFSET
	};

	struct SamplerDesc {
		// comma/space seperated units bound to this sampler
		String unitsBound;
		TextureUnitParams texUnitParams;
	};

	typedef vector<SamplerDesc>::type TextureDescMap;

	struct ParamBufferOffsetParams {
		uint32 offset[(size_t) ParameterContext::CTX_COUNT];
		ParamBufferOffsetParams() {
			for (uint32 i = 0; i < ParameterContext::CTX_COUNT; ++i)
				offset[i] = 0;
		}
	};

	typedef map<String, AutoParamName>::type VarToAutoParamMap;

	class CompileParams {
	public:
		mutable std::atomic_flag inited;
		const StringUtils::WordList* compileOptions;
		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		Pass::TextureDescMap textureStates;
		ParamEntryTable* parameters;
		VarToAutoParamMap autoNames;
		//uint16* inputLayoutId;
		uint32 passIndex;
		String programSources[Pass::STAGE_COUNT];

		CompileParams() : passIndex(-1),
				compileOptions(nullptr), parameters(nullptr) {
			inited.clear(std::memory_order_release);
		}

		CompileParams(const CompileParams& other) = delete;
		Pass::CompileParams& operator =(const nextar::Pass::CompileParams &) = delete;
	};

	class _NexEngineAPI View: public ContextObject::View {
	public:

		View();

		void SwitchAndUpdateParams(CommitContext& context);
		void UpdateParams(CommitContext& context, ParameterContext paramContext,
				uint32 updateId);
		// Bind texture to a shader parameter. The number of units must match the desc->numUnits count.
		virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc,
				TextureBase* t) = 0;

		virtual void Update(nextar::RenderContext*, uint32 msg,
				ContextParamPtr);
		virtual void Compile(nextar::RenderContext*,
				const Pass::CompileParams&) = 0;

		// todo refer to the static table as well as a custom table
		// created from parsing the shader script
		// string should be in uppercase
		inline const AutoParam* MapParam(AutoParamName name) {
			return Pass::MapParam(name);
		}

		inline ParameterGroupList& GetSharedParameters() {
			return sharedParameters;
		}

	protected:

		ParameterGroupList sharedParameters;
		ParameterGroupEntries paramGroupEntries;
		SamplerParameter* samplers;
		uint32 numSamplerCount;
		uint32 viewNumber;
		uint32 frameNumber;
		uint32 passNumber;
	};

	// implemented
	Pass(Pass&& p);
	// not implemented
	Pass(StringID name, uint16 number);
	virtual ~Pass();

	inline uint16 GetPassNumber() const {
		return passNumber;
	}

	inline bool IsProgramDataInited() const {
		return flags & PROGRAM_DATA_INITED;
	}

	inline void SetProgramDataInited(bool v) {
		flags = v ? flags | PROGRAM_DATA_INITED : flags & ~PROGRAM_DATA_INITED;
	}

	//inline uint16 GetInputLayoutID() const {
	//	return inputLayoutUniqueID;
	//}
	//TextureBase* GetDefaultTexture(const String& name, uint32 index) const;
	//const TextureUnitParams* GetTextureUnit(const String& name) const;

	static void AddParamDef(AutoParamName autoName, ParamDataType type, ParameterContext context,
		ParamProcessorProc processor, uint32 size/*, const String& desc*/);

	static const AutoParam* MapParam(AutoParamName name);

	static uint32 MapSamplerParams(const String& name,
			const TextureDescMap& texMap, ParameterContext& context);

	// Set texture states, called during pass creation

	static inline ParamProcessorProc GetStructProcessor() {
		return customStructProcessor;
	}

	static inline ParamProcessorProc GetConstantProcessor() {
		return customConstantProcessor;
	}

	static inline ParamProcessorProc GetTextureProcessor() {
		return customTextureProcessor;
	}

protected:

	/* Unique number representing this pass */
	//uint16 inputLayoutUniqueID;
	uint16 flags;
	uint16 passNumber;

	//static AutoParam autoParams[AutoParamName::AUTO_COUNT];
	static ParamProcessorProc customConstantProcessor;
	static ParamProcessorProc customTextureProcessor;
	static ParamProcessorProc customStructProcessor;

	typedef array<AutoParam, (size_t)AutoParamName::AUTO_COUNT>::type AutoParamList;
	static AutoParamList autoParams;
	friend class ShaderAsset;

private:
	// not implemented
	Pass(const Pass& p);

};

typedef Pass* PassPtr;
typedef vector<Pass>::type PassList;

} /* namespace nextar */
#endif /* PASS_H_ */
