/* 
* File:   VertexBufferGL.cpp
* Author: obhi
* 
* Created on February 21, 2011, 10:05 PM
*/
#include <RenderOpenGL.h>
#include <VertexBufferGL.h>
#include <BufferManagerGL.h>
#include <RenderContextGL.h>

#ifdef NEX_DEBUG
static bool VertexBuffer_locked = false;
#endif

namespace RenderOpenGL {

	VertexBufferGL::VertexBufferGL(size_t bufferSize, uint32 accessFlags, RelocationPolicy policy) : VertexBuffer(bufferSize, accessFlags, policy),
		bufferId(0) {
			usage = (accessFlags & Buffer::CPU_WRITE) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	}

	VertexBufferGL::~VertexBufferGL() {
	}

	void VertexBufferGL::_Create(RenderContext* rc) {
		/* check if we are shared */
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		if (GetPolicy() != RelocationPolicy::NEVER_RELEASE) {
			BufferManagerGL* manager = static_cast<BufferManagerGL*>(BufferManager::InstancePtr());
			manager->AddTempBuffer(this);
			bufferId = manager->GetFreeBuffer(GetSize());
		}

		if (!bufferId) {
			bufferId = rcGL->CreateVertexBuffer(GetSize(), usage);
		}
	}

	void VertexBufferGL::_Destroy(RenderContext* rc) {

		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		if (bufferId) {
			if (GetPolicy() != RelocationPolicy::NEVER_RELEASE) {
				BufferManagerGL* manager = static_cast<BufferManagerGL*>(BufferManager::InstancePtr());
				manager->RemoveTempBuffer(this);
				manager->SetFreeBuffer(GetSize(), bufferId);
			} else
				rcGL->DestroyVertexBuffer(bufferId);
		}

		bufferId = 0;
	}

	void VertexBufferGL::Read(RenderContext* rc, void *dest, size_t offset, size_t size) {
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		if (bufferId) {
			rcGL->ReadBuffer(GL_ARRAY_BUFFER, bufferId, dest, offset, size);
		}
	}

	void VertexBufferGL::Write(RenderContext* rc, const void *src, size_t offset, size_t size) {
		if (!bufferId)
			_Create(rc);
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		rcGL->WriteBuffer(GL_ARRAY_BUFFER, bufferId, GetSize(), usage, src, offset, size);
	}

	void VertexBufferGL::CopyFrom(RenderContext* rc, BufferPtr& ptr) {
		if (!bufferId)
			_Create(rc);
		
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		VertexBufferGL* other = (VertexBufferGL*) ptr.GetPtr();
		NEX_ASSERT(size == other->size);
		rcGL->CopyBuffer(other->bufferId, bufferId, size);

	}


}