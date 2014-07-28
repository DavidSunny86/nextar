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

class DefaultTextureLodStrategy: public TextureAsset::LodStrategy {
public:
	static DefaultTextureLodStrategy strategy;

	virtual uint32 GetNumLevelsToLoad(TextureAsset* texture) {
		return 2;
	}

	virtual uint32 GetNumLevelsToUnload(TextureAsset* texture) {
		return 2;
	}

protected:
	~DefaultTextureLodStrategy() {
	}
};

DefaultTextureLodStrategy DefaultTextureLodStrategy::strategy;

TextureAsset::TextureAsset(const StringID name, const StringID factory) :
		TextureBase(TextureBase::TEXTURE_ASSET), Asset(name, factory), lodStrategy(
				&DefaultTextureLodStrategy::strategy) {

	flags |= (AUTOGEN_MIP_MAPS_ENABLED | AUTOSTREAM_ENABLED);
}

TextureAsset::~TextureAsset() {
}

uint32 TextureAsset::GetClassID() const {
	return TextureAsset::CLASS_ID;
}

void TextureAsset::LoadImpl(StreamRequest* request, bool isStreamed) {
	/* load a certain mip level if streaming is on, else
	 * load the whole texture */
	TextureStreamRequest* textureParams =
			static_cast<TextureStreamRequest*>(request);
	/* exclusive call between NotifyLoaded and LoadImpl */
	if (IsTextureInited()) {
		textureParams->flags |= TextureStreamRequest::TEXTURE_PARAMS_INITED;
	} else
		textureParams->flags &= ~TextureStreamRequest::TEXTURE_PARAMS_INITED;

	ImageParams& img = textureParams->imageParams;
	img.progressiveLoad = IsAutoStreamEnabled();
	img.numMipLevelToLoad =
			(img.progressiveLoad) ?
					lodStrategy->GetNumLevelsToLoad(this) :
					Image::IMAGE_ALL_MIP_LEVELS;
	/* was the texture ever loaded? */
	if (!(textureParams->flags & TextureStreamRequest::TEXTURE_PARAMS_INITED)) {
		img.codecName = GetAssetLocator().GetExtension();
		img.name = GetNameID();
		img.baseMipLevel = Image::IMAGE_LOWEST_MIP_LEVEL;
		textureParams->inputStream = FileSystem::Instance().OpenRead(
				GetAssetLocator());
		if (!textureParams->inputStream) {
			Warn("Texture not found: " + GetAssetLocator().ToString());
			request->returnCode = -1; // todo Not found return code. unify return codes??
			return;
		}
	} else
		img.baseMipLevel = currentMaxMipLevel;

	if (textureParams->inputStream) {
		// todo handle error here? try catch {}
		textureParams->inputStream->Rewind();
		textureParams->image.Load(textureParams->inputStream, img,
				textureParams->codecInfo);
	}

	if (!(textureParams->flags & TextureStreamRequest::TEXTURE_PARAMS_INITED)) {
		textureParams->flags |= TextureStreamRequest::TEXTURE_PARAMS_INITED;
	}
}

void TextureAsset::NotifyAssetLoaded() {
	TextureStreamRequest* textureParams =
			static_cast<TextureStreamRequest*>(GetStreamRequest());

	// set up the highest dimensions currently available
	width = textureParams->image.GetWidth();
	height = textureParams->image.GetHeight();
	depth = textureParams->image.GetDepth();
	faces = textureParams->image.GetNumFaces();

	if (!IsTextureInited()) {
		numMipMaps = textureParams->codecInfo.metaInfo.maxMipMapCount;
		textureParams->createParams.image = &textureParams->image;
		textureParams->createParams.textureFormat =
				PixelUtils::GetNearestTextureFormat(
						textureParams->image.GetFormat());
		textureParams->createParams.textureFlags = (uint8)GetTextureFlags();
		// todo Texture array support
		type = TEXTURE_2D;
		if (height == 1)
			type = TEXTURE_1D;
		else if (depth > 1)
			type = TEXTURE_3D;
		if (faces == 6)
			type = TEXTURE_CUBE_MAP;

		textureParams->createParams.type = GetTextureType();
	}

	textureParams->createParams.baseMipLevel =
			(uint8)textureParams->codecInfo.mipLevelsToRead;
	textureParams->createParams.desc = textureParams->codecInfo.metaInfo;

	ContextObject::RequestUpdate(MSG_TEX_CREATE | MSG_TEX_UPLOAD,
			reinterpret_cast<ContextObject::ContextParamPtr>(&textureParams->createParams));
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
	if ((reqflags
			& (StreamRequest::AUTO_DELETE_REQUEST | StreamRequest::COMPLETED))
			== (StreamRequest::AUTO_DELETE_REQUEST | StreamRequest::COMPLETED))
		DestroyStreamRequestImpl(streamRequest);

	if (!IsTextureInited()) {
		SetTextureInited(true);
		Asset::NotifyAssetLoaded();
		SetReady(true);
	} else {
		Asset::NotifyAssetUpdated();
	}
}

void TextureAsset::NotifyAssetUnloaded() {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	Asset::NotifyAssetUnloaded();
}

void TextureAsset::NotifyAssetUpdated() {
	TextureStreamRequest* textureParams =
			static_cast<TextureStreamRequest*>(GetStreamRequest());
	// @TODO update parameters?
	ContextObject::RequestUpdate(MSG_TEX_RESIZE,
			reinterpret_cast<ContextObject::ContextParamPtr>(textureParams));
	Asset::NotifyAssetUpdated();
}

void TextureAsset::UnloadImpl() {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

StreamRequest* TextureAsset::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(TextureStreamRequest(this));
}

void TextureAsset::DestroyStreamRequestImpl(StreamRequest*& request,
		bool load) {
	TextureStreamRequest* req = static_cast<TextureStreamRequest*>(request);
	NEX_DELETE(req);
	request = nullptr;
}

} /* namespace nextar */
