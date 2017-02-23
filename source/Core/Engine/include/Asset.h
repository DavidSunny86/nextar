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

struct AssetStreamerInfo {
	union {
		AssetLoaderImpl* manualLoader;
		AssetSaverImpl* manualSaver;
	};

	InputStreamPtr inputStream;
	OutputStreamPtr outputStream;
};

struct _NexEngineAPI StreamInfo {
	static const StreamInfo Null;
	AssetStreamRequest* externalRequest;
	URL locator; // in case the locator is not set yet
	AssetStreamerInfo manualStreamer;

	StreamInfo() : externalRequest(nullptr) {
		manualStreamer.manualLoader = nullptr;
	}

	StreamInfo(AssetLoaderImpl* loader, const InputStreamPtr& streamPtr) : externalRequest(nullptr) {
		manualStreamer.manualLoader = loader;
		manualStreamer.inputStream = streamPtr;
	}

	StreamInfo(AssetSaverImpl* saver, const OutputStreamPtr& streamPtr) : externalRequest(nullptr) {
		manualStreamer.manualSaver = saver;
		manualStreamer.outputStream = streamPtr;
	}
};

enum StreamNotification : uint32 {
	NOTIFY_READY = 1 << 0,
	NOTIFY_COMPLETED = 1 << 1,
	NOTIFY_RESUBMIT = 1 << 2,
	NOTIFY_FAILED = 1 << 3,
	NOTIFY_COMPLETED_AND_READY = NOTIFY_COMPLETED|NOTIFY_READY,
	NOTIFY_RESUBMIT_AND_READY = NOTIFY_RESUBMIT|NOTIFY_READY,
};

NEX_ENUM_FLAGS(StreamNotification, uint32);

class Asset;
class AssetStreamRequest;

typedef STDPoolAllocator<AssetStreamRequest*,
	(uint32)EngineConstants::NUM_POINTER_PER_POOL_BLOCK,
		MEMCAT_GENERAL> AssetStreamRequestSetAllocator;
