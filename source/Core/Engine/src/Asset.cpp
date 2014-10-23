#include <NexEngine.h>
#include <Asset.h>
#include <AssetStreamer.h>

namespace nextar {

Asset::AssetLocatorAccessor Asset::AssetLocatorAccessor::assetLocatorAccessor;
const StreamInfo StreamInfo::Null;

Asset::Asset(const StringID id, const StringID factory) :
<<<<<<< HEAD
	memoryCost(sizeof(Asset)), SharedComponent(id, factory), _savedRequestPtr(nullptr) {
	assetState = ASSET_CREATED;
=======
	memoryCost(sizeof(Asset)), SharedComponent(id, factory), _savedRequestPtr(nullptr),
	assetState(ASSET_CREATED) {
	
>>>>>>> 71b15a1f10310ca15601df42ebede165149705cc
	if (OverrideDictionary("Asset")) {
		Populate(GetDictionary());
	}
}

Asset::~Asset() {
}

Asset::ID Asset::ToID(const URL& url) {
	Asset::ID id;
	id.name = NamedObject::AsyncStringID(url.GetComputedName());
	return id;
}

void Asset::Populate(PropertyDictionary* dict) {
	NEX_ASSERT(dict->GetName() == "Asset");
	dict->AddParam("location", &AssetLocatorAccessor::assetLocatorAccessor,
			PROP_TYPE_STRING, "Locates an asset via file identifier.");
}

void Asset::AssetLocatorAccessor::SetStringValue(PropertyInterface* pi,
		const String& value) {
	Asset* asset = static_cast<Asset*>(pi);
	asset->SetAssetLocator(value);
}

const String Asset::AssetLocatorAccessor::GetStringValue(
		const PropertyInterface* pi) const {
	const Asset* asset = static_cast<const Asset*>(pi);
	return asset->GetAssetLocator().ToString();
}

bool Asset::AsyncLoad(const StreamInfo& request, bool async) {
	//NEX_THREAD_LOCK_GUARD_MUTEX(assetLock);
	State expected = ASSET_CREATED;
	if (assetState.compare_exchange_strong(expected,
		ASSET_LOADING, std::memory_order_release,
		std::memory_order_relaxed)) {
		// only a single thread can reach here,
		// if we have, we issue the draw request
		_savedRequestPtr = nullptr;
		if (request.externalRequest)
			_savedRequestPtr = request.externalRequest;
		else if (!_savedRequestPtr)
			_savedRequestPtr = CreateStreamRequestImpl(true);

		if (_savedRequestPtr->flags & StreamRequest::ASSET_STREAM_REQUEST) {
			AssetStreamRequest* assetRequest = static_cast<AssetStreamRequest*>(_savedRequestPtr);
			assetRequest->SetStreamerInfo(request.manualStreamer);
			// @todo potentially dangerous because this value might
			// be written from outside using SetAssetLocator, when
			// GetAssetLocator is called, may be guard the calls using a spinlock
			if (request.locator != URL::Invalid)
				assetRequest->SetAssetLocator(request.locator);
			else
				assetRequest->SetAssetLocator(GetAssetLocator());
		}

		if (async) {
			if (IsBackgroundStreamed() &&
				(_savedRequestPtr->flags & StreamRequest::ASSET_STREAM_REQUEST)) {
				AssetStreamer::Instance().AsyncRequestLoad(static_cast<AssetStreamRequest*>(_savedRequestPtr));
			} else {
				Warn("Asset cannot be background loaded!");
			}
		} else {
			try {
				LoadImpl(_savedRequestPtr, false);
				if (_savedRequestPtr->flags & StreamRequest::ASSET_STREAM_REQUEST)
					_LoadDependencies(
					static_cast<AssetStreamRequest*>(_savedRequestPtr));
			} catch (GracefulErrorExcept& e) {
				Debug(e.GetMsg());
				// @todo Combine the flags set/unset
				_savedRequestPtr->returnCode = StreamResult::STREAM_FAILED;
			}
			NotifyAssetLoaded();
		}
		return true;
	}
	return false;
}

void Asset::NotifyAssetLoaded() {
	bool resubmitRequest = false;
	if (!_savedRequestPtr || (_savedRequestPtr->returnCode
		== StreamResult::STREAM_SUCCESS)) {
		assetState.store(ASSET_LOADED, std::memory_order_release);
		StreamNotification state = NotifyAssetLoadedImpl(_savedRequestPtr);
		if (state & NOTIFY_COMPLETED) 
			_savedRequestPtr->flags |= StreamRequest::COMPLETED;
		if (state & NOTIFY_READY)
			assetState.store(ASSET_READY, std::memory_order_release);
		if (state & NOTIFY_RESUBMIT)
			resubmitRequest = true;
		else if (state & NOTIFY_FAILED) {
			_savedRequestPtr->flags |= StreamRequest::COMPLETED;
			assetState.store(ASSET_LOAD_FAILURE, std::memory_order_release);
		}
	} else if (_savedRequestPtr->returnCode == StreamResult::STREAM_FAILED) {
		_savedRequestPtr->flags |= StreamRequest::COMPLETED;
		assetState.store(ASSET_LOAD_FAILURE, std::memory_order_release);
	}
	_FireCallbacksLoaded();
	if (_savedRequestPtr) {
		if (resubmitRequest)
			AssetStreamer::Instance().AsyncRequestLoad(static_cast<AssetStreamRequest*>(_savedRequestPtr));
		else {
			uint16 reqflags = _savedRequestPtr->flags;
			if ((reqflags
				& (StreamRequest::AUTO_DELETE_REQUEST | StreamRequest::COMPLETED))
				== (StreamRequest::AUTO_DELETE_REQUEST
				| StreamRequest::COMPLETED))
				NEX_DELETE(_savedRequestPtr);
			_savedRequestPtr = nullptr;
		}
	}
}

bool Asset::AsyncSave(const StreamInfo& request, bool async) {
	//NEX_THREAD_LOCK_GUARD_MUTEX(assetLock);
	State expected = ASSET_READY;
	if (assetState.compare_exchange_strong(expected,
		ASSET_SAVING, std::memory_order_release,
		std::memory_order_relaxed)) {
		// only a single thread can reach here,
		// if we have, we issue the draw request
		_savedRequestPtr = nullptr;
		if (request.locator != URL::Invalid)
			SetAssetLocator(request.locator);
		if (request.externalRequest)
			_savedRequestPtr = request.externalRequest;
		else if (!_savedRequestPtr)
			_savedRequestPtr = CreateStreamRequestImpl(true);

		if (_savedRequestPtr->flags & StreamRequest::ASSET_STREAM_REQUEST) {
			AssetStreamRequest* assetRequest = static_cast<AssetStreamRequest*>(_savedRequestPtr);
			assetRequest->SetStreamerInfo(request.manualStreamer);
			// @todo potentially dangerous because this value might
			// be written from outside using SetAssetLocator, when
			// GetAssetLocator is called, may be guard the calls using a spinlock
			if (request.locator != URL::Invalid)
				assetRequest->SetAssetLocator(request.locator);
			else
				assetRequest->SetAssetLocator(GetAssetLocator());
		}

		if (async) {
			if (IsBackgroundStreamed() &&
				(_savedRequestPtr->flags & StreamRequest::ASSET_STREAM_REQUEST)) {
				AssetStreamer::Instance().AsyncRequestSave(static_cast<AssetStreamRequest*>(_savedRequestPtr));
			}
			else {
				Warn("Asset cannot be background saved!");
			}
		}
		else {
			try {
				SaveImpl(_savedRequestPtr, false);
			}
			catch (GracefulErrorExcept& e) {
				Debug(e.GetMsg());
				// @todo Combine the flags set/unset
				_savedRequestPtr->returnCode = StreamResult::STREAM_FAILED;
			}
			NotifyAssetSaved();
		}
		return true;
	}
	return false;
}

void Asset::NotifyAssetSaved() {
	bool resubmitRequest = false;
	if (!_savedRequestPtr || (_savedRequestPtr->returnCode == StreamResult::STREAM_SUCCESS)) {
		assetState.store(ASSET_READY, std::memory_order_release);
		StreamNotification state = NotifyAssetSavedImpl(_savedRequestPtr);
		if (state & NOTIFY_COMPLETED)
			_savedRequestPtr->flags |= StreamRequest::COMPLETED;
		else if (state & NOTIFY_RESUBMIT)
			resubmitRequest = true;
	} else if (_savedRequestPtr->returnCode == StreamResult::STREAM_FAILED) {
		_savedRequestPtr->flags |= StreamRequest::COMPLETED;
	}

	_FireCallbacksSaved();
	if (_savedRequestPtr) {
		if (resubmitRequest)
			AssetStreamer::Instance().AsyncRequestSave(static_cast<AssetStreamRequest*>(_savedRequestPtr));
		else {
			uint16 reqflags = _savedRequestPtr->flags;
			if ((reqflags
				& (StreamRequest::AUTO_DELETE_REQUEST | StreamRequest::COMPLETED))
				== (StreamRequest::AUTO_DELETE_REQUEST
				| StreamRequest::COMPLETED))
				NEX_DELETE(_savedRequestPtr);
			_savedRequestPtr = nullptr;
		}
	}
}

void Asset::Unload() {
	//NEX_THREAD_LOCK_GUARD_MUTEX(assetLock);
	State expected = ASSET_READY;
	if (assetState.compare_exchange_strong(expected,
		ASSET_UNLOADING, std::memory_order_release,
		std::memory_order_relaxed)) {
		try {
			UnloadImpl();
			NotifyAssetUnloaded();
		}
		catch (GracefulErrorExcept& e) {
			Debug(e.GetMsg());
		}

		assetState.store(ASSET_CREATED, std::memory_order_release);
	}
}

void Asset::NotifyAssetUnloaded() {
}

void Asset::NotifyAssetUpdated() {
}

StreamNotification Asset::NotifyAssetLoadedImpl(StreamRequest*) {
	return StreamNotification::NOTIFY_READY | StreamNotification::NOTIFY_COMPLETED;
}

StreamNotification Asset::NotifyAssetSavedImpl(StreamRequest*) {
	return StreamNotification::NOTIFY_READY | StreamNotification::NOTIFY_COMPLETED;
}

void Asset::AsyncLoad(StreamRequest* request) {
	try {
		LoadImpl(request, true);
	} catch (GracefulErrorExcept& e) {
		Debug(e.GetMsg());
		request->returnCode = StreamResult::STREAM_FAILED;
	}
}

void Asset::AsyncSave(StreamRequest* request) {
	try {
		SaveImpl(request, true);
	} catch (GracefulErrorExcept& e) {
		Debug(e.GetMsg());
		request->returnCode = StreamResult::STREAM_FAILED;
	}
}

void Asset::LoadImpl(nextar::StreamRequest* request, bool) {
	AssetLoader loader(static_cast<AssetStreamRequest*>(request));
	loader.Serialize();
}

void Asset::SaveImpl(nextar::StreamRequest* request, bool) {
	AssetSaver saver(static_cast<AssetStreamRequest*>(request));
	saver.Serialize();
}

StreamRequest* Asset::CreateStreamRequestImpl(bool) {
	return NEX_ALLOC_INIT_T(AssetStreamRequest, MEMCAT_GENERAL, this);
}

void Asset::_FireCallbacksLoaded() {
	for (auto callback : callbacks) {
		callback->AssetLoaded(this);
	}
}

void Asset::_FireCallbacksUnloaded() {
	for (auto callback : callbacks) {
		callback->AssetUnloaded(this);
	}
}

void Asset::_FireCallbacksUpdated() {
	for (auto callback : callbacks) {
		callback->AssetUpdated(this);
	}
}

void Asset::_FireCallbacksSaved() {
	for (auto callback : callbacks) {
		callback->AssetSaved(this);
	}
}

void Asset::_LoadDependencies(AssetStreamRequest* req) {
	AssetSet& s = req->GetMetaInfo().GetDependencies();
	for (auto a : s) {
		a->RequestLoad();
		if (a->AsyncHasLoadFailed()) {
			req->returnCode = StreamResult::STREAM_FAILED;
			return;
		}
	}
}

AssetPtr Asset::AssetLoad(const URL& input, const String& streamer) {
	InputStreamPtr inputStream = FileSystem::Instance().OpenRead(input);
	if (inputStream)
		return AssetLoad(inputStream, streamer.length() ? streamer : input.GetExtension());
	return AssetPtr();
}

AssetPtr Asset::AssetLoad(InputStreamPtr& input, const String& streamer) {
	ChunkInputStream ser(input);
	InputSerializer::Chunk c = InputSerializer::Invalid;

	ser.ReadChunk(ASSET_HEADER, c);
	if (InputSerializer::IsValid(c)) {
		uint32 classId;
		Asset::ID id;
		ser >> classId >> id;
		SharedComponentPtr oInst;
		Group* groupPtr = nullptr;
		if (id.group != StringUtils::NullID)
			groupPtr = ComponentGroupArchive::Instance().AsyncFindOrCreate(
			id.group);
		SharedComponent::Instance(oInst, classId, id.name, id.factory, groupPtr);
		if (oInst) {
			ser.Skip(c);
			AssetPtr asset = oInst;
			if (asset->AsyncIsCreated()) {
				
				String streamerImpl = streamer;
				StringUtils::ToUpper(streamerImpl);
				AssetLoaderImpl* impl = AssetLoader::GetImpl(streamerImpl, classId);
				// @todo Write appropriate constructor to initialize the loader/input combo
				StreamInfo streamInfo(impl, input);
				// load synchronously
				asset->RequestLoad(streamInfo);
			}
			return asset;
		}			
	}
	
	return AssetPtr();
}

void Asset::AssetSave(AssetPtr& asset, const URL& output, const String& streamer) {
	OutputStreamPtr stream = FileSystem::Instance().OpenWrite(output);
	if (stream)
		AssetSave(asset, stream, streamer.length() ? streamer : output.GetExtension());
}

void Asset::AssetSave(AssetPtr& asset, OutputStreamPtr& output, const String& streamer) {
	
	{
		ChunkOutputStream cser(output);
		OutputSerializer& ser = cser.BeginChunk(ASSET_HEADER);
		SharedComponent::ID id;
		uint32 classId = asset->GetProxyID();
		asset->GetID(id);
		ser << classId << id.name << id.factory << id.group;
		cser.EndChunk();
		// destroy the cser object for flushing
	}
	if (asset->AsyncIsReady()) {
	
		String streamerImpl = streamer;
		StringUtils::ToUpper(streamerImpl);
		AssetSaverImpl* impl = AssetSaver::GetImpl(streamerImpl, asset->GetClassID());
		// @todo Write appropriate constructor to initialize the loader/input combo
		StreamInfo streamInfo(impl, output);
		asset->RequestSave(streamInfo);
	}
}

void Asset::AddDependent(AssetStreamRequest* request) {
	if (_savedRequestPtr) {
		NEX_ASSERT(_savedRequestPtr->flags & StreamRequest::ASSET_STREAM_REQUEST);
		static_cast<AssetStreamRequest*>(_savedRequestPtr)->GetMetaInfo().AddDependent(request);
	}
}

/*********************************
 * Asset::Loader
 *********************************/
NEX_IMPLEMENT_COMPONENT_FACTORY(AssetLoader);

AssetLoader::AssetLoader(nextar::AssetStreamRequest* req) :
		request(req) {
}

AssetLoader::~AssetLoader() {
}

void AssetLoader::Serialize() {
	Asset* assetPtr = static_cast<Asset*>(request->GetStreamedObject());
	AssetLoaderImpl* impl = request->GetManualLoader();

	const URL& location = request->GetAssetLocator();

	if (!impl) {
		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		impl = GetImpl(ext, assetPtr->GetClassID());
	}
	if (!impl) {
		Error("No loader for type.");
		NEX_THROW_GracefulError(EXCEPT_MISSING_PLUGIN);
	}

	InputStreamPtr input = request->GetInputStream();

	if (!input && location != URL::Invalid) {
		input = FileSystem::Instance().OpenRead(location);
	}

	impl->Load(input, *this);
}

/*********************************
 * Asset::Saver
 *********************************/
NEX_IMPLEMENT_COMPONENT_FACTORY(AssetSaver);

AssetSaver::AssetSaver(nextar::AssetStreamRequest* req) :
		request(req) {
}

AssetSaver::~AssetSaver() {
}

void AssetSaver::Serialize() {
	Asset* assetPtr = static_cast<Asset*>(request->GetStreamedObject());
	AssetSaverImpl* impl = request->GetManualSaver();

	const URL& location = request->GetAssetLocator();

	if (!impl) {

		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		impl = GetImpl(ext, assetPtr->GetClassID());
	}
	if (!impl) {
		Error("No saver for type.");
		NEX_THROW_GracefulError(EXCEPT_MISSING_PLUGIN);
	}

	OutputStreamPtr output = request->GetOutputStream();

	if (!output && location != URL::Invalid) {
		output = FileSystem::Instance().OpenWrite(location);
	}

	impl->Save(output, *this);
}

}
