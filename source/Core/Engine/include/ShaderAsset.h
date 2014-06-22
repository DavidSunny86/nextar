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

	struct StreamPass {
		StringID name;
		Pass::ParamBufferOffsetParams offsets;
		Pass::CompileParams compileParams;
		//Pass::DefaultTextureUnitMap defaultTextureUnits;
	};

	// todo Make this a list not a vector
	typedef vector<StreamPass>::type StreamPassList;

	class StreamRequest: public AllocGeneral, public AssetStreamRequest {
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
		virtual void AddParam(const String& name, const String& param,
				const String& description, const String& defaultValue,
				ParamDataType type);
		virtual void AddMacro(const String& name, const String& param,
				const String& description, bool defualtState);
		/* Add Pass */
		virtual void AddPass(StringID name);
		/* Set pass paramter buffer data */
		virtual void SetParamterBuffer(ParameterBuffer&& data);
		/* Pass related */
		void SetOptions(const String& options);
		void SetProgramSource(Pass::ProgramStage stage, String&& src);
		void SetRasterState(RasterState& state);
		void SetBlendState(BlendState& state);
		void SetDepthStencilState(DepthStencilState& state);
		// If called multiple times for the same unit, the texture will be appended in a
		// list and if the sampler is an array
		void AddTextureUnit(const String& unitName, TextureUnitParams& unit,
				TextureBase* defaultTexture);

	protected:

		friend class ShaderAsset;
		uint32 currentPass;
		StreamPassList passes;
		String compilationOpt;
	};

	ShaderAsset(const StringID);
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

	virtual uint32 GetClassID() const override;

	// todo
	static ParamDataType MapParamType(const String& typeName);

protected:

	virtual void NotifyAssetLoaded();
	virtual void NotifyAssetUnloaded();
	virtual void NotifyAssetUpdated();

	virtual void UnloadImpl(nextar::StreamRequest* req, bool async);

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void DestroyStreamRequestImpl(nextar::StreamRequest*&, bool load =
			true);

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
