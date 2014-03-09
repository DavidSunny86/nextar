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

	NEX_DEFINE_SINGLETON_PTR(AssetStreamer);
	AssetStreamer::AssetStreamer() {

	}

	AssetStreamer::~AssetStreamer() {
	}

	void AssetStreamer::RequestLoad(Asset* asset) {
		NEX_ASSERT(asset);
		StreamRequest* req = asset->GetStreamRequest();
		if (!(req->flags & StreamRequest::ASSET_STREAM_REQUEST)) {
			Warn("Cannot register a generic load request for asset, "
					"request must be of type AssetStreamRequest: " + asset->GetName());
			return;
		}
		/** */
		if (asset->IsLoading()) {
			Warn("Redundant load request for asset: " + asset->GetName());
			return;
		}
		asset->SetLoading(true);
		req->streamHandler = this;
		BackgroundStreamer::Instance().Stream(req);
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
			for(auto it = dependencies.begin(); it != dependencies.end(); ) {
				if ((*it)->IsLoaded())
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
					if (!(*it)->IsLoading())
						RequestLoad((*it));
					++it;
				}
			}
		}
	}

	void AssetStreamer::_NotifyAssetLoaded(Asset* asset, AssetStreamRequest* assetReq) {
		Asset::MetaInfo& meta = assetReq->GetMetaInfo();
		asset->SetLoading(false);
		if (assetReq->returnCode == (uint16)StreamResult::LOAD_SUCCESS)
			asset->SetLoaded(true);
		// do a quick swap for the dependent set, as the request might
		// get deleted in notify, and dependents are no more required.
		AssetStreamRequestSet reqSet;
		reqSet.swap(meta.GetDependents());
		asset->NotifyAssetLoaded();
		_NotifyDependents(asset, reqSet);
	}

	void AssetStreamer::_NotifyDependents(Asset* asset, AssetStreamRequestSet& reqSet) {
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
		for(auto d : reqSet) {
			if(!d->GetMetaInfo().RemoveDependency(asset)) {
				_NotifyAssetLoaded(static_cast<Asset*>(d->streamedObject), d);
			}
		}
	}
} /* namespace nextar */
