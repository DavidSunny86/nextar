/*
 * ShaderTemplate.h
 *
 *  Created on: 19-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERTEMPLATE_H_
#define SHADERTEMPLATE_H_

#include <AssetTemplate.h>
#include <ShaderAsset.h>

namespace nextar {

/*
 *
 */
class ShaderTemplate :
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
	typedef map<uint32, String>::type SourceMap;

	struct PassUnit {
		StringID name;
		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		TextureUnitMap textureUnitStates;
		SourceMap sourceMap;
	};

	typedef vector<PassUnit>::type PassList;

	class StreamRequest;

	typedef AssetTraits<ShaderTemplate> Traits;
	typedef FactoryTraits<ShaderTemplate> FactoryTraits;

	class StreamRequest: public AllocGeneral, public AssetStreamRequest {
		NEX_LOG_HELPER(ShaderTemplate::StreamRequest);
	public:

		StreamRequest(ShaderTemplate* shaderTemplate);
		void AddPass(StringID name);
		void SetProgramSource(Pass::ProgramStage stage,
				RenderManager::ShaderProgramLanguage lang,
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
	virtual void NotifyAssetLoaded();
	virtual void NotifyAssetUnloaded();
	virtual void NotifyAssetUpdated();
	virtual void NotifyAssetSaved();

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

	// load a shader

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
	friend class nextar::ShaderTemplate::StreamRequest;

	String GetHashNameFromOptions(const StringUtils::WordList& );

	PassList passes;

	ShaderTable shaders;
	ParameterTable parameters;
	MacroTable macros;

};

} /* namespace nextar */

#endif /* SHADERTEMPLATE_H_ */
