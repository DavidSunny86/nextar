/* 
 * File:   GlVertexBuffer.h
 * Author: obhi
 *
 * Created on February 21, 2011, 10:05 PM
 */

#ifndef GLVERTEXBUFFER_H
#define	GLVERTEXBUFFER_H

#include <VertexBuffer.h>

namespace RenderOpenGL {

	class VertexBufferGL : public VertexBuffer {
	public:

		VertexBufferGL(size_t bufferSize, uint32 accessFlags, RelocationPolicy);
		virtual ~VertexBufferGL();

		virtual void Read(RenderContext* rc, void *dest, size_t offset = 0, size_t size = 0);
		virtual void Write(RenderContext* rc, const void *src, size_t offset = 0, size_t size = 0);
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
		
	protected:

		friend class BufferManagerGL;
		GLenum usage; // either GL_DYNAMIC_DRAW or GL_STATIC_DRAW
		GLuint bufferId;
	};

	typedef RefPtr<VertexBufferGL> VertexBufferGLPtr;

}

#endif	/* GLVERTEXBUFFER_H */



