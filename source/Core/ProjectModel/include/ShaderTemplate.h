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
		String description;
		ParameterContext context;
	};

	struct ParameterDesc : public Parameter {
		String activateOption;
	};

	typedef map<String, Parameter>::type ParameterTable;

	struct ShaderUnit {
		String longName;
		ShaderAssetPtr shaderObject;
	};

	typedef map<hash_t, ShaderUnit>::type ShaderTable;

	struct SamplerUnit {
		String unitsBound;
		TextureUnitParams params;
	};

	typedef map<hash_t, SamplerUnit>::type SamplerMap;

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

	struct TextureUnitDesc {
		String samplerName;
		ParameterContext context;
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
		void SetName(const String& name);
		void AddPass(StringID name);
		void SetProgramSource(Pass::ProgramStage::Type stage,
				RenderManager::ShaderLanguage lang,
				String&& source);

		void SetRasterState(RasterState& state);
		void SetBlendState(BlendState& state);
		void SetDepthStencilState(DepthStencilState& state);
		void SetRenderFlags(uint32 flags);

		void AddTextureUnit(const String& unitName, const TextureUnitDesc& params);
		void AddSampler(const String& samplerName, const TextureUnitParams& unit);
		void AddParam(const String& param, const ParameterDesc& parameter);
		void AddSemanticBinding(const String& varName, AutoParamName name);

	protected:

		PassUnit* current;
	};

	ShaderTemplate(const StringID, const StringID);
	virtual ~ShaderTemplate();

	/* notify loaded/unloaded */
	virtual StreamNotification NotifyAssetSavedImpl(StreamRequest* request);

	virtual void UnloadImpl();

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);
	ShaderAssetPtr& GetShaderUnit(const StringUtils::WordList& definedParms,
		const StringUtils::WordList& shaderOptions);
	ShaderAssetPtr& CreateShader(hash_t, const String& options);

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
	virtual String GetPoxyAssetExtension() const;

	uint32 RegisterOptions(const String& options);
protected:

	inline void _BindParamToOp(const String& name, const String& op) {
		MultiStringHelper h(paramActivationOptions[name]);
		h.PushBack(op);
	}

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

	static hash_t GetHashFromOptions(const String& allOptions, String& oLongName);

	PassList passes;

	typedef map<String, hash_t>::type OptionHash;

	uint32 renderFlags;
	ShaderTable shaders;
	NameValueMap paramActivationOptions;
	ParameterTable parameters;

};

} /* namespace nextar */

#endif /* SHADERTEMPLATE_H_ */
