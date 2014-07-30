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
		public AllocGraphics {
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
		TextureUnitParams texUnitParams;
		TextureBase* defaultTexture;
		~SamplerDesc() {
			if (defaultTexture && defaultTexture->IsTextureAsset()) {
				static_cast<TextureAsset*>(defaultTexture)->Release();
				defaultTexture = nullptr;
			}
		}
	};

	typedef map<String, SamplerDesc>::type TextureDescMap;

	struct ParamBufferOffsetParams {
		uint32 offset[(size_t) ParameterContext::CTX_COUNT];
	};

	struct CompileParams {
		mutable std::atomic_flag inited;
		const StringUtils::WordList* compileOptions;
		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		Pass::TextureDescMap textureStates;
		ParamEntryTable* parameters;
		//uint16* inputLayoutId;
		uint32 passIndex;
		String programSources[Pass::STAGE_COUNT];
	};

	class _NexEngineAPI View: public ContextObject::View {
	public:

		View();

		void SwitchAndUpdateParams(CommitContext& context);
		void UpdateParams(CommitContext& context, ParameterContext paramContext,
				uint32 updateId);
		// Bind texture to a shader parameter. The number of units must match the desc->numUnits count.
		virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc,
				const TextureUnit* tu) = 0;

		virtual void Update(nextar::RenderContext*, uint32 msg,
				ContextParamPtr);
		virtual void Compile(nextar::RenderContext*,
				const Pass::CompileParams&) = 0;

		// todo refer to the static table as well as a custom table
		// created from parsing the shader script
		inline const AutoParam* MapParam(const String& name) {
			return Pass::MapParam(name);
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

	// not implemented
	Pass(Pass&& p);
	// not implemented
	Pass(const Pass& p);
	// not implemented

	Pass(StringID name);
	virtual ~Pass();

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

	static const AutoParam* MapParam(const String& name);
	static const SamplerDesc* MapSamplerParams(const String& name,
			const TextureDescMap& texMap);

	// Set texture states, called during pass creation

	static inline AutoParamProcessor* GetStructProcessor() {
		return customStructProcessor;
	}

	static inline AutoParamProcessor* GetConstantProcessor() {
		return customConstantProcessor;
	}

	static inline AutoParamProcessor* GetTextureProcessor() {
		return customTextureProcessor;
	}

protected:

	/* Unique number representing this pass */
	//uint16 inputLayoutUniqueID;
	uint16 flags;

	//static AutoParam autoParams[AutoParamName::AUTO_COUNT];
	static AutoParamProcessor* customConstantProcessor;
	static AutoParamProcessor* customTextureProcessor;
	static AutoParamProcessor* customStructProcessor;

	typedef map<StringRef, AutoParam*>::type AutoParamMap;
	static AutoParamMap autoParams;
	friend class ShaderAsset;
};

typedef Pass* PassPtr;
typedef vector<Pass>::type PassList;

} /* namespace nextar */
#endif /* PASS_H_ */
