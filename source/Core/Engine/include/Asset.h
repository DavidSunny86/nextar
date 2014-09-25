#ifndef NEXTAR_ASSET_H
#define NEXTAR_ASSET_H

#include <NexEngine.h>
#include <PropertyInterface.h>
#include <Component.h>
#include <BackgroundStreamer.h>
#include <URL.h>
#include <ComponentFactoryArchive.h>
#include <ComponentGroupArchive.h>

namespace nextar {

class AssetCallback {
public:
	virtual void AssetUnloaded(Asset*) = 0;
	virtual void AssetLoaded(Asset*) = 0;
	virtual void AssetUpdated(Asset*) = 0;
	virtual void AssetSaved(Asset*) = 0;
protected:
	~AssetCallback() {
	}
};

class Asset;
class AssetStreamRequest;

typedef STDPoolAllocator<AssetStreamRequest*,
		(uint32) EngineConstants::NUM_STREAM_REQ_POINTER_PER_POOL_BLOCK,
		MEMCAT_GENERAL> AssetStreamRequestSetAllocator;
typedef STDPoolAllocator<Asset*,
		(uint32) EngineConstants::NUM_ASSET_POINTER_PER_POOL_BLOCK,
		MEMCAT_GENERAL> AssetSetAllocator;

typedef unordered_set<AssetStreamRequest*, std::hash<AssetStreamRequest*>,
		std::equal_to<AssetStreamRequest*>, AllocatorGeneral,
		AssetStreamRequestSetAllocator>::type AssetStreamRequestSet;
typedef unordered_set<Asset*, std::hash<Asset*>, std::equal_to<Asset*>,
		AllocatorGeneral, AssetSetAllocator>::type AssetSet;

/**
 * @class    Asset
 * Type of @Component, assets are basic game data like mesh, texture, sound, shaders, etc.
 **/
class _NexEngineAPI Asset: public PropertyInterface,
		public Streamable,
		public SharedComponent {
	NEX_LOG_HELPER(Asset)
	;

public:

	typedef SharedComponent::Factory Factory;
	typedef list<AssetCallback*>::type AssetCallbackList;
	typedef SharedComponent InstanceImplementor;

	enum Flags {
		BACKGROUND_STREAMED = Component::LAST_FLAG << 0,
		ASSET_LOADING = Component::LAST_FLAG << 1,
		ASSET_LOADED = Component::LAST_FLAG << 2,
		ASSET_LOAD_FAILED = Component::LAST_FLAG << 3,
		ASSET_READY = Component::LAST_FLAG << 4,
		ASSET_SAVING = Component::LAST_FLAG << 5,
		ASSET_SAVE_FAILED = Component::LAST_FLAG << 6,
		LAST_FLAG = Component::LAST_FLAG << 7,
	};

	struct AssetLocatorAccessor: public PropertyAccessor {
		static AssetLocatorAccessor assetLocatorAccessor;

		virtual void SetStringValue(PropertyInterface*, const String&);
		virtual const String GetStringValue(const PropertyInterface*) const;
	};

	template<typename AssetType>
	class AssetTraits : public ComponentTraits<AssetType> {
	public:

		typedef typename AssetType::InstanceImplementor InstanceImplementor;
		typedef RefPtr<AssetType> AssetTypePtr;

		inline static AssetTypePtr Instance(const ID& object) {
			return Instance(object.name, object.factory, object.group);
		}

		inline static AssetTypePtr Instance(const ID& object, const URL& locator) {
			return Instance(object.name, locator, object.factory, object.group);
		}

		inline static AssetTypePtr Instance(const StringID name,
				const StringID factory = StringUtils::DefaultID,
				const StringID group = StringUtils::DefaultID) {
			Group* groupPtr = nullptr;
			if (group != StringUtils::NullID)
				groupPtr = ComponentGroupArchive::Instance().AsyncFindOrCreate(
						group);
			SharedComponentPtr ret;
			InstanceImplementor::Instance(ret, CLASS_ID, name, factory,
								groupPtr);
			return ret;
		}

		inline static AssetTypePtr Instance(const StringID name,
				Component::Factory* factory,
				SharedComponent::Group* group) {
			SharedComponentPtr ret;
			InstanceImplementor::Instance(ret, CLASS_ID, name, factory, group);
			return ret;
		}

		inline static AssetTypePtr Instance(const StringID name,
				const URL& locator, Component::Factory* factory,
				SharedComponent::Group* group) {
			SharedComponentPtr asset;
			if(InstanceImplementor::Instance(asset, CLASS_ID, name,
					factory, group) == SharedComponent::INSTANCE_CREATED) {
				NEX_ASSERT(asset);
				static_cast<Asset*>(asset.GetPtr())->SetAssetLocator(locator);
			}
			return asset;
		}

		inline static AssetTypePtr Instance(const StringID name,
				const URL& locator, const StringID factory = StringUtils::DefaultID,
				const StringID group = StringUtils::DefaultID) {
			SharedComponentPtr asset;
			Group* groupPtr = nullptr;
			if (group != StringUtils::NullID)
				groupPtr = ComponentGroupArchive::Instance().AsyncFindOrCreate(
						group);
			if(InstanceImplementor::Instance(asset, CLASS_ID, name,
					factory, groupPtr) == SharedComponent::INSTANCE_CREATED) {
				NEX_ASSERT(asset);
				static_cast<Asset*>(asset.GetPtr())->SetAssetLocator(locator);
			}
			return asset;
		}
	};

	template<typename AssetType>
	class FactoryTraits: public Factory {
	public:
		typedef typename AssetType::Traits AssetTraits;
		typedef typename AssetTraits::AssetTypePtr AssetTypePtr;
		typedef FactoryTraits<AssetType> Type;
		inline FactoryTraits(const StringID name) : Factory(name) {}

		inline static Type* FindFactory(const StringID name) {
			return ComponentFactoryArchive::Instance().AsyncFindFactory(
					AssetTraits::CLASS_ID, name);
		}

		virtual AssetTypePtr AsyncCreateInstance(const StringID name,
				const URL& location) {
			AssetTypePtr asset = Assign(
					static_cast<AssetType*>(AsyncCreate(AssetTraits::CLASS_ID,
							name)));
			if (asset) {
				asset->SetAssetLocator(location);
			}
			return asset;
		}

		/* Implementation */
		virtual Component* AsyncCreate(uint32 classId, const StringID name) {
			if (classId == AssetTraits::CLASS_ID) {
				return NEX_NEW(AssetType(name, GetID()));
			}
			return nullptr;
		}

		inline static void _InternalRegisterToArchive() {
			ComponentFactoryArchive::Instance()._InternalDefaultFactory(NEX_NEW(Type(StringUtils::DefaultID)), AssetTraits::CLASS_ID);
		}
	};

	/**
	 * Can hold creation data. The background streamer will retr
	 **/
	class MetaInfo: public AllocGeneral {
	public:

		inline bool AddDependency(Asset* asset) {
			NEX_ASSERT(asset);
			return unresolvedDependencies.insert(asset).second;
		}

		inline uint32 RemoveDependency(Asset* asset) {
			unresolvedDependencies.erase(asset);
			return (uint32) unresolvedDependencies.size();
		}

		inline bool AddDependent(AssetStreamRequest* request) {
			NEX_ASSERT(request);
			return dependents.insert(request).second;
		}

		inline uint32 RemoveDependent(AssetStreamRequest* request) {
			dependents.erase(request);
			return (uint32) dependents.size();
		}

		inline uint32 GetDependencyCount() const {
			return (uint32) unresolvedDependencies.size();
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

	Asset(const StringID name, const StringID factory);
	virtual ~Asset();

	static ID ToID(const URL& url);
	/* Populate dictionary */
	static void Populate(PropertyDictionary* dict);

	// This should not be called from any thread other than main thread
	inline void SetSaving(bool b) {
		SetFlag(ASSET_SAVING, b);
	}

	// This should not be called from any thread other than main thread
	inline bool IsSaving() const {
		return (flags & ASSET_SAVING) != 0;
	}

	inline bool HasLoadFailed() const {
		return (flags & ASSET_LOAD_FAILED) != 0;
	}

	inline bool HasSaveFailed() const {
		return (flags & ASSET_SAVE_FAILED) != 0;
	}

	inline void SetLoadFailed(bool b) {
		SetFlag(ASSET_LOAD_FAILED, b);
	}

	inline void SetSaveFailed(bool b) {
		SetFlag(ASSET_SAVE_FAILED, b);
	}

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

	inline bool IsBackgroundStreamed() const {
		return (BACKGROUND_STREAMED & flags) != 0;
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

	inline void Save(bool useAsyncLoad) {
		Save(nullptr, useAsyncLoad);
	}

	virtual void Load(StreamRequest* request, bool useAsyncLoad);
	virtual void Save(StreamRequest* request, bool useAsyncLoad);
	virtual void Unload();
	/* Called from main thread when the asset has been unloaded */
	virtual void NotifyAssetUnloaded();
	/* Called from main thread when the asset has been updated */
	virtual void NotifyAssetUpdated();
	/* Called from main thread when the asset is loaded */
	void NotifyAssetLoaded();
	/* Called from main thread when the asset has been saved */
	void NotifyAssetSaved();
	/**
	 * When load is called, stream request is either created
	 * or retrieved from the asset implementation.
	 * */
	inline StreamRequest* GetStreamRequest(bool loadOrSave = true) {
		return (streamRequest) ? streamRequest : (streamRequest =
											CreateStreamRequestImpl(loadOrSave));
	}

	/* proxy objects when saved will return the object type it is saving via this call */
	virtual uint32 GetProxyID() const {
		return GetClassID();
	}
		
	// @remarks Used by the engine to initialize resources
	static AssetPtr AsyncLoad(const URL& input, const String& streamer = StringUtils::Null);
	static AssetPtr AsyncLoad(InputStreamPtr& input, const String& streamer);
	// @remarks Used by the engine to stream out resources
	static void AsyncSave(AssetPtr& asset, const URL& output, const String& streamer = StringUtils::Null);
	static void AsyncSave(AssetPtr& asset, OutputStreamPtr& output, const String& streamer);

protected:
	/* return true if the loading was completed */
	virtual bool NotifyAssetLoadedImpl();
	/* return true if the saving was completed */
	virtual bool NotifyAssetSavedImpl();

	/** Streamable */
	virtual void AsyncLoad(StreamRequest* req);
	virtual void AsyncSave(StreamRequest* req);

	/** @param isStreamed is true when the call is made from @Asset::AsyncLoad rather than @Asset::Load */
	virtual void LoadImpl(StreamRequest* req, bool isAsync);
	virtual void UnloadImpl() = 0;
	virtual void SaveImpl(StreamRequest* req, bool isAsync);

	virtual StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void DestroyStreamRequestImpl(StreamRequest*&, bool load = true);

	void _FireCallbacksLoaded();
	void _FireCallbacksUnloaded();
	void _FireCallbacksUpdated();
	void _FireCallbacksSaved();

	static void _LoadDependencies(AssetStreamRequest* req);

	//NEX_THREAD_MUTEX(assetLock);
	/* The asset locator */
	URL assetLocator;
	/* List of asset callbacks */
	AssetCallbackList callbacks;
	/* The asset memory used count in bytes */
	size_t memoryCost;
	/* Stream request object */
	StreamRequest* streamRequest;

private:
	enum {
		ASSET_HEADER = 0xa5e7,
	};

	friend class AssetStreamer;
};

class AssetStreamRequest: public StreamRequest {
public:

	inline AssetStreamRequest(Asset* asset,
			uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags | ASSET_STREAM_REQUEST)
			 {
		manualLoader = nullptr;
	}

	inline AssetStreamRequest(Asset* asset,
			AssetLoaderImpl* _manualLoader,
			uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags | ASSET_STREAM_REQUEST)
			 {
		manualLoader = _manualLoader;
	}

	inline AssetStreamRequest(Asset* asset,
			AssetSaverImpl* _manualSaver,
			uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags | ASSET_STREAM_REQUEST)
			 {
		manualSaver = _manualSaver;
	}

	inline void SetManualLoader(AssetLoaderImpl* manualLoader) {
		this->manualLoader = manualLoader;
	}

	inline void SetManualSaver(AssetSaverImpl* manualSaver) {
		this->manualSaver = manualSaver;
	}

	inline void SetInputStream(const InputStreamPtr& inputStream) {
		this->inputStream = inputStream;
	}

	inline void SetOutputStream(const OutputStreamPtr& outputStream) {
		this->outputStream = outputStream;
	}

	inline AssetLoaderImpl* GetManualLoader() const {
		return manualLoader;
	}

	inline AssetSaverImpl* GetManualSaver() const {
		return manualSaver;
	}

	inline const InputStreamPtr& GetInputStream() const {
		return inputStream;
	}

	inline const OutputStreamPtr& GetOutputStream() const {
		return outputStream;
	}

	inline Asset::MetaInfo& GetMetaInfo() {
		return metaInfo;
	}

	inline String GetName() const {
		Asset* asset = static_cast<Asset*>(GetStreamedObject());
		return asset->GetAssetLocator().ToString();
	}

	inline const String& GetParameterValue(const String& name) const {
		auto it = parameters.find(name);
		if (it != parameters.end())
			return (*it).second;
		return StringUtils::Null;
	}

	inline void SetParameterValue(const String& name, const String& value) {
		parameters[name] = value;
	}

protected:
	union {
		struct {
			AssetLoaderImpl* manualLoader;
			InputStreamPtr inputStream;
		};
		struct {
			AssetSaverImpl* manualSaver;
			OutputStreamPtr outputStream;
		};
	};
	NameValueMap parameters;
	Asset::MetaInfo metaInfo;
};

class AssetLoaderImpl {
public:

	virtual void Load(InputStreamPtr&, AssetLoader&) = 0;

protected:
	~AssetLoaderImpl() {
	}
};

class _NexEngineAPI AssetLoader: public AllocGeneral {

	NEX_LOG_HELPER(AssetLoader);
	NEX_DECLARE_COMPONENT_FACTORY(AssetLoaderImpl);

public:
	AssetLoader(AssetStreamRequest* req);
	~AssetLoader();

	inline AssetStreamRequest* GetRequestPtr() {
		return request;
	}

	void Serialize();

protected:
	AssetStreamRequest* request;
};

class AssetSaver;
class AssetSaverImpl {
public:

	virtual void Save(OutputStreamPtr&, AssetSaver&) = 0;

protected:
	~AssetSaverImpl() {
	}
};

class _NexEngineAPI AssetSaver: public AllocGeneral {

	NEX_LOG_HELPER(AssetLoader);
	NEX_DECLARE_COMPONENT_FACTORY(AssetSaverImpl);

public:
	AssetSaver(AssetStreamRequest* req);
	~AssetSaver();

	inline AssetStreamRequest* GetRequestPtr() {
		return request;
	}

	void Serialize();

protected:
	AssetStreamRequest* request;
};


}

#endif //NEXTAR_ASSET_H