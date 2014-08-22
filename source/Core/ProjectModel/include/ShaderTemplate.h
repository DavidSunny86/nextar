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
public:

	enum Type {
		CLASS_ID = Component::CLASS_SHADER_TEMPLATE,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	struct Parameter {
		ParamDataType type;
		String uiName;
		String uiDescription;
		String catagory;

	};

	typedef map<String, Parameter>::type ParameterTable;

	struct Macro {
		Macro() : index(-1) {}
		uint32 index;
		String uiName;
		String uiDescription;
	};

	typedef map<String, Macro>::type MacroTable;

	struct ShaderUnit {
		String compilationOptions;
		ShaderAssetPtr shaderObject;
	};

	typedef map<String, ShaderUnit>::type ShaderTable;

	struct TextureUnit {

		TextureUnitParams params;
		TextureAssetPtr defaultTexture;
		URL defaultTexturePath;
	};

	typedef map<String, TextureUnit>::type TextureUnitMap;
	typedef multimap<RenderManager::ShaderLanguage, std::pair<Pass::ProgramStage, String>>::type SourceMap;

	struct PassUnit {
		StringID name;
		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		TextureUnitMap textureUnitStates;
		SourceMap sourceMap;
	};

	typedef vector<PassUnit>::type PassList;

	class LoadStreamRequest;

	typedef AssetTraits<ShaderTemplate> Traits;
	typedef FactoryTraits<ShaderTemplate> FactoryTraits;

	class _NexProjectAPI SaveStreamRequest: public AllocGeneral, public AssetStreamRequest {
	public:
		SaveStreamRequest(ShaderTemplate* shaderTemplate) : AssetStreamRequest(shaderTemplate) {}
	};

	class _NexProjectAPI LoadStreamRequest: public AllocGeneral, public AssetStreamRequest {
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

		void AddTextureUnit(const String& unitName,
				TextureUnitParams& unit,
				URL& defaultTexturePath);
		virtual void AddParam(const String& param,
				const String& name,
				const String& description,
				ParamDataType type);
		virtual void AddMacro(const String& param,
				const String& name,
				const String& description);

	protected:
		PassUnit* current;
	};

	ShaderTemplate(const StringID, const StringID);
	virtual ~ShaderTemplate();

	/* notify loaded/unloaded */
	virtual void NotifyAssetUnloaded();
	virtual void NotifyAssetUpdated();
	virtual bool NotifyAssetLoadedImpl();
	virtual bool NotifyAssetSavedImpl();

	virtual void UnloadImpl();

	virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void DestroyStreamRequestImpl(nextar::StreamRequest*&, bool load);

	ShaderAssetPtr& GetShaderUnit(const StringUtils::WordList& options);
	ShaderAssetPtr& CreateShader(const String& hash, const StringUtils::WordList& options);

	// inlines
	inline uint32 GetPassCount() const {
		return passes.size();
	}

	inline uint32 GetShaderCount() const {
		return shaders.size();
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

protected:

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

	String GetHashNameFromOptions(const StringUtils::WordList& );

	PassList passes;

	uint32 renderFlags;
	ShaderTable shaders;
	ParameterTable parameters;
	MacroTable macros;

};

} /* namespace nextar */

#endif /* SHADERTEMPLATE_H_ */
