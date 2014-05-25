/*
 * TextureAssetGL.h
 *
 *  Created on: 25-May-2014
 *      Author: obhi
 */

#ifndef TEXTUREASSETGL_H_
#define TEXTUREASSETGL_H_

#include <TextureAsset.h>

namespace RenderOpenGL {

	class TextureAssetGL: public nextar::TextureAsset {
	public:
		TextureAssetGL();
		virtual ~TextureAssetGL();

	protected:

		virtual void WriteBoxImpl(RenderContext*, size_t faceNum, size_t mipNum,
			            PixelBox& box);

	};

} /* namespace RenderOpenGL */

#endif /* TEXTUREASSETGL_H_ */
