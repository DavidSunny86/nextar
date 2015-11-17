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
			textureParams->SetCompleted(false); // todo Not found return code. unify return codes??
			return;
		}
	} else {
		if (currentMaxMipLevel < img.numMipLevelToLoad)
			img.numMipLevelToLoad = currentMaxMipLevel;
		img.baseMipLevel = currentMaxMipLevel - img.numMipLevelToLoad;
	}

	if (textureParams->inputStream) {
		textureParams->image.Load(textureParams->inputStream, img,
				textureParams->codecInfo);
	}

	if (!(textureParams->flags & TextureStreamRequest::TEXTURE_PARAMS_INITED)) {
		textureParams->flags |= TextureStreamRequest::TEXTURE_PARAMS_INITED;
	}
	textureParams->SetCompleted(true);
}

StreamNotification TextureAsset::NotifyAssetLoadedImpl(nextar::StreamRequest* request) {
	TextureStreamRequest* textureParams =
			static_cast<TextureStreamRequest*>(request);

	// set up the highest dimensions currently available
	width = textureParams->image.GetWidth();
	height = textureParams->image.GetHeight();
	depth = textureParams->image.GetDepth();
	faces = textureParams->image.GetNumFaces();

	if (!IsTextureInited()) {
		numMipMaps = textureParams->codecInfo.metaInfo.maxMipMapCount;
		textureParams->createParams.lowestMipLevel = (uint8)textureParams->codecInfo.metaInfo.maxMipMapCount - 1;
		textureParams->createParams.image = &textureParams->image;
		
		textureParams->createParams.textureFormat =
				PixelUtils::GetNearestTextureFormat(
						textureParams->image.GetFormat());
		textureParams->createParams.textureFlags = (uint8)GetTextureFlags();
		// progressive load? we have to preallocate as otherwise it doesn't work for some reason
		if (textureParams->codecInfo.mipLevelsToRead > 0)
			textureParams->createParams.textureFlags |= TextureBase::PRE_ALLOCATE_STORAGE;
		// todo Texture array support
		type = TEXTURE_2D;
		if (textureParams->codecInfo.metaInfo.maxHeight == 1)
			type = TEXTURE_1D;
		else if (textureParams->codecInfo.metaInfo.maxDepth > 1)
			type = TEXTURE_3D;
		if (faces == 6)
			type = TEXTURE_CUBE_MAP;

		textureParams->createParams.type = GetTextureType();
	}

	textureParams->createParams.desc = textureParams->codecInfo.metaInfo;
	uint8 numMipsLeftToRead = (uint8)textureParams->codecInfo.mipLevelsToRead;
	
	textureParams->createParams.baseMipLevel = numMipsLeftToRead;			
	currentMaxMipLevel = numMipsLeftToRead;
	
	
	ContextObject::RequestUpdate(MSG_TEX_CREATE | MSG_TEX_UPLOAD,
			reinterpret_cast<ContextObject::ContextParamPtr>(&textureParams->createParams));
	bool completed = false;
	/* should we stream again, in case it was not fully loaded ? */
	if (numMipsLeftToRead > 0) {
		if (!IsAutoStreamEnabled()) {
			textureParams->flags |= StreamRequest::COMPLETED;
			completed = true;
		}
	} else {
		SetTextureComplete(true);
		textureParams->flags |= StreamRequest::COMPLETED;
		completed = true;
	}

	if (!IsTextureInited()) {
		SetTextureInited(true);
	}

	return completed ? StreamNotification::NOTIFY_COMPLETED_AND_READY : StreamNotification::NOTIFY_RESUBMIT_AND_READY;
}

void TextureAsset::NotifyAssetUnloaded() {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	Asset::NotifyAssetUnloaded();
}

void TextureAsset::NotifyAssetUpdated() {

	Asset::NotifyAssetUpdated();
}

void TextureAsset::UnloadImpl() {
}

StreamRequest* TextureAsset::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(TextureStreamRequest(this));
}

} /* namespace nextar */
