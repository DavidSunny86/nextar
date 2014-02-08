/*
 * Material.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Asset.h"
#include "ShaderParam.h"

namespace nextar {

	class MaterialAssetManager : public nextar::AssetManager
	{
	public:
		MaterialAssetManager(const String&);
		virtual ~MaterialAssetManager(void);

	protected:

		virtual Component* AsyncCreateImpl(int type, const String& name);
	};

	/* Non automatic parameters for shaders and shader options
	 * and the compiled shader technique reference stored here.
	 */
	class _NexExport MaterialAsset: public nextar::Asset {
	public:
		
		enum Type {
			CLASS_ID = Component::CLASS_ASSET_MATERIAL,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};
		typedef ByteStream PropertyBuffer;
		typedef unordered_map<String, ShaderVariant>::type ShaderParamMap;

		class Loader;
		struct LoaderImpl;
		class StreamRequest;

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
			void SetShader(const String& name, const String& options, const URL& location);
			void AddParameter(const String& name, ShaderVariant&& swapValue);

		protected:
			friend class MaterialAsset;
			ShaderParamMap params;
		};

		MaterialAsset(MaterialAssetManager* manager, const String& name);
		virtual ~MaterialAsset();

		inline const ShaderAssetPtr& GetShader() const {
			return shader;
		}

		inline uint8 GetLayerMask() const {
			return layerMask;
		}

		inline uint8 GetRenderLayer() const {
			return renderLayer;
		}

		static MaterialAsset* Instance(AssetManager* manager, const String& name, const URL& location);

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

		void _PrepareParamData(const ShaderParamMap&);
		void _PrepareMaterial(MaterialAsset::StreamRequest*);
		void _PreparePropertyBuffer();
		void _RelievePropertyBuffer();

		// used as sort key
		uint8 layerMask;
		uint8 renderLayer;

		ShaderAssetPtr shader;
		String options;

		PropertyBuffer propertyBuffer;
				
	};

} /* namespace nextar */
#endif /* MATERIAL_H_ */
