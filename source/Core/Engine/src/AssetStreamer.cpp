/*
 * AssetStreamer.cpp
 *
 *  Created on: 07-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Asset.h>
#include <AssetStreamer.h>
#include <BackgroundStreamer.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(AssetStreamer);AssetStreamer ::AssetStreamer() {

}

AssetStreamer::~AssetStreamer() {
}

void AssetStreamer::AsyncRequestLoad(AssetStreamRequest* req) {
	/** */
	req->streamHandler = this;
	BackgroundStreamer::Instance().AsyncAddRequest(req);
}

void AssetStreamer::AsyncRequestSave(AssetStreamRequest* req) {
	req->flags |= StreamRequest::REQUEST_SAVE;
	req->streamHandler = this;
	BackgroundStreamer::Instance().AsyncAddRequest(req);
}

void AssetStreamer::NotifySaved(StreamRequest* request) {
	NEX_ASSERT(request->flags & StreamRequest::REQUEST_SAVE);
	AssetStreamRequest* assetReq = static_cast<AssetStreamRequest*>(request);
	Asset* asset = static_cast<Asset*>(assetReq->streamedObject);
	// no dependencies, just notify
	_NotifyAssetSaved(asset, assetReq);
}

void AssetStreamer::NotifyLoaded(StreamRequest* request) {
	NEX_ASSERT(request->flags & StreamRequest::ASSET_STREAM_REQUEST);
	AssetStreamRequest* assetReq = static_cast<AssetStreamRequest*>(request);
	Asset* asset = static_cast<Asset*>(assetReq->streamedObject);
	Asset::MetaInfo& meta = assetReq->GetMetaInfo();
	if (meta.GetDependencyCount() == 0) {
		_NotifyAssetLoaded(asset, assetReq);
	} else {
		AssetSet& dependencies = meta.GetDependencies();
		// NB: It would probably be better to check an unresolved asset state atomically if 
		// its in loading state and then push it for loading directly to the queue. This will
		// probably kill starvation, but two things need to be atomic in such a case (or may
		// be one depending on memory ordering). The request pointer, and the atomic state.
		for (auto it = dependencies.begin(); it != dependencies.end();) {
			if ((*it)->AsyncHasLoadFailed()) {
				dependencies.clear();
				assetReq->returnCode = StreamResult::STREAM_FAILED;
				_NotifyAssetLoaded(asset, assetReq);
				break;
			}
			if ((*it)->AsyncIsLoaded())
				dependencies.erase(it++);
			else {
				/**
				 * Note we touch dependent list but not unresolvedDependencies
				 * because that might be accessed from the loading thread.
				 * */
				StreamRequest* dep = (*it)->GetStreamRequest();
				NEX_ASSERT(dep->flags & StreamRequest::ASSET_STREAM_REQUEST);
				AssetStreamRequest* asr = static_cast<AssetStreamRequest*>(dep);
				asr->GetMetaInfo().AddDependent(assetReq);
				if (!(*it)->IsLoading() && !(*it)->HasLoadFailed())
					RequestLoad((*it));
				++it;
			}
		}
	}
}

void AssetStreamer::_NotifyAssetLoaded(Asset* asset,
		AssetStreamRequest* assetReq) {
	Asset::MetaInfo& meta = assetReq->GetMetaInfo();
	asset->SetLoading(false);
	if (assetReq->returnCode == StreamResult::STREAM_SUCCESS) {
		asset->SetLoaded(true);
	} else {
		asset->SetLoadFailed(true);
		assetReq->flags |= StreamRequest::COMPLETED;
	}
	asset->NotifyAssetLoaded();
	// do a quick swap for the dependent set, as the request might
	// get deleted in notify, and request pointer is no more required.
	AssetStreamRequestSet reqSet;
	reqSet.swap(meta.GetDependents());	
	_NotifyDependents(asset, reqSet);
}

void AssetStreamer::_NotifyAssetSaved(Asset* asset,
		AssetStreamRequest* assetReq) {
	asset->SetSaving(false);
	if (assetReq->returnCode != StreamResult::STREAM_SUCCESS) {
		asset->SetSaveFailed(true);
		assetReq->flags |= StreamRequest::COMPLETED;
	}
	asset->NotifyAssetSaved();
}

void AssetStreamer::_NotifyDependents(Asset* asset,
		AssetStreamRequestSet& reqSet) {
	/**
	 * Important: the unresolvedDependency list in the MetaInfo class is filled up
	 * in the Asset::AsyncLoad call. This means it is a task carried in a separate thread.
	 * So, modifying it from Main thread might seem dangerous. However, it should be noted
	 * that this list is never touched outside the Loading thread when the loading is happening.
	 * Once the list is built and the AssetStreamer::NotifyLoaded call is made, the list
	 * is exclusive and will never be touched from any other thread. The asset is marked as
	 * unresolved (when waiting for dependencies filled up in AsyncLoad), and the only place
	 * these dependencies are resolved is the Main thread.
	 * */
	for (auto d : reqSet) {
		if (!d->GetMetaInfo().RemoveDependency(asset)) {
			_NotifyAssetLoaded(static_cast<Asset*>(d->streamedObject), d);
		}
	}
}

} /* namespace nextar */
