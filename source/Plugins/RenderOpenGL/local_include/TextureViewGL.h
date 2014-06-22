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
	bool isCompressed;bool isDepthSencil;
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
		return texture == 0;
	}
protected:

	PixelFormatGl pixelFormat;
	GLenum target;
	GLuint texture;
};

} /* namespace RenderOpenGL */

#endif /* TEXTUREVIEWGL_H_ */
