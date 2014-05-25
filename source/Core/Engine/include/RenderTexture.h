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

	class RenderTexture:
			public TextureBase,
			public RenderTarget {

	public:
		struct Params {
			Size dimensions;
			PixelFormat format;
		};

		RenderTexture();
		virtual ~RenderTexture();


		virtual Size GetDimensions() const;
		virtual PixelFormat GetPixelFormat() const;
		/* update */
		virtual void NotifyUpdated(UpdateParamPtr);
		virtual void Update(nextar::RenderContext*, UpdateParamPtr);
	protected:

		/* throws INVALID_CALL */
		virtual void WriteBoxImpl(RenderContext*, size_t faceNum, size_t mipNum,
			            PixelBox& box) = 0;
	};

} /* namespace nextar */
#endif /* RENDERTEXTURE_H_ */
