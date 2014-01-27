/*
 * Shader.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SHADER_H_
#define SHADER_H_

#include "NexSys.h"
#include "Asset.h"
#include "Pass.h"
#include "ShaderParam.h"
#include "FactoryPlugin.h"
#include "GpuProgram.h"

namespace nextar {

	class SceneParameters;
	/**
	 * ShaderUI : Shader {
	 * 	ShaderPtr compiledShader;
	 * }
	 */
	class _NexExport ShaderAsset: public nextar::Asset,
	public ContextObject {
		NEX_LOG_HELPER(ShaderAsset);
	public:

		enum Type {
			TYPE = Asset::COMPONENT_ASSET_SHADER,
		};

		enum {
			USE_FALLBACK = Asset::LAST_FLAG << 5,
		};

		struct ParamDef {
			uint8 updateFrequency;
			uint8 type;
			uint16 autoName;
			ParamDef(uint16 autoName, uint8 type, uint8 updateFreq =
					(uint8)UpdateFrequency::PER_MATERIAL);
		};

		typedef map<String, ParamDef>::type ParamDefMap;
		typedef std::unique_ptr<Pass> PassPtr;
		typedef vector<PassPtr>::type PassList;
		typedef Pass::DefaultTextureUnitMap DefaultTextureUnitMap;

		struct Macro {
			bool value;
			String macro;
		};

		typedef vector<Macro>::type MacroTable;

		class StreamRequest;
		class Loader;
		struct LoaderImpl;

		struct LoaderImpl {
			virtual void Load(InputStreamPtr& input, ShaderAsset::Loader& shader) = 0;
		};

		class Loader {
			NEX_LOG_HELPER(ShaderAsset::Loader);
			NEX_DECLARE_FACTORY(LoaderImpl);

		public:
			Loader(nextar::StreamRequest* shader);
			~Loader();

			inline nextar::StreamRequest* GetRequestPtr() {
				return shaderRequest;
			}

			void Serialize();

		protected:
			nextar::StreamRequest* shaderRequest;
		};

		struct StreamPass {
			String name;
			String programSources[Pass::NUM_STAGES];
			RasterState rasterState;
			BlendState blendState;
			DepthStencilState depthStencilState;
			Pass::DefaultTextureUnitMap defaultTextureUnits;
		};

		typedef vector<StreamPass>::type StreamPassList;

		class StreamRequest : public AllocGeneral,
			public AssetStreamRequest {
			NEX_LOG_HELPER(ShaderAsset::StreamRequest);
		public:
			StreamRequest(ShaderAsset*);
			~StreamRequest();

			/* all parameters which are not auto type will have update frequency of material, which means they will
			 * be updated per material (using the material property table). */
			virtual void AddParam(const String& name,
					const String& param, const String& description, const String& defaultValue, ParamDataType type);
			virtual void AddMacro(const String& name,
					const String& param, const String& description, bool defualtState);
			/* Add Pass */
			virtual void AddPass(const String& name);

			/* Pass related */
			void SetProgramSource(GpuProgram::Type, const String& src);
			void SetRasterState(RasterState& state);
			void SetBlendState(BlendState& state);
			void SetDepthStencilState(DepthStencilState& state);
			void BindDefaultTexture(const String& unitName, TextureBase* texture);
			void AddTextureUnit(const String& unitName, TextureUnitParams& unit);

		protected:

			friend class ShaderAsset;
			uint32 currentPass;
			StreamPassList passes;

			MacroTable macroTable;
			String shaderOptionSuffix;
		};

		ShaderAsset(AssetManager*, const String&);
		virtual ~ShaderAsset();

		static ShaderAsset* Instance(AssetManager* manager, const String& name, const URL& location);
				
		inline uint16 GetTranslucency() const {
			return translucency;
		}

		inline uint16 GetShaderMask() const {
			return ((uint32)(this)) & SortKeyHelper::SHADER_KEY_MASK;
		}

		//uint16 GetTextureUnitIndex(const String& name) const;

		//const TexturePtr& GetDefaultTexture(const uint16 index) const;
		//const TextureUnitParams& GetTextureUnit(const uint16 index) const;

		virtual void Create(nextar::RenderContext*);
		virtual void Update(nextar::RenderContext*, ContextObject::UpdateParamPtr);
		virtual void Destroy(nextar::RenderContext*);

		virtual int GetType() const;

		//@ todo
		static ParamDef& MapParamName(const String& name);
		//@ todo
		static ParamDataType MapParamType(const String& typeName);
		
	protected:
		
		virtual void NotifyAssetLoaded();
		virtual void NotifyAssetUnloaded();
		virtual void NotifyAssetUpdated();
		
		virtual Pass* CreatePass(StreamPass&, const String& compilationOpt);
		virtual Pass* CreatePassImpl(const String& name) = 0;
		virtual bool Compile(nextar::RenderContext*) = 0;
		virtual void Decompile(nextar::RenderContext*) = 0;

		virtual void LoadImpl(StreamRequest* req, bool async = true);
		virtual void UnloadImpl(StreamRequest* req, bool async);
				
		virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);
		virtual void DestroyStreamRequestImpl(nextar::StreamRequest*&, bool load=true);

		void _UpdatePasses();

		// used as sort key
		uint8 translucency;
		
		PassList passes;

		friend class StreamRequest;
	};
} /* namespace nextar */
#endif /* SHADER_H_ */
