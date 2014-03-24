/*
 * AssetStreamer.h
 *
 *  Created on: 07-Nov-2013
 *      Author: obhi
 */

#ifndef ASSETSTREAMER_H_
#define ASSETSTREAMER_H_

#include <Singleton.h>
#include <BackgroundStreamer.h>

namespace nextar {

	class _NexEngineAPI AssetStreamer:
			public nextar::Singleton<AssetStreamer>,
			public nextar::StreamHandler,
			public AllocGeneral {

		NEX_LOG_HELPER(AssetStreamer);

	public:

		AssetStreamer();
		virtual ~AssetStreamer();
		
		void RequestLoad(Asset* asset);
		void RequestUnload(Asset* asset);
		
		virtual void NotifyLoaded(StreamRequest*);
		virtual void NotifyUnloaded(StreamRequest*);

	protected:

		void _NotifyAssetLoaded(Asset* asset, AssetStreamRequest* req);
		void _NotifyDependents(Asset*, AssetStreamRequestSet&);
	};

} /* namespace nextar */
#endif /* ASSETSTREAMER_H_ */
