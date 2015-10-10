/*
 * TextureViewGL.h
 *
 *  Created on: 25-May-2014
 *      Author: obhi
 */

#ifndef TEXTUREVIEWGL_H_
#define TEXTUREVIEWGL_H_

#include <RenderOpenGL.h>
#include <TextureBase.h>

namespace RenderOpenGL {

struct PixelFormatGl {
	bool isCompressed : 1;
	bool isDepthSencil : 1;
	uint8 pixelSize;
	PixelFormat textureFormat;
	GLenum attachmentType;
	GLenum internalFormat;
	GLenum sourceFormat;
	GLenum dataType;
};

class TextureViewGL: public nextar::TextureBase::View {
	NEX_LOG_HELPER(TextureViewGL)
	;

public:

	TextureViewGL();
	virtual ~TextureViewGL();

	virtual void Update(RenderContext*, uint32 msg,
			ContextObject::ContextParamPtr);
	virtual void Destroy(RenderContext*);

	GLenum GetAttachment() const {
		return pixelFormat.attachmentType;
	}

	GLenum GetType() const {
		return target;
	}

	GLuint GetTexture() const {
		return texture;
	}

	bool IsCreated() const {
		return texture != 0;
	}

	bool IsColorTarget() const {
		return !pixelFormat.isDepthSencil;
	}

	bool IsDepthStencilTarget() const {
		return pixelFormat.isDepthSencil;
	}

	void ReadPixels(RenderContext_Base_GL*, TextureBase::ReadPixelUpdateParams& box);
protected:

	PixelFormatGl pixelFormat;
	GLenum target;
	GLuint texture;
};

} /* namespace RenderOpenGL */

#endif /* TEXTUREVIEWGL_H_ */
