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

	class TextureBase: public nextar::ContextObject {
	public:

		enum TextureType {
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_3D,
			TEXTURE_CUBE_MAP,
			TEXTURE_RENDER_TARGET,
			TEXTURE_TYPE_UNKNOWN
		};

		enum TextureFlags {
			/* Indicates its a texture asset */
			TEXTURE_ASSET = 1 << 1,
			/* Automatically generates all mip levels */
			AUTOGEN_MIP_MAPS_ENABLED = 1 << 2,
		};

		TextureBase(uint16 flags = 0);
		virtual ~TextureBase();

		inline float GetTextureLod() const {
			return (float)currentMaxMipLevel / (float)numMipMaps;
		}

		inline bool IsTextureAsset() const {
			return (textureFlags & TEXTURE_ASSET) != 0;
		}

	protected:

	    virtual void WriteBoxImpl(RenderContext*, size_t faceNum, size_t mipNum,
	            PixelBox& box) = 0;

		// type
		uint8 type;
		// is an asset?
		uint8 textureFlags;
		// texture dimensions
		uint16 width, height, depth;
		// cube map faces
		uint16 faces;
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
