#ifndef NEXTAR_ASSET_H
#define NEXTAR_ASSET_H

#include <NexEngine.h>
#include <PropertyInterface.h>
#include <Component.h>
#include <BackgroundStreamer.h>
#include <URL.h>


namespace nextar {
	
	class AssetCallback {
	public:
		virtual void AssetUnloaded(Asset*) = 0;
		virtual void AssetLoaded(Asset*) = 0;
		virtual void AssetUpdated(Asset*) = 0;
	protected:
		~AssetCallback() {}
	};

	class Asset;
	class AssetStreamRequest;

	typedef STDPoolAllocator<AssetStreamRequest*, (uint32)EngineConstants::NUM_STREAM_REQ_POINTER_PER_POOL_BLOCK, MEMCAT_GENERAL> AssetStreamRequestSetAllocator;
	typedef STDPoolAllocator<Asset*, (uint32)EngineConstants::NUM_ASSET_POINTER_PER_POOL_BLOCK, MEMCAT_GENERAL> AssetSetAllocator;

	typedef unordered_set<AssetStreamRequest*, std::hash<AssetStreamRequest*>, std::equal_to<AssetStreamRequest*>, AllocatorGeneral, AssetStreamRequestSetAllocator >::type AssetStreamRequestSet;
	typedef unordered_set<Asset*, std::hash<Asset*>, std::equal_to<Asset*>, AllocatorGeneral, AssetSetAllocator >::type AssetSet;

