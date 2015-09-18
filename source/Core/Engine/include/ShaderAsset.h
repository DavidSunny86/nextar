/*
 * Shader.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <NexBase.h>
#include <Asset.h>
#include <SortKeyHelper.h>
#include <Pass.h>
#include <ShaderParam.h>
#include <FactoryPlugin.h>
#include <ParameterBuffer.h>
#include <SceneTraversal.h>

namespace nextar {

/**
 * ShaderUI : Shader {
 * 	ShaderPtr compiledShader;
 * }
 */
class _NexEngineAPI ShaderAsset: public nextar::Asset {
	NEX_LOG_HELPER(ShaderAsset)
	;
public:

	enum {
		CLASS_ID = Asset::CLASS_ASSET_SHADER,
	};

	enum {
		USE_FALLBACK = Asset::LAST_FLAG << 5,
	};

	class StreamRequest;

	typedef AssetTraits<ShaderAsset> Traits;
	typedef FactoryTraits<ShaderAsset> FactoryTraits;

	class _NexEngineAPI StreamPass {
	public:
		StringID name;
		Pass::ParamBufferOffsetParams offsets;
		Pass::CompileParams compileParams;
		//Pass::DefaultTextureUnitMap defaultTextureUnits;
		StreamPass(StringID _name) : name(_name) {}
		// not implemented
		StreamPass(const StreamPass& p);
		StreamPass(StreamPass&& p);

		StreamPass& operator = (const nextar::ShaderAsset::StreamPass &);
	};


	// todo Make this a list not a vector
	typedef vector<StreamPass>::type StreamPassList;

	class _NexEngineAPI StreamRequest: public AssetStreamRequest {
		NEX_LOG_HELPER(ShaderAsset::StreamRequest)
		;
	public:
		StreamRequest(ShaderAsset*);
		~StreamRequest();

		/* all parameters which are not auto type will have update frequency of material, which means they will
		 * be updated per material (using the material property table). */
		// Parameters are registered here for UI specific display (or when meta data has to be stored).
		// The parameter name is of the form: Parameter Category:Parameter Name. The category, if ignored,
		// will be set to General.
		// virtual void AddParam(const String& name, const String& param,
		//		const String& description, const String& defaultValue,
		//		ParamDataType type);
		// virtual void AddMacro(const String& name, const String& param,
		//		const String& description, bool defualtState);
		/* Add Pass */
		void AddPass(StringID name);
		/* Set pass paramter buffer data */
		void SetParameterBuffer(ParameterBuffer&& data);

		/* Pass related */
		void SetCompilationOptions(const String& options);
		void SetProgramSource(Pass::ProgramStage stage, String&& src);
		void SetRasterState(RasterState& state);
		void SetBlendState(BlendState& state);
		void SetDepthStencilState(DepthStencilState& state);
		// If called multiple times for the same unit, the texture will be appended in a
		// list and if the sampler is an array
		void AddSamplerUnit(TextureUnitParams& unit,
				const String& boundUnitNames);
		void AddAutoNameMapping(const String& varName, AutoParamName name);
		void SetRenderQueueFlags(uint32 flags);
		void SetSemanticMap(const Pass::VarToAutoParamMap& m);

	protected:

		uint32 renderQueueFlags;
		String compilationOptions;
		friend class ShaderAsset;
		StreamPassList::reverse_iterator currentPass;
		StreamPassList passes;
	};

	ShaderAsset(const StringID, const StringID factory);
	virtual ~ShaderAsset();

	inline uint16 GetShaderMask() const {
		return ((std::ptrdiff_t) (this))
				& (uint32) SortKeyHelper::SHADER_KEY_MASK;
	}

	inline uint16 GetTranslucency() const {
		return translucency;
	}

	inline const Pass& GetPass(uint32 i) const {
		return passes[i];
	}

	inline Pass& GetPass(uint32 i) {
		return passes[i];
	}

	virtual uint32 GetClassID() const override;


	const ParamEntryTableItem& GetParamTableItem(ParameterContext context) {
		return paramsPerContext[(uint32)context];
	}

	const ParameterBuffer& GetParameters() const {
		return passParamData;
	}

	inline uint16 GetRenderQueue() const {
		return renderQueue;
	}

	inline VisibilityMask GetVisibilityMask() const {
		return visibilityMask;
	}

	inline void SetRenderQueue(uint16 l) {
		renderQueue = (uint8)l;
	}
protected:

	virtual StreamNotification NotifyAssetLoadedImpl(nextar::StreamRequest* request) override;
	virtual void NotifyAssetUnloaded() override;
	virtual void NotifyAssetUpdated() override;

	virtual void UnloadImpl();

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);

	struct ParamTableBuilder {
		uint32 viewParamCount;
		uint32 frameParamCount;
		uint32 passParamCount;
		uint32 objectParamCount;
		uint32 materialParamCount;

		uint32 passParamOffset;
		uint32 objectParamOffset;
		uint32 materialParamOffset;

		ParamEntryTable passTable;
	};

	void _DestroyPasses();
	void _CompilePasses(StreamPassList& spl);
	void _BuildParameterTable(StreamPassList& spl);
	void _CompilePass(Pass& pass, StreamPass&, const String& co);

	// used as sort key
	uint8 translucency;
	uint8 renderQueue;
	VisibilityMask visibilityMask;

	PassList passes;
	// required when parameter is looked up by name
	ParamEntryTable paramLookup;

	ParamEntryTableItem paramsPerContext[(uint32) ParameterContext::CTX_COUNT];
	// todo
	ParameterBuffer passParamData;

	friend class StreamRequest;
};
} /* namespace nextar */
#endif /* SHADER_H_ */