typedef STDPoolAllocator<Asset*,
	(uint32)EngineConstants::NUM_POINTER_PER_POOL_BLOCK,
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

	enum State : uint32 {
		ASSET_CREATED = 1 << 0,
		ASSET_LOADINFO_REG = 1 << 1,
		ASSET_LOADING = 1 << 2,
		ASSET_LOADED = 1 << 3,
		ASSET_LOAD_FAILURE = 1 << 4,
		ASSET_READY = ASSET_LOADED|(1 << 5),
		ASSET_SAVEINFO_REG = ASSET_READY|(1 << 6),
		ASSET_SAVING = ASSET_READY|(1 << 7),
		ASSET_UNLOADING = 1 << 8,
	};

	enum Flags {
		BACKGROUND_STREAMED = Component::LAST_FLAG << 0,
		LAST_FLAG = Component::LAST_FLAG << 1,
	};

	class AssetLocatorAccessor: public PropertyAccessor {
	public:
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
			InstanceImplementor::Instance(ret, AssetType::CLASS_ID, name, factory,
								groupPtr);
			return ret;
		}

		inline static AssetTypePtr Instance(const StringID name,
				Component::Factory* factory,
				SharedComponent::Group* group) {
			SharedComponentPtr ret;
			InstanceImplementor::Instance(ret, AssetType::CLASS_ID, name, factory, group);
			return ret;
		}

		inline static AssetTypePtr Instance(const StringID name,
				const URL& locator, Component::Factory* factory,
				SharedComponent::Group* group) {
			SharedComponentPtr asset;
			if(InstanceImplementor::Instance(asset, AssetType::CLASS_ID, name,
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
			if(InstanceImplementor::Instance(asset, AssetType::CLASS_ID, name,
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
	class _NexEngineAPI MetaInfo : public AllocGeneral {
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

	virtual void Unload();
	/* Called from main thread when the asset has been unloaded */
	virtual void NotifyAssetUnloaded();
	/* Called from main thread when the asset has been updated */
	virtual void NotifyAssetUpdated();
	/* Called from main thread when the asset is loaded */
	void NotifyAssetLoaded();
	/* Called from main thread when the asset has been saved */
	void NotifyAssetSaved();

	/* proxy objects when saved will return the object type it is saving via this call */
	virtual uint32 GetProxyID() const {
		return GetClassID();
	}

	void AddDependent(AssetStreamRequest* request);
	bool AsyncHasLoadFailed() const {
		return (assetState.load(std::memory_order_acquire) & ASSET_LOAD_FAILURE) != 0;
	}

	bool AsyncIsLoaded() const {
		return (assetState.load(std::memory_order_acquire) & ASSET_LOADED) != 0;
	}

	bool AsyncIsLoading() const {
		return (assetState.load(std::memory_order_acquire) & ASSET_LOADING) != 0;
	}

	bool AsyncIsSaving() const {
		return (assetState.load(std::memory_order_acquire) & ASSET_SAVING) != 0;
	}

	bool AsyncIsReady() const {
		return (assetState.load(std::memory_order_acquire) & ASSET_READY) != 0;
	}

	bool AsyncIsCreated() const {
		return (assetState.load(std::memory_order_acquire) & ASSET_CREATED) != 0;
	}

	// @remarks call to blocking load
	// @returns false if the asset is not in a state when it can be loaded
	bool RequestLoad(const StreamInfo& request = StreamInfo::Null) {
		return AsyncLoad(request, false);
	}
	// @remarks call to blocking save 
	// @returns false if the asset is not in a state when it can be saved
	bool RequestSave(const StreamInfo& request = StreamInfo::Null) {
		return AsyncSave(request, false);
	}
	// @remarks Places a load request, and returns true if the request was placed
	// This should not be called by loader implementations. Loaders should
	// just add dependencies.
	// @returns false if the asset is not in a state when it can be loaded
	bool AsyncRequestLoad(const StreamInfo& request = StreamInfo::Null) {
		return AsyncLoad(request, true);
	}
	// @remarks Places a save request, and returns true if the request was placed.
	// This should not be called by saver implementations. 
	// @returns false if the asset is not in a state when it can be saved
	bool AsyncRequestSave(const StreamInfo& request = StreamInfo::Null) {
		return AsyncSave(request, true);
	}
	// @remarks load and save cannot be called from a streaming thread,
	// however, since loading might require to set streaming info
	// this function can be used for the same
	bool AsyncSetLoadInfo(const StreamInfo& request);
	// @remarks load and save cannot be called from a streaming thread,
	// however, since loading might require to set streaming info
	// this function can be used for the same
	bool AsyncSetSaveInfo(const StreamInfo& request);
	// @remarks Used by the engine to initialize resources
	static AssetPtr AssetLoad(const URL& input, const String& streamer = StringUtils::Null);
	static AssetPtr AssetLoad(InputStreamPtr& input, const String& streamer);
	// @remarks Used by the engine to stream out resources
	static void AssetSave(AssetPtr& asset, const URL& output, const String& streamer);
	static void AssetSave(AssetPtr& asset, OutputStreamPtr& output, const String& streamer);


protected:

	
	virtual StreamRequest* CreateStreamRequestImpl(bool load) = 0;

	/* return true if the loading was completed */
	virtual StreamNotification NotifyAssetLoadedImpl(StreamRequest* request);
	/* return true if the saving was completed */
	virtual StreamNotification NotifyAssetSavedImpl(StreamRequest* request);

	bool AsyncLoad(const StreamInfo& request, bool async);
	bool AsyncSave(const StreamInfo& request, bool async);

	/** Streamable */
	virtual void AsyncLoad(StreamRequest* req);
	virtual void AsyncSave(StreamRequest* req);

	/** @param isStreamed is true when the call is made from @Asset::AsyncLoad rather than @Asset::Load */
	virtual void LoadImpl(StreamRequest* req, bool isAsync);
	virtual void UnloadImpl() = 0;
	virtual void SaveImpl(StreamRequest* req, bool isAsync);

	void _FireCallbacksLoaded();
	void _FireCallbacksUnloaded();
	void _FireCallbacksUpdated();
	void _FireCallbacksSaved();

	static void _LoadDependencies(AssetStreamRequest* req);

	// async state
	std::atomic<State> assetState;
	// end async state

	//NEX_THREAD_MUTEX(assetLock);
	/* The asset locator */
	URL assetLocator;
	/* List of asset callbacks */
	AssetCallbackList callbacks;
	/* The asset memory used count in bytes */
	size_t memoryCost;
	/* Stream request object */


private:
	StreamRequest* _savedRequestPtr;
	enum {
		ASSET_HEADER = 0xa5e7,
	};

	friend class AssetStreamer;
};

class _NexEngineAPI AssetStreamRequest : public StreamRequest {
public:

	inline AssetStreamRequest(Asset* asset,
			uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags | ASSET_STREAM_REQUEST)
			 {
		manualStreamer.manualLoader = nullptr;
	}

	inline AssetStreamRequest(Asset* asset,
			AssetLoaderImpl* _manualLoader,
			uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags | ASSET_STREAM_REQUEST)
			 {
		manualStreamer.manualLoader = _manualLoader;
	}

	inline AssetStreamRequest(Asset* asset,
			AssetSaverImpl* _manualSaver,
			uint16 exFlags = AUTO_DELETE_REQUEST) :
			StreamRequest(asset, exFlags | ASSET_STREAM_REQUEST)
			 {
		manualStreamer.manualSaver = _manualSaver;
	}

	inline void SetManualLoader(AssetLoaderImpl* manualLoader) {
		this->manualStreamer.manualLoader = manualLoader;
	}

	inline void SetManualSaver(AssetSaverImpl* manualSaver) {
		this->manualStreamer.manualSaver = manualSaver;
	}

	inline void SetInputStream(const InputStreamPtr& inputStream) {
		this->manualStreamer.inputStream = inputStream;
	}

	inline void SetOutputStream(const OutputStreamPtr& outputStream) {
		this->manualStreamer.outputStream = outputStream;
	}

	inline AssetLoaderImpl* GetManualLoader() const {
		return manualStreamer.manualLoader;
	}

	inline AssetSaverImpl* GetManualSaver() const {
		return manualStreamer.manualSaver;
	}

	inline const InputStreamPtr& GetInputStream() const {
		return manualStreamer.inputStream;
	}

	inline const OutputStreamPtr& GetOutputStream() const {
		return manualStreamer.outputStream;
	}

	inline Asset::MetaInfo& GetMetaInfo() {
		return metaInfo;
	}

	inline String GetName() const {
		Asset* asset = static_cast<Asset*>(GetStreamedObject());
		return asset->GetAssetLocator().ToString();
	}

	inline void SetCompleted(bool status) {
		returnCode = status ? StreamResult::STREAM_SUCCESS : StreamResult::STREAM_FAILED;
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

	inline void SetStreamerInfo(const AssetStreamerInfo& info) {
		manualStreamer = info;
	}

	inline void SetAssetLocator(const URL& locator) {
		this->locator = locator;
	}

	inline const URL& GetAssetLocator() const {
		return locator;
	}

	inline uint32 RemoveDependency(Asset* asset) {
		return metaInfo.RemoveDependency(asset);
	}

protected:
	URL locator;
	AssetStreamerInfo manualStreamer;
	NameValueMap parameters;
	Asset::MetaInfo metaInfo;
};

class _NexEngineAPI AssetLoaderImpl {
public:

	virtual void PreLoadFailed(AssetLoader& assetLoader) {}
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

	virtual void PreSaveFailed(AssetSaver&) {}
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
