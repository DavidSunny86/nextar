/* 
 * File:   GlIndexBuffer.h
 * Author: obhi
 *
 * Created on March 8, 2011, 8:19 PM
 */

#ifndef GLINDEXBUFFER_H
#define	GLINDEXBUFFER_H

#include <IndexBuffer.h>

namespace RenderOpenGL {

class IndexBufferGL: public IndexBuffer {
	NEX_LOG_HELPER(GlIndexBuffer)
	;
public:

	IndexBufferGL(size_t sz, uint32 flags, IndexBuffer::Type type);
	virtual ~IndexBufferGL();

	virtual void Read(RenderContext* rc, void *dest, size_t offset = 0,
			size_t size = 0);
	virtual void Write(RenderContext* rc, const void *src, size_t offset = 0,
			size_t size = 0);
	virtual void CopyFrom(RenderContext* rc, BufferPtr&);

	void _Create(RenderContext* rc);
	void _Destroy(RenderContext* rc);

	GLuint GetBufferId() const {
		NEX_ASSERT(bufferId);
		return bufferId;
	}

	GLenum GetUsage() const {
		return usage;
	}

private:

	GLenum usage; // either GL_DYNAMIC_DRAW or GL_STATIC_DRAW
	GLuint bufferId;
};

}
#endif	/* GLINDEXBUFFER_H */

