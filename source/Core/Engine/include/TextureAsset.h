/*
 * Texture.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <Asset.h>
#include <ContextObject.h>
#include <Image.h>
#include <TextureBase.h>

namespace nextar {

class _NexEngineAPI TextureAsset: public nextar::Asset, public TextureBase {
	NEX_LOG_HELPER(TextureAsset);

public:

	enum Type {
		CLASS_ID = Component::CLASS_ASSET_TEXTURE, CATAGORY = COMPONENT_CAT(
				CLASS_ID),
	};

	enum Flags {
		TEXTURE_INITED = Asset::LAST_FLAG << 0,
		TEXTURE_COMPLETE = Asset::LAST_FLAG << 1,
		AUTOSTREAM_ENABLED = Asset::LAST_FLAG << 2,
		LAST_FLAG = Asset::LAST_FLAG << 3,
	};

	class TextureStreamRequest;
	class LodStrategy;

	typedef AssetTraits<TextureAsset> Traits;
	typedef FactoryTraits<TextureAsset> FactoryTraits;

	class LodStrategy {
	public:
		virtual uint32 GetNumLevelsToLoad(TextureAsset*) = 0;
		virtual uint32 GetNumLevelsToUnload(TextureAsset*) = 0;

	protected:
		~LodStrategy() {
		}
	};

	class TextureStreamRequest: public AllocGeneral, public AssetStreamRequest {
	public:
		/** todo, global render options can be checked */
		enum {
			TEXTURE_PARAMS_INITED = AssetStreamRequest::LAST_FLAG << 0,
			TEXTURE_STREAMING_ENABLED = AssetStreamRequest::LAST_FLAG << 1,
			LAST_FLAG = AssetStreamRequest::LAST_FLAG << 2,
		};

		InputStreamPtr inputStream;
		Image image;
		ImageParams imageParams;
		ImageCodecMetaInfo codecInfo;
		UpdateParams createParams;

		TextureStreamRequest(TextureAsset* texture) :
				AssetStreamRequest(texture) {
			codecInfo.metaInfo.maxMipMapCount = -1;
		}
	};

	TextureAsset(const StringID, const StringID factory);
	virtual ~TextureAsset();

	inline bool IsTextureInited() const {
		return (flags & TEXTURE_INITED) != 0;
	}

	inline bool IsAutoStreamEnabled() const {
		return (flags & AUTOSTREAM_ENABLED) != 0;
	}

	inline void SetTextureInited(bool t) {
		SetFlag(TEXTURE_INITED, t);
	}

	inline void SetTextureComplete(bool t) {
		SetFlag(TEXTURE_COMPLETE, t);
	}

	virtual uint32 GetClassID() const;

protected:

	virtual void NotifyAssetLoaded();
	virtual void NotifyAssetUnloaded();
	virtual void NotifyAssetUpdated();

	virtual void LoadImpl(StreamRequest* req, bool isAsync);
	virtual void UnloadImpl();

	virtual StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void DestroyStreamRequestImpl(StreamRequest*&, bool load = true);

	LodStrategy* lodStrategy;
};

} /* namespace nextar */
#endif /* TEXTURE_H_ */
