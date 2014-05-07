/*
 * Material.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Asset.h>
#include <ShaderParam.h>
#include <ShaderAsset.h>
#include <ParameterBuffer.h>

namespace nextar {


	/* Non automatic parameters for shaders and shader options
	 * and the compiled shader technique reference stored here.
	 */
	class _NexEngineAPI MaterialAsset: public nextar::Asset {
	public:
		
		enum Type {
			CLASS_ID = Component::CLASS_ASSET_MATERIAL,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};

		class Loader;
		struct LoaderImpl;
		class StreamRequest;

		
		class Factory: public Asset::Factory {
		protected:

		public:
			Factory(const StringID name);
			
			/* Implementation */
			virtual Component* AsyncCreate(uint32 type, const StringID name);
		};


		struct LoaderImpl {
			virtual void Load(InputStreamPtr& input, Loader& shader) = 0;
		};

		class Loader {
			NEX_LOG_HELPER(Loader);
			NEX_DECLARE_FACTORY(LoaderImpl);

		public:
			Loader(nextar::StreamRequest* material);
			~Loader();

			inline nextar::StreamRequest* GetRequestPtr() {
				return requestPtr;
			}

			void Serialize();

		protected:
			nextar::StreamRequest* requestPtr;
		};

		class StreamRequest : public AllocGeneral,
		public AssetStreamRequest {
			NEX_LOG_HELPER(Shader::StreamRequest);
		public:

			StreamRequest(Asset*);
			~StreamRequest();

			/** @options should be a string of comma separated indices */
			void SetShader(StringID name, const URL& location, StringID group  = StringUtils::DefaultID,
					 StringID factory = StringUtils::DefaultID);
			void SetParamBufferSize(uint32 paramBufferSize);
			void SetParamValue(uint32 offset, const void* data, size_t amount);
			void SetTextureValue(uint32 offset, const TextureUnit* texture);
			void SetRenderLayer(uint8 layer);

		protected:
			friend class MaterialAsset;
			String optionHash;
		};

		MaterialAsset(const StringID name);
		virtual ~MaterialAsset();

		inline const ShaderAssetPtr& GetShader() const {
			return shader;
		}

		inline ShaderAssetPtr& GetShader() {
			return shader;
		}

		inline uint8 GetLayerMask() const {
			return layerMask;
		}

		inline uint8 GetRenderLayer() const {
			return renderLayer;
		}

		inline void SetRenderLayer(uint8 l) {
			renderLayer = l;
		}

		static MaterialAssetPtr Instance(MaterialAsset::Factory* factory, const StringID name, const URL& location);

		virtual uint32 GetClassID() const;

	protected:
		/* notify loaded/unloaded */
		virtual void NotifyAssetLoaded();
		virtual void NotifyAssetUnloaded();
		virtual void NotifyAssetUpdated();

		virtual void LoadImpl(nextar::StreamRequest* req, bool isStreamed);
		virtual void UnloadImpl(nextar::StreamRequest* req, bool isStreamed);

		virtual nextar::StreamRequest* CreateStreamRequestImpl(bool load);
		virtual void DestroyStreamRequestImpl(nextar::StreamRequest*&, bool load=true);

		void _PrepareMaterial(MaterialAsset::StreamRequest*);
		void _SetParamBufferSize(size_t size);
		void _SetParamData(const void* data, size_t offset, size_t size);

		// used as sort key
		uint8 layerMask;
		uint8 renderLayer;

		ShaderAssetPtr shader;
		String options;

		//ConstantBufferPtr materialParameters;
		ParameterBuffer materialParamData;

		friend class StreamRequest;
	};

} /* namespace nextar */
#endif /* MATERIAL_H_ */
