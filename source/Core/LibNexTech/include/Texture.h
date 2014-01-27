/*
 * Texture.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Asset.h"
#include "ContextObject.h"
#include "Image.h"

namespace nextar {

	class _NexExport Texture: public nextar::Asset,
	public TextureBase {
	public:

		enum Type {
			TYPE = Asset::COMPONENT_ASSET_TEXTURE,
		};

		enum Flags {
			TEXTURE_INITED = Asset::LAST_FLAG << 0,
			TEXTURE_COMPLETE = Asset::LAST_FLAG << 1,
			AUTOSTREAM_ENABLED = Asset::LAST_FLAG << 2,
			LAST_FLAG = Asset::LAST_FLAG << 3,
		};

		class TextureStreamRequest;
		struct LodStrategy;

		struct LodStrategy {
			virtual uint32 GetNumLevelsToLoad(Texture*) = 0;
			virtual uint32 GetNumLevelsToUnload(Texture*) = 0;
		};

		class TextureStreamRequest : public AllocGeneral,
							  public AssetStreamRequest {
		public:
			/** @todo, global render options can be checked */
			enum {
				TEXTURE_PARAMS_INITED = AssetStreamRequest::LAST_FLAG << 0,
				TEXTURE_STREAMING_ENABLED = AssetStreamRequest::LAST_FLAG << 1,
				LAST_FLAG = AssetStreamRequest::LAST_FLAG << 2,
			};

			InputStreamPtr inputStream;
			ImageParams imageParams;
			Image image;
			uint32 maxLodLevels;

			TextureStreamRequest(Texture* texture) : AssetStreamRequest(texture), maxLodLevels(-1) {
			}
		};

		Texture(AssetManager*, const String&);
		virtual ~Texture();

		static Texture* Instance(AssetManager*, const String&);
		static Texture* Instance(AssetManager*, const String&, const URL& locator);

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

		virtual void Create(nextar::RenderContext*);
		virtual void Update(nextar::RenderContext*, ContextObject::UpdateParamPtr update);
		virtual void Destroy(nextar::RenderContext*);

		virtual int GetType() const;

	protected:

		virtual void NotifyAssetLoaded();
		virtual void NotifyAssetUnloaded();
		virtual void NotifyAssetUpdated();

		virtual void LoadImpl(StreamRequest* req, bool isAsync);
		virtual void UnloadImpl(StreamRequest* req, bool isAsync);

		virtual StreamRequest* CreateStreamRequestImpl(bool load);
		virtual void DestroyStreamRequestImpl(StreamRequest*&, bool load=true);

	    virtual void WriteBoxImpl(RenderContext*, size_t faceNum, size_t mipNum,
	            PixelBox& box) = 0;

		LodStrategy* lodStrategy;
	};

} /* namespace nextar */
#endif /* TEXTURE_H_ */
