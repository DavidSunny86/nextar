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

namespace nextar {

	class TextureImpl {
	public:
		virtual void WriteBoxImpl(RenderContext*, size_t faceNum, size_t mipNum,
	            PixelBox& box) = 0;

	protected:
		~TextureImpl() {}
	};

	class TextureBase: public nextar::ContextObject {
	public:

		enum TextureType {
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
			/* Use mutable storage, allocated when streaming individual mip levels */
			USE_MUTABLE_STORAGE = 1 << 3,
		};

		TextureBase(TextureImpl* impl, uint16 flags = 0);

		inline float GetTextureLod() const {
			return (float)currentMaxMipLevel / (float)numMipMaps;
		}

		inline bool IsTextureAsset() const {
			return (textureFlags & TEXTURE_ASSET) != 0;
		}

	protected:
		virtual ~TextureBase();

	    virtual void WriteBoxImpl(RenderContext*, size_t faceNum, size_t mipNum,
	            PixelBox& box) = 0;

	    TextureImpl* impl;
		// type
		uint16 type;
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
