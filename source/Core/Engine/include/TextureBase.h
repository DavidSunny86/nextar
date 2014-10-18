/*
 * TextureBase.h
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#ifndef TEXTUREBASE_H_
#define TEXTUREBASE_H_

#include <ContextObject.h>
#include <PixelFormat.h>
#include <Image.h>

namespace nextar {

class TextureBase: public nextar::ContextObject {
public:

	enum TextureType
		: uint8 {
			TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_CUBE_MAP,
		TEXTURE_1D_ARRAY,
		TEXTURE_2D_ARRAY,
		TEXTURE_CUBE_MAP_ARRAY,
		TEXTURE_RENDER_TARGET,
		TEXTURE_TYPE_UNKNOWN
	};

	enum TextureFlags {

		/* Indicates its a texture asset */
		TEXTURE_ASSET = 1 << 1,
		/* Automatically generates all mip levels */
		AUTOGEN_MIP_MAPS_ENABLED = 1 << 2,
		/* Use immutable storage rather than allocating when streaming individual mip levels */
		PRE_ALLOCATE_STORAGE = 1 << 3,

		IS_RENDER_TEXTURE = 1 << 4,
	};

	enum TextureUpdateMessage {
		MSG_TEX_RESIZE = 1 << 1, MSG_TEX_UPLOAD = 1 << 2, MSG_TEX_CREATE = 1
				<< 3, MSG_TEX_READ = 1 << 4,
	};

	struct ReadPixelUpdateParams {
		std::atomic_flag lock;
		PixelBox* box;
		ReadPixelUpdateParams() : box(nullptr) {
			lock.clear(std::memory_order_relaxed);
		}
	};

	struct UpdateParams {
		PixelFormat textureFormat;
		TextureType type;
		uint8 textureFlags;
		ImageMetaInfo desc;
		uint8 baseMipLevel;
		Image* image;

		UpdateParams() :
				image(nullptr), baseMipLevel(0), textureFlags(0), type(
						TEXTURE_TYPE_UNKNOWN), textureFormat(
						PixelFormat::UNKNOWN) {
		}
	};

	TextureBase(uint16 flags = 0, ContextObject::Type type =
			ContextObject::TYPE_TEXTURE);
	virtual ~TextureBase();

	inline float GetTextureLod() const {
		return (float) currentMaxMipLevel / (float) numMipMaps;
	}

	inline bool IsTextureAsset() const {
		return (textureFlags & TEXTURE_ASSET) != 0;
	}

	inline bool IsImmutable() const {
		return (textureFlags & PRE_ALLOCATE_STORAGE) != 0;
	}

	inline TextureType GetTextureType() const {
		return type;
	}

	inline uint16 GetTextureFlags() const {
		return textureFlags;
	}

protected:
	// type
	TextureType type;
	// is an asset?
	uint16 textureFlags;
	// texture dimensions
	uint32 width, height, depth;
	// cube map faces
	uint32 faces;
	// number of mip levels this texture will support
	// when fully loaded
	uint16 numMipMaps;
	/* biggest mip level, smallest texture */
	uint16 currentMaxMipLevel;
	// format
	PixelFormat format;
};

} /* namespace nextar */
#endif /* TEXTUREBASE_H_ */
