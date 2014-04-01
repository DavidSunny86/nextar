/* 
* File:   IndexBufferGL.cpp
* Author: obhi
* 
* Created on March 8, 2011, 8:19 PM
*/
#include <RenderOpenGL.h>
#include <BufferManagerGL.h>
#include <IndexBufferGL.h>
#include <RenderContextGL.h>

#ifdef NEX_DEBUG
static bool IndexBuffer_locked = false;
#endif


namespace RenderOpenGL {

	IndexBufferGL::IndexBufferGL(size_t sz, 
		uint32 flags, IndexBuffer::Type type) :	IndexBuffer(sz, flags, type), bufferId(0) {
		usage = (flags & Buffer::CPU_WRITE) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	}

	IndexBufferGL::~IndexBufferGL() {
	}

	void IndexBufferGL::_Create(RenderContext* rc) {
		/* check if we are shared */
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		
		if (!bufferId) {
			bufferId = rcGL->CreateIndexBuffer(GetSize(), usage);
		}
	}

	void IndexBufferGL::_Destroy(RenderContext* rc) {
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		if(bufferId) {
			rcGL->DestroyIndexBuffer(bufferId);
			bufferId = 0;
		}
	}
	
	void IndexBufferGL::Read(RenderContext* rc, void *dest, size_t offset, size_t size) {
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		if (bufferId) {
			rcGL->Bind(GL_ELEMENT_ARRAY_BUFFER, bufferId);
			rcGL->ReadBuffer(GL_ELEMENT_ARRAY_BUFFER, dest, offset, size);
		}
	}

	void IndexBufferGL::Write(RenderContext* rc, const void *src, size_t offset, size_t size) {
		if (!bufferId)
			_Create(rc);
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		rcGL->Bind(GL_ELEMENT_ARRAY_BUFFER, bufferId);
		rcGL->WriteBuffer(GL_ELEMENT_ARRAY_BUFFER, GetSize(), usage, src, offset, size);
	}

	void IndexBufferGL::CopyFrom(RenderContext* rc, BufferPtr& ptr) {
		if (!bufferId)
			_Create(rc);
		
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		IndexBufferGL* other = (IndexBufferGL*) ptr.GetPtr();
		NEX_ASSERT(size == other->size);
		rcGL->CopyBuffer(other->bufferId, bufferId, size);

	}
}
