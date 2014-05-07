/* 
* File:   BufferManagerGL.cpp
* Author: obhi
*
* Created on February 21, 2011, 10:21 PM
*/

#include <RenderOpenGL.h>
#include <BufferManagerGL.h>
#include <VertexElementGL.h>
#include <IndexBufferGL.h>
#include <RenderContextGL.h>

namespace RenderOpenGL {

	BufferManagerGL::BufferManagerGL(size_t memoryBudget, uint32 rft) : BufferManager(memoryBudget, rft) {
	}

	BufferManagerGL::~BufferManagerGL() {
		// clear up the released buffers
	}
	
	VertexLayout* BufferManagerGL::CreateVertexLayoutImpl(bool usesDynamicBuffers) {
			if (usesDynamicBuffers)  
				return NEX_NEW(VertexLayoutDynamicGL());
			else
				return NEX_NEW(VertexLayoutStaticGL());
	}
	
	VertexBufferPtr BufferManagerGL::CreateVertexBuffer(
		size_t bufferSize,
		uint32 accessFlags,
		RelocationPolicy policy
		) {

			VertexBufferGL* vb = NEX_NEW(VertexBufferGL(bufferSize, accessFlags, policy));

			if (policy != RelocationPolicy::NEVER_RELEASE)
				tempBuffers.push_back(vb);

			memoryInUse += bufferSize;
			if (memoryBudget < memoryInUse)
				Warn("Exceeding memory budget! todo Free unused buffers");

			return Assign(vb);
	}

	IndexBufferPtr BufferManagerGL::CreateIndexBuffer(
		size_t bufferSize,
		uint32 accessFlags,
		IndexBuffer::Type type
		) {

			IndexBufferGL* ib = NEX_NEW(IndexBufferGL(bufferSize, accessFlags, type));

			memoryInUse += bufferSize;
			if (memoryBudget < memoryInUse)
				Warn("Exceeding memory budget!");

			return Assign(ib);
	}

	void BufferManagerGL::FreeUnusedBuffers() {
		TemporaryBufferPool::const_iterator en = tempBuffers.end();
		for (TemporaryBufferPool::iterator it = tempBuffers.begin();
			en != it;) {
				VertexBufferGL* buff = (*it);
				if ( buff->policy == RelocationPolicy::RELEASE_IMMEDIATELY ||
					!(--buff->frameThreshold) ) {
						SetFreeBuffer( buff->GetSize(), buff->GetBufferId() );
						buff->bufferId = 0;
						it = tempBuffers.erase(it);
				} else
					++it;
		}
	}
}