/*
 * GpuBufferGL.cpp
 *
 *  Created on: 15-Jun-2014
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderContextGL.h>
#include <GpuBufferViewGL.h>

namespace RenderOpenGL {

/********************************
 * GpuBufferGL
 ********************************/
GpuBufferViewGL::GpuBufferViewGL(GLenum _type) : type(_type), usage(0),
		elementStride(0),
		bufferId(0), policy(GpuBuffer::NEVER_RELEASED),
		totalSize(-1), syncRequired(false) {
}

GpuBufferViewGL::~GpuBufferViewGL() {
}

void GpuBufferViewGL::Destroy(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->DestroyBuffer(bufferId);
}

void GpuBufferViewGL::Create(RenderContext* rc, size_t size,
		uint32 elementStride,
		const void* dataPtr, GpuBuffer::RelocationPolicy policy) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	usage = GL_STATIC_DRAW;
	if (policy == GpuBuffer::REGULARLY_RELEASED)
		usage = GL_STREAM_DRAW;
	else if (policy != GpuBuffer::NEVER_RELEASED)
		usage = GL_DYNAMIC_DRAW;
	bufferId = gl->CreateBuffer(size, usage, type);
	totalSize = size;
	this->elementStride = elementStride;
	if (dataPtr) {
		gl->Bind(type, bufferId);
		gl->WriteBuffer(type, size, usage, dataPtr, 0, size);
	}
}

void GpuBufferViewGL::Read(RenderContext* rc, void *dest, size_t offset,
		size_t size) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

void GpuBufferViewGL::Write(RenderContext* rc, const void *src, size_t offset,
		size_t size) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->Bind(type, bufferId);
	gl->WriteBuffer(type, totalSize, usage, src, offset, size);
}

GpuBuffer::MapResult GpuBufferViewGL::Map(RenderContext* rc,
		size_t offset, size_t size) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	GpuBuffer::MapResult result;
	result.data = static_cast<uint8*>(gl->MapRange(type, offset, size, GL_MAP_WRITE_BIT));
	return result;
}

void GpuBufferViewGL::Unmap(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->Unmap(type);
}

/********************************
 * GpuTransientBufferGL
 ********************************/
GpuTransientBufferViewGL::GpuTransientBufferViewGL(GLenum type) :
		GpuBufferViewGL(type) {
}

void GpuTransientBufferViewGL::Create(RenderContext* rc, size_t size,
		uint32 elementStride,
		const void* dataPtr, GpuBuffer::RelocationPolicy policy) {
	NEX_ASSERT (policy == GpuBuffer::REGULARLY_RELEASED ||
			policy == GpuBuffer::IMMEDIATELY_RELEASED);
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (policy == GpuBuffer::REGULARLY_RELEASED)
		usage = GL_STREAM_DRAW;
	else if (policy != GpuBuffer::NEVER_RELEASED)
		usage = GL_DYNAMIC_DRAW;
	totalSize = size;
	this->elementStride = elementStride;
	if (dataPtr) {
		bufferId = GetWritable(gl);
		gl->Bind(type, bufferId);
		gl->WriteBuffer(type, size, usage, dataPtr, 0, size);
		syncRequired = true;
	} else {
		bufferId = 0;
	}
}

GpuBuffer::MapResult GpuTransientBufferViewGL::Map(RenderContext* rc,
		size_t offset, size_t size) {
	NEX_ASSERT(!syncRequired);
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	bufferId = GetWritable(gl);
	gl->Bind(type, bufferId);
	GpuBuffer::MapResult result;
	result.data = static_cast<uint8*>(gl->MapRange(type, offset, size, GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT));
	syncRequired = true;
	return result;
}

GLuint GpuTransientBufferViewGL::GetWritable(RenderContextGL* rc) {
	if (allocatedList.size()) {
		// @optimize If more than one is free probably release the memory,
		auto& i = allocatedList.front();
		GLenum result = rc->GlClientWaitSync(i.fence, 0, 0);
		GL_CHECK();
		if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
			rc->GlDeleteSync(i.fence);
			GL_CHECK();
			GLuint bufferRet = i.buffer;
			allocatedList.pop_front();
			return bufferRet;
		}
	}
	// we either need to allocate or wait till the last one is
	// finished.
	if (allocatedList.size() < (size_t)RenderConstants::MAX_TRANSIENT_BUFFER) {
		GLuint bufferId = rc->CreateBuffer(totalSize, usage, type);
		rc->Bind(type, bufferId);
		rc->WriteBuffer(type, totalSize, usage, nullptr, 0, 0);
		return bufferId;
	} else {
		Debug("Explicit sync for buffers required!");
		auto& i = allocatedList.front();
		GLenum result;
		do {
			result = rc->GlClientWaitSync(i.fence, 0, 100);
			GL_CHECK();
		} while (result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED);
		rc->GlDeleteSync(i.fence);
		GL_CHECK();
		GLuint bufferRet = i.buffer;
		allocatedList.pop_front();
		return bufferRet;
	}
}

void GpuTransientBufferViewGL::Sync(RenderContextGL* rc) {
	GLsync sync = rc->GlFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GL_CHECK();
	allocatedList.push_back(Buffer(sync, bufferId));
	bufferId = 0;
}

void GpuTransientBufferViewGL::Destroy(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (syncRequired) {
		Sync(gl);
	}
	if (bufferId) {
		gl->DestroyBuffer(bufferId);
		bufferId = 0;
	}
	for(auto &i : allocatedList) {
		GLenum result;
		do {
			result = gl->GlClientWaitSync(i.fence, 0, 100);
			GL_CHECK();
		} while (result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED);		
		gl->GlDeleteSync(i.fence);
		gl->DestroyBuffer(i.buffer);
	}
	allocatedList.clear();
}

} /* namespace RenderOpenGL */
