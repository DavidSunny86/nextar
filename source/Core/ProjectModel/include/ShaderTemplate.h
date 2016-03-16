/*
 * ShaderTemplate.h
 *
 *  Created on: 19-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERTEMPLATE_H_
#define SHADERTEMPLATE_H_

#include <NexProjectModel.h>
#include <AssetTemplate.h>
#include <ShaderAsset.h>
#include <RenderManager.h>

namespace nextar {

/*
 *
 */
class _NexProjectAPI ShaderTemplate :
		public AssetTemplate {
	NEX_LOG_HELPER(ShaderTemplate);
public:

	enum Type {
		CLASS_ID = Component::CLASS_SHADER_TEMPLATE,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	struct Parameter {
		ParamDataType type;
		String name;
		String description;
		ParameterContext context;
	};

	typedef map<String, Parameter>::type ParameterTable;

	struct ShaderOption {
		String activateOptions;
		String description;
	};

	typedef map<String, ShaderOption>::type ShaderOptionsTable;

	struct ShaderUnit {
		String compilationOptions;
		ShaderAssetPtr shaderObject;
	};

	typedef map<String, ShaderUnit>::type ShaderTable;

	struct SamplerUnit {
		String unitsBound;
		TextureUnitParams params;
	};

	typedef map<String, SamplerUnit>::type SamplerMap;

	typedef multimap<RenderManager::ShaderLanguage, std::pair<Pass::ProgramStage, String>>::type SourceMap;
	typedef Pass::VarToAutoParamMap VarToAutoParamMap;

	struct PassUnit {
		StringID name;
		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		SamplerMap textureUnitStates;
		VarToAutoParamMap semanticMap;
		SourceMap sourceMap;
	};

	typedef vector<PassUnit>::type PassList;

	class LoadStreamRequest;

	typedef AssetTraits<ShaderTemplate> Traits;
	typedef FactoryTraits<ShaderTemplate> FactoryTraits;

	class _NexProjectAPI SaveStreamRequest: public AssetStreamRequest {
	public:
		SaveStreamRequest(ShaderTemplate* shaderTemplate) : AssetStreamRequest(shaderTemplate) {}
	};

	class _NexProjectAPI LoadStreamRequest: public AssetStreamRequest {
		NEX_LOG_HELPER(ShaderTemplate::LoadStreamRequest);
	public:

		LoadStreamRequest(ShaderTemplate* shaderTemplate);
		void AddPass(StringID name);
		void SetProgramSource(Pass::ProgramStage stage,
				RenderManager::ShaderLanguage lang,
				String&& source);

		void SetRasterState(RasterState& state);
		void SetBlendState(BlendState& state);
		void SetDepthStencilState(DepthStencilState& state);
		void SetRenderFlags(uint32 flags);

		void AddTextureUnit(const String& unitName, const String& samplerName, ParameterContext context);
		void AddSampler(const String& samplerName,
				TextureUnitParams& unit);

		void AddParam(const String& param,
				const String& name,
				const String& description,
				ParamDataType type);
		void AddMacro(const String& name,
			const String& activateOptions,
			const String& description);
		void AddSemanticBinding(const String& var,
				AutoParamName name);

	protected:

		PassUnit* current;
	};

	ShaderTemplate(const StringID, const StringID);
	virtual ~ShaderTemplate();

	/* notify loaded/unloaded */
	virtual StreamNotification NotifyAssetSavedImpl(StreamRequest* request);

	virtual void UnloadImpl();

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);

	void AppendCompilerOptions(const StringUtils::WordList& definedParms, 
		const StringUtils::WordList& enabledOptions,
		String& outOptions);
	ShaderAssetPtr& GetShaderUnit(const StringUtils::WordList& options);
	ShaderAssetPtr& CreateShader(const String& hash, const StringUtils::WordList& options);

	// inlines
	inline uint32 GetPassCount() const {
		return (uint32)passes.size();
	}

	inline uint32 GetShaderCount() const {
		return (uint32)shaders.size();
	}

	inline const PassList& GetPassList() const {
		return passes;
	}

	inline const ShaderTable& GetShaderTable() const {
		return shaders;
	}

	inline void SetRenderFlags(uint32 renderFlags) {
		this->renderFlags = renderFlags;
	}

	inline uint32 GetRenderFlags() const {
		return renderFlags;
	}
	
	virtual uint32 GetClassID() const;
	virtual uint32 GetProxyID() const;

	void RegisterOptions(const String& options);
protected:

	void _AppendCompilerOption(const String& options, String& outCompilerOptions);

	class ShaderFromTemplate : public AssetLoaderImpl {
	public:

		ShaderFromTemplate(ShaderTemplate* parent,
				const StringUtils::WordList& compilationOptions);

		virtual void Load(InputStreamPtr&, AssetLoader&);

	protected:

		ShaderTemplate* parent;
		StringUtils::WordList compilationOptions;
	};

	friend class nextar::ShaderTemplate::ShaderFromTemplate;
	friend class nextar::ShaderTemplate::LoadStreamRequest;
	friend class nextar::ShaderTemplate::SaveStreamRequest;

	String GetHashNameFromOptions(const set<String>::type&);

	PassList passes;

	typedef map<String, uint32>::type CompilerMacroMap;

	CompilerMacroMap registeredOptions;
	uint32 renderFlags;
	ShaderTable shaders;
	ParameterTable parameters;
	ShaderOptionsTable macros;

};

} /* namespace nextar */

#endif /* SHADERTEMPLATE_H_ */
