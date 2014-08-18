#include <NexEngine.h>
#include <Asset.h>
#include <AssetStreamer.h>

namespace nextar {

Asset::AssetLocatorAccessor Asset::AssetLocatorAccessor::assetLocatorAccessor;

Asset::Asset(const StringID id, const StringID factory) :
		memoryCost(sizeof(Asset)), SharedComponent(id, factory), streamRequest(nullptr) {
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

void Asset::Load(StreamRequest* req, bool async) {
	if (IsLoaded() || IsLoading()) {
		Debug("Asset already loaded: " + GetName());
		return;
	}

	if (!req) {
		streamRequest = GetStreamRequest();
	} else
		streamRequest = req;

	if (async && IsBackgroundStreamed()) {
		AssetStreamer::Instance().RequestLoad(this);
	} else {
		SetLoading(true);
		try {
			LoadImpl(streamRequest, false);
			if (streamRequest->flags & StreamRequest::ASSET_STREAM_REQUEST)
				_LoadDependencies(
						static_cast<AssetStreamRequest*>(streamRequest));

			SetLoading(false);
			SetLoaded(true);
			NotifyAssetLoaded();
		} catch (GracefulErrorExcept& e) {
			Debug(e.GetMsg());
			SetLoading(false);
			SetLoaded(false);
		}
	}
}

void Asset::Unload() {
	if (IsLoading()) {
		Debug("Asset being loaded: " + GetName());
		return;
	}
	try {
		UnloadImpl();
		NotifyAssetUnloaded();
	} catch (GracefulErrorExcept& e) {
		Debug(e.GetMsg());
		SetLoaded(false);
	}
}

void Asset::Save(StreamRequest* req, bool async) {
	if (!IsLoaded() || IsLoading() || IsSaving()) {
		Debug("Asset has to be saved first: " + GetName());
		return;
	}

	if (!req) {
		streamRequest = GetStreamRequest(false);
	} else
		streamRequest = req;

	if (async && IsBackgroundStreamed()) {
		AssetStreamer::Instance().RequestSave(this);
	} else {
		SetSaving(true);
		try {
			SaveImpl(streamRequest, false);
			SetSaving(false);
			NotifyAssetSaved();
		} catch (GracefulErrorExcept& e) {
			Debug(e.GetMsg());
			SetSaving(false);
		}
	}
}

void Asset::NotifyAssetLoaded() {
	if (NotifyAssetLoadedImpl()) {
		streamRequest->flags |= StreamRequest::COMPLETED;
		SetReady(true);
	}

	_FireCallbacksLoaded();
	if (streamRequest) {
		uint16 reqflags = streamRequest->flags;
		if ((reqflags
				& (StreamRequest::AUTO_DELETE_REQUEST | StreamRequest::COMPLETED))
				== (StreamRequest::AUTO_DELETE_REQUEST
						| StreamRequest::COMPLETED))
			DestroyStreamRequestImpl(streamRequest);
		streamRequest = nullptr;
	}
}

void Asset::NotifyAssetUnloaded() {
}

void Asset::NotifyAssetUpdated() {
}

void Asset::NotifyAssetSaved() {
	if (NotifyAssetSavedImpl()) {
		streamRequest->flags |= StreamRequest::COMPLETED;
		SetReady(true);
	}

	_FireCallbacksSaved();
	if (streamRequest) {
		uint16 reqflags = streamRequest->flags;
		if ((reqflags
			& (StreamRequest::AUTO_DELETE_REQUEST | StreamRequest::COMPLETED))
			== (StreamRequest::AUTO_DELETE_REQUEST
			| StreamRequest::COMPLETED))
			DestroyStreamRequestImpl(streamRequest, false);
		streamRequest = nullptr;
	}
}

bool Asset::NotifyAssetLoadedImpl() {
	return true;
}

bool Asset::NotifyAssetSavedImpl() {
	return true;
}

void Asset::AsyncLoad(StreamRequest* request) {
	try {
		LoadImpl(request, true);
	} catch (GracefulErrorExcept& e) {
		Debug(e.GetMsg());
		request->returnCode = (uint16) StreamResult::LOAD_FAILED;
	}
}

void Asset::AsyncSave(StreamRequest* request) {
	try {
		SaveImpl(request, true);
	} catch (GracefulErrorExcept& e) {
		Debug(e.GetMsg());
		request->returnCode = (uint16) StreamResult::SAVE_FAILED;
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

void Asset::DestroyStreamRequestImpl(StreamRequest*& request, bool) {
	NEX_FREE_T(AssetStreamRequest, MEMCAT_GENERAL,
			static_cast<AssetStreamRequest*>(request));
	request = nullptr;
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
		a->Load(false);
	}
}

AssetPtr Asset::AsyncLoad(const URL& input) {
	InputStreamPtr inputStream = FileSystem::Instance().OpenRead(input);
	if (inputStream)
		return AsyncLoad(inputStream);
	return AssetPtr();
}

AssetPtr Asset::AsyncLoad(InputStreamPtr& input) {
	ChunkInputStream ser(input);
	InputSerializer::Chunk c = InputSerializer::Invalid;

	ser.ReadChunk(ASSET_HEADER, c);
	if (InputSerializer::IsValid(c)) {
		uint32 classId;
		StringID name, factory, group;
		ser >> classId >> name >> factory >> group;
		SharedComponentPtr oInst;
		Group* groupPtr = nullptr;
		if (group != StringUtils::NullID)
			groupPtr = ComponentGroupArchive::Instance().AsyncFindOrCreate(
			group);
		SharedComponent::Instance(oInst, classId, name, factory, groupPtr);
		if (oInst) {
			ser.Skip(c);
			AssetPtr asset = oInst;
			AssetStreamRequest* req = static_cast<AssetStreamRequest*>(asset->GetStreamRequest());
			if (req) {
				req->flags |= StreamRequest::AUTO_DELETE_REQUEST;
				req->SetInputStream(input);
			}
			// load synchronously
			asset->Load(req, false);
			return asset;
		}			
	}
	
	return AssetPtr();
}

void Asset::AsyncSave(AssetPtr& asset, const URL& output) {
	OutputStreamPtr stream = FileSystem::Instance().OpenWrite(output);
	if (stream)
		AsyncSave(asset,stream);
}

void Asset::AsyncSave(AssetPtr& asset, OutputStreamPtr& output) {

	{
		ChunkOutputStream cser(output);
		OutputSerializer& ser = cser.BeginChunk(ASSET_HEADER);
		SharedComponent::ID id;
		asset->GetID(id);
		uint32 classId = asset->GetClassID();
		ser << classId << id.name << id.factory << id.group;
		cser.EndChunk();
		// destroy the cser object for flushing
	}

	AssetStreamRequest* req = static_cast<AssetStreamRequest*>(asset->GetStreamRequest(false));
	if (req) {
		req->flags |= StreamRequest::AUTO_DELETE_REQUEST;
		req->SetOutputStream(output);
	}
	asset->Save(req, false);
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

	const URL& location = assetPtr->GetAssetLocator();

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

	if (!input) {
		Error(
			String("Could not open asset file: ")
			+ request->GetName());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOCATE_ASSET);
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

	const URL& location = assetPtr->GetAssetLocator();

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

	if (!output) {
		Error(
			String("Could not write to asset file: ")
			+ assetPtr->GetAssetLocator().ToString());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOCATE_ASSET);
	}

	impl->Save(output, *this);
}

}
