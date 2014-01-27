/*
 * Texture.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include "Texture.h"

namespace nextar {

	class DefaultTextureLodStrategy : public Texture::LodStrategy {
	public:
		static DefaultTextureLodStrategy strategy;

		virtual uint32 GetNumLevelsToLoad(Texture* texture) {
			return 2;
		}

		virtual uint32 GetNumLevelsToUnload(Texture* texture) {
			return 2;
		}
	};

	Texture::Texture(AssetManager* assetManager, const String& name) :
		TextureBase(TextureBase::TEXTURE_ASSET),
		Asset(assetManager, name),
		lodStrategy(&DefaultTextureLodStrategy::strategy) {

		flags |= (AUTOGEN_MIP_MAPS_ENABLED|AUTOSTREAM_ENABLED);
	}

	Texture::~Texture() {
	}

	int Texture::GetType() const {
		return Texture::TYPE;
	}

	Texture* Texture::Instance(AssetManager* am, const String& name) {
		return am->AsyncCreate(Texture::TYPE, name);
	}

	Texture* Texture::Instance(AssetManager* am, const String& name, const URL& locator) {
		Texture* texture = Instance(am, name);
		if (texture)
			texture->SetAssetLocator(locator);
		return texture;
	}

	void Texture::LoadImpl(StreamRequest* request, bool isStreamed) {
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
		InputStreamPtr& is = FileSystem::Instance().OpenRead(GetAssetLocator());
		if (is) {
			// @todo handle error here? try catch {}
			textureParams->image.Load(is, img);
		}

		if(textureParams->flags & TextureStreamRequest::TEXTURE_PARAMS_INITED) {
			textureParams->maxLodLevels = textureParams->image.GetTotalMipMapCount();
			textureParams->flags |= TextureStreamRequest::TEXTURE_PARAMS_INITED;
		}
	}

	void Texture::NotifyAssetLoaded() {
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

		ContextObject::NotifyUpdated(textureParams);
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
				StreamRequest::AUTO_DELETE_REQUEST|StreamRequest::COMPLETED)
			DestroyStreamRequestImpl(streamRequest);

		if (!IsTextureInited()) {
			SetTextureInited(true);
			Asset::NotifyAssetLoaded();
			SetReady(true);
		} else {
			Asset::NotifyAssetUpdated();
		}
	}

	void Texture::Create(nextar::RenderContext* rc) {
		/* unload */
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}

	void Texture::Update(nextar::RenderContext* rc, ContextObject::UpdateParamPtr params) {
		TextureStreamRequest* textureParams = static_cast<TextureStreamRequest*>(params);
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

	void Texture::Destroy(nextar::RenderContext* rc) {
		/* unload */
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}

	void Texture::NotifyAssetUnloaded() {
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
		Asset::NotifyAssetUnloaded();
	}

	void Texture::NotifyAssetUpdated() {
		TextureStreamRequest* textureParams = static_cast<TextureStreamRequest*>(
						GetStreamRequest());
		// @TODO update parameters?
		ContextObject::NotifyUpdated(textureParams);
		Asset::NotifyAssetUpdated();
	}

	void Texture::UnloadImpl(StreamRequest* req, bool isStreamed) {
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}

	StreamRequest* Texture::CreateStreamRequestImpl(bool load) {
		return NEX_NEW TextureStreamRequest(this);
	}

	void Texture::DestroyStreamRequestImpl(StreamRequest*& request, bool load) {
		TextureStreamRequest* req = static_cast<TextureStreamRequest*>(request);
		NEX_DELETE req;
		request = nullptr;
	}

} /* namespace nextar */
