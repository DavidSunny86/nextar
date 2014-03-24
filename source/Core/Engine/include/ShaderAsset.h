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
#include <GpuProgram.h>
#include <ParameterBuffer.h>
#include <ParameterIterator.h>

namespace nextar {

	class SceneParameters;
	/**
	 * ShaderUI : Shader {
	 * 	ShaderPtr compiledShader;
	 * }
	 */
	class _NexEngineAPI ShaderAsset: public nextar::Asset,
	public ContextObject {
		NEX_LOG_HELPER(ShaderAsset);
	public:

		enum {
			CLASS_ID = Asset::CLASS_ASSET_SHADER,
		};

		enum {
			USE_FALLBACK = Asset::LAST_FLAG << 5,
		};

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
			Pass::TextureDescMap textureStates;
			//Pass::DefaultTextureUnitMap defaultTextureUnits;
		};

		// todo Make this a list not a vector
		typedef vector<StreamPass>::type StreamPassList;

		class StreamRequest : public AllocGeneral,
			public AssetStreamRequest {
			NEX_LOG_HELPER(ShaderAsset::StreamRequest);
		public:
			StreamRequest(ShaderAsset*);
			~StreamRequest();

			/* all parameters which are not auto type will have update frequency of material, which means they will
			 * be updated per material (using the material property table). */
			// Parameters are registered here for UI specific display (or when meta data has to be stored).
			// The parameter name is of the form: Parameter Category:Parameter Name. The category, if ignored,
			// will be set to General.
			virtual void AddParam(const String& name,
					const String& param, const String& description, const String& defaultValue, ParamDataType type);
			virtual void AddMacro(const String& name,
					const String& param, const String& description, bool defualtState);
			/* Add Pass */
			virtual void AddPass(const String& name);
			/* Set pass paramter buffer data */
			virtual void SetParamterBuffer(const ParameterBuffer&& data);
			/* Pass related */
			void SetOptions(const String& options);
			void SetProgramSource(GpuProgram::Type, const String& src);
			void SetRasterState(RasterState& state);
			void SetBlendState(BlendState& state);
			void SetDepthStencilState(DepthStencilState& state);
			// If called multiple times for the same unit, the texture will be appended in a
			// list and if the sampler is an array
			void BindDefaultTexture(const String& unitName, TextureBase* texture, uint32 index);
			void AddTextureUnit(const String& unitName, uint32 arrayCount, TextureUnitParams& unit);

		protected:

			friend class ShaderAsset;
			uint32 currentPass;
			StreamPassList passes;

			String compilationOpt;
		};

		ShaderAsset(const StringID);
		virtual ~ShaderAsset();

		static ShaderAssetPtr Instance(Component::Factory* factory, const StringID name, const URL& location,
				SharedComponent::Group* group = nullptr);

		inline uint16 GetShaderMask() const {
			return ((std::ptrdiff_t)(this)) & (uint32)SortKeyHelper::SHADER_KEY_MASK;
		}

		inline uint16 GetTranslucency() const {
			return translucency;
		}

		ParameterIterator GetParameterIterator(uint32 type);

		virtual void Create(nextar::RenderContext*);
		virtual void Update(nextar::RenderContext*, ContextObject::UpdateParamPtr);
		virtual void Destroy(nextar::RenderContext*);

		virtual uint32 GetClassID() const override;

		// todo
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

		void _Process(ShaderParamIterator& it, ParamTableBuilder& ptb);
		void _BeginPass(PassPtr& p, ParamTableBuilder& ptb);
		void _Finalize(ParamTableBuilder& ptb);
		void _EndPass(PassPtr& p, ParamTableBuilder& ptb);
		void _BuildParameterTable();

		// used as sort key
		uint8 translucency;
		PassList passes;
		// required when parameter is looked up by name
		ParamEntryTable paramLookup;

		ParamEntryTableItem passProperties;
		ParamEntryTableItem materialProperties;
		ParamEntryTableItem objectProperties;

		ParameterBuffer passParamData;

		friend class StreamRequest;
	};
} /* namespace nextar */
#endif /* SHADER_H_ */
