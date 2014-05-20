/*
 * Texture.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <TextureAsset.h>
#include <AssetStreamer.h>

namespace nextar {

	class DefaultTextureLodStrategy : public TextureAsset::LodStrategy {
	public:
		static DefaultTextureLodStrategy strategy;

		virtual uint32 GetNumLevelsToLoad(TextureAsset* texture) {
			return 2;
		}

		virtual uint32 GetNumLevelsToUnload(TextureAsset* texture) {
			return 2;
		}

	protected:
		~DefaultTextureLodStrategy() {}
	};

	DefaultTextureLodStrategy DefaultTextureLodStrategy::strategy;

	TextureAsset::TextureAsset(const StringID name) :
		TextureBase(TextureBase::TEXTURE_ASSET),
		Asset(name),
		lodStrategy(&DefaultTextureLodStrategy::strategy) {

		flags |= (AUTOGEN_MIP_MAPS_ENABLED|AUTOSTREAM_ENABLED);
	}

	TextureAsset::~TextureAsset() {
	}

	uint32 TextureAsset::GetClassID() const {
		return TextureAsset::CLASS_ID;
	}

	void TextureAsset::LoadImpl(StreamRequest* request, bool isStreamed) {
		/* load a certain mip level if streaming is on, else
		 * load the whole texture */
		TextureStreamRequest* textureParams = static_cast<TextureStreamRequest*>(request);
		/* exclusive call between NotifyLoaded and LoadImpl */
		if (IsTextureInited()) {
			textureParams->flags |= TextureStreamRequest::TEXTURE_PARAMS_INITED;
			textureParams->imageParams.name = GetName();
		} else
			textureParams->flags &=~TextureStreamRequest::TEXTURE_PARAMS_INITED;

		ImageParams& img = textureParams->imageParams;
		img.numMipLevelToLoad = IsAutoStreamEnabled() ? lodStrategy->GetNumLevelsToLoad(this) :
				Image::IMAGE_ALL_MIP_LEVELS;
		/* was the texture ever loaded? */
		if(!(textureParams->flags & TextureStreamRequest::TEXTURE_PARAMS_INITED)) {
			img.baseMipLevel = Image::IMAGE_LOWEST_MIP_LEVEL;
			img.name = GetName();
		} else
			img.baseMipLevel = currentMaxMipLevel;
		InputStreamPtr is = FileSystem::Instance().OpenRead(GetAssetLocator());
		if (is) {
			// todo handle error here? try catch {}
			textureParams->image.Load(is, img);
		}

		if(textureParams->flags & TextureStreamRequest::TEXTURE_PARAMS_INITED) {
			textureParams->maxLodLevels = textureParams->image.GetTotalMipMapCount();
			textureParams->flags |= TextureStreamRequest::TEXTURE_PARAMS_INITED;
		}
	}

	void TextureAsset::NotifyAssetLoaded() {
		TextureStreamRequest* textureParams = static_cast<TextureStreamRequest*>(
				GetStreamRequest());

		width = textureParams->image.GetWidth();
		height = textureParams->image.GetHeight();
		depth = textureParams->image.GetDepth();
		faces = textureParams->image.GetNumFaces();

		if (!IsTextureInited()) {
			ContextObject::NotifyCreated();
			numMipMaps = textureParams->maxLodLevels;
			type = TEXTURE_2D;
			if (height == 1)
				type = TEXTURE_1D;
			else if (depth > 1)
				type = TEXTURE_3D;
			if (faces == 6)
				type = TEXTURE_CUBE_MAP;
		}

		ContextObject::NotifyUpdated(reinterpret_cast<ContextObject::UpdateParamPtr>(textureParams));
		/* should we stream again, in case it was not fully loaded ? */
		if (currentMaxMipLevel < numMipMaps) {
			if (IsAutoStreamEnabled()) {
				AssetStreamer::Instance().RequestLoad(this);
			} else
				textureParams->flags |= StreamRequest::COMPLETED;
		} else {
			SetTextureComplete(true);
			textureParams->flags |= StreamRequest::COMPLETED;
		}

		uint16 reqflags = streamRequest->flags;
		if ((reqflags & (StreamRequest::AUTO_DELETE_REQUEST|StreamRequest::COMPLETED)) ==
				(StreamRequest::AUTO_DELETE_REQUEST|StreamRequest::COMPLETED))
			DestroyStreamRequestImpl(streamRequest);

		if (!IsTextureInited()) {
			SetTextureInited(true);
			Asset::NotifyAssetLoaded();
			SetReady(true);
		} else {
			Asset::NotifyAssetUpdated();
		}
	}

	void TextureAsset::Create(nextar::RenderContext* rc) {
		/* unload */
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}

	void TextureAsset::Update(nextar::RenderContext* rc, ContextObject::UpdateParamPtr params) {
		TextureStreamRequest* textureParams = reinterpret_cast<TextureStreamRequest*>(params);
		Image& img = textureParams->image;
		uint32 numMips = img.GetNumMipMaps();
		uint32 numFaces = img.GetNumFaces();
		for (uint32 f = 0; f < numFaces; ++f) {
			for(uint32 i = 0; i < numMips; ++i) {
				PixelBox box = img.GetPixelBox(f, i);
				WriteBoxImpl(rc, f, currentMaxMipLevel + i, box);
			}
		}
		currentMaxMipLevel += numMips;
	}

	void TextureAsset::Destroy(nextar::RenderContext* rc) {
		/* unload */
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}

	void TextureAsset::NotifyAssetUnloaded() {
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
		Asset::NotifyAssetUnloaded();
	}

	void TextureAsset::NotifyAssetUpdated() {
		TextureStreamRequest* textureParams = static_cast<TextureStreamRequest*>(
						GetStreamRequest());
		// @TODO update parameters?
		ContextObject::NotifyUpdated(reinterpret_cast<ContextObject::UpdateParamPtr>(textureParams));
		Asset::NotifyAssetUpdated();
	}

	void TextureAsset::UnloadImpl(StreamRequest* req, bool isStreamed) {
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}

	StreamRequest* TextureAsset::CreateStreamRequestImpl(bool load) {
		return NEX_NEW(TextureStreamRequest(this));
	}

	void TextureAsset::DestroyStreamRequestImpl(StreamRequest*& request, bool load) {
		TextureStreamRequest* req = static_cast<TextureStreamRequest*>(request);
		NEX_DELETE(req);
		request = nullptr;
	}

} /* namespace nextar */