	/**
	 * @class    Asset
	 * Type of @Component, assets are basic game data like mesh, texture, sound, shaders, etc.
	 **/
	class _NexEngineAPI Asset:
	public PropertyInterface,
	public Streamable,
	public SharedComponent {
		NEX_LOG_HELPER(Asset);

	public:

		typedef SharedComponent::Factory Factory;
		typedef list<AssetCallback*>::type AssetCallbackList;

		enum Flags {
			BACKGROUND_LOADED = Component::LAST_FLAG << 0,
			ASSET_LOADING = Component::LAST_FLAG << 1,
			ASSET_LOADED = Component::LAST_FLAG << 2,
			ASSET_READY = Component::LAST_FLAG << 3,
			LAST_FLAG = Component::LAST_FLAG << 4,
		};

		struct AssetLocatorAccessor: public PropertyAccessor {
			static AssetLocatorAccessor assetLocatorAccessor;

			virtual void SetStringValue(PropertyInterface*, const String&);
			virtual const String GetStringValue(const PropertyInterface*) const;
		};

		/**
		 * Can hold creation data. The background streamer will retr
		 **/
		class MetaInfo : public AllocGeneral {
		public:

			inline bool AddDependency(Asset* asset) {
				NEX_ASSERT(asset);
				return unresolvedDependencies.insert(asset).second;
			}

			inline uint32 RemoveDependency(Asset* asset) {
				unresolvedDependencies.erase(asset);
				return (uint32)unresolvedDependencies.size();
			}

			inline bool AddDependent(AssetStreamRequest* request) {
				NEX_ASSERT(request);
				return dependents.insert(request).second;
			}

			inline uint32 RemoveDependent(AssetStreamRequest* request) {
				dependents.erase(request);
				return (uint32)dependents.size();
			}

			inline uint32 GetDependencyCount() const {
				return (uint32)unresolvedDependencies.size();
			}

			inline AssetStreamRequestSet& GetDependents() {
				return dependents;
			}

			inline AssetSet& GetDependencies() {
				return unresolvedDependencies;
			}

		protected:

			AssetStreamRequestSet dependents;
			AssetSet unresolvedDependencies;
		};

		Asset(const StringID);
		virtual ~Asset();

		/* Populate dictionary */
		static void Populate(PropertyDictionary* dict);

		// This should not be called from any thread other than main thread
		inline void SetLoading(bool b) {
			SetFlag(ASSET_LOADING, b);
		}

		// This should not be called from any thread other than main thread
		inline bool IsLoading() const {
			return (flags & ASSET_LOADING) != 0;
		}

		// This should not be called from any thread other than main thread
		inline void SetReady(bool b) {
			SetFlag(ASSET_READY, b);
		}

		// This should not be called from any thread other than main thread
		inline bool IsReady() const {
			return (flags & ASSET_READY) != 0;
		}

		// This should not be called from any thread other than main thread
		inline void SetLoaded(bool l) {
			SetFlag(ASSET_LOADED, true);
		}

		// This should not be called from any thread other than main thread
		inline bool IsLoaded() const {
			return (flags & ASSET_LOADED) != 0;
		}

		inline bool IsBackgroundLoaded() const {
			return (BACKGROUND_LOADED & flags) != 0;
		}

		inline size_t GetMemoryCost() const {
			return memoryCost;
		}

		inline void SetMemoryCost(size_t c) {
			memoryCost = c;
		}

		/** @remarks Returns the asset locator definition **/
		inline const URL& GetAssetLocator() const {
			return assetLocator;
		}

		/***/
		inline void SetAssetLocator(const URL& value) {
				assetLocator = value;
		}

		inline void AddAssetCallback(AssetCallback* assetCallback) {
			callbacks.push_back(assetCallback);
		}

		inline void RemoveAssetCallback(AssetCallback* assetCallback) {
			callbacks.remove(assetCallback);
		}

		inline void Load(bool useAsyncLoad) {
			Load(nullptr, useAsyncLoad);
		}

		inline void Unload(bool useAsyncUnload) {
			Unload(nullptr, useAsyncUnload);
		}
		
		virtual void Load(StreamRequest* request, bool useAsyncLoad);
		virtual void Unload(StreamRequest* request, bool useAsyncUnload);
		/* Called from main thread when the asset is loaded */
		virtual void NotifyAssetLoaded();
		/* Called from main thread when the asset has been unloaded */
		virtual void NotifyAssetUnloaded();
		/* Called from main thread when the asset has been updated */
		virtual void NotifyAssetUpdated();
		/**
		 * When load is called, stream request is either created
		 * or retrieved from the asset implementation.
		 * */
		inline StreamRequest* GetStreamRequest(bool load=true) {
			return (streamRequest) ? streamRequest : (streamRequest = CreateStreamRequestImpl(load));
		}

	protected:
		/** Streamable */
		virtual void AsyncLoad(StreamRequest* req);
		virtual void AsyncUnload(StreamRequest* req);

		/** @param isStreamed is true when the call is made from @Asset::AsyncLoad rather than @Asset::Load */
		virtual void LoadImpl(StreamRequest* req, bool isAsync) = 0;
		virtual void UnloadImpl(StreamRequest* req, bool isAsync) = 0;

		virtual StreamRequest* CreateStreamRequestImpl(bool load);
		virtual void DestroyStreamRequestImpl(StreamRequest*&);

		void _FireCallbacksLoaded();
		void _FireCallbacksUnloaded();
		void _FireCallbacksUpdated();

		static void _LoadDependencies(AssetStreamRequest* req);

		NEX_THREAD_MUTEX(assetLock);
		/* The asset locator */
		URL assetLocator;
		/* List of asset callbacks */
		AssetCallbackList callbacks;
		/* The asset memory used count in bytes */
		size_t memoryCost;
		/* Stream request object */
		StreamRequest* streamRequest;

	private:
		friend class AssetStreamer;
	};

	class AssetStreamRequest : public StreamRequest {
	public:

		inline AssetStreamRequest(Asset* asset, uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags|ASSET_STREAM_REQUEST) {
		}

		inline Asset::MetaInfo& GetMetaInfo() {
			return metaInfo;
		}

	protected:
		Asset::MetaInfo metaInfo;
	};

}

#endif //NEXTAR_ASSET_H
