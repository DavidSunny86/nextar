/*
 * RenderTexture.h
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERTEXTURE_H_
#define RENDERTEXTURE_H_

#include <RenderTarget.h>
#include <TextureBase.h>

namespace nextar {

class RenderTexture: public TextureBase, public RenderTarget {

public:

	struct CreateParams {
		TextureType type;
		PixelFormat format;
		uint32 width;
		uint32 height;
		uint32 depth;
	};

	RenderTexture();
	virtual ~RenderTexture();

	inline void Create(const CreateParams& params) {
		Create(params.type, params.format, params.width, params.height, params.depth);
	}

	virtual Size GetDimensions() const;
	virtual PixelFormat GetPixelFormat() const;

	virtual void Create(TextureType type, PixelFormat format, uint32 width,
			uint32 height, uint32 depth);
	virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer);
	
protected:

};

} /* namespace nextar */
#endif /* RENDERTEXTURE_H_ */
