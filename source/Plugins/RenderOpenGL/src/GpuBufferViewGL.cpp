/*
 * GpuBufferGL.cpp
 *
 *  Created on: 15-Jun-2014
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderContext_Base_GL.h>
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
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->DestroyBuffer(bufferId);
}

void GpuBufferViewGL::Create(RenderContext* rc, size_t size,
		uint32 elementStride,
		const void* dataPtr, GpuBuffer::RelocationPolicy policy) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
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
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

void GpuBufferViewGL::Write(RenderContext* rc, const void *src, size_t offset,
		size_t size) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->Bind(type, bufferId);
	gl->WriteBuffer(type, totalSize, usage, src, offset, size);
}

GpuBuffer::MapResult GpuBufferViewGL::Map(RenderContext* rc,
		size_t offset, size_t size) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	GpuBuffer::MapResult result;
	result.data = static_cast<uint8*>(gl->MapRange(type, offset, size, GL_MAP_WRITE_BIT));
	return result;
}

void GpuBufferViewGL::Unmap(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
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
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
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
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	bufferId = GetWritable(gl);
	gl->Bind(type, bufferId);
	GpuBuffer::MapResult result;
	result.data = static_cast<uint8*>(gl->MapRange(type, offset, size, GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT));
	syncRequired = true;
	return result;
}

GLuint GpuTransientBufferViewGL::GetWritable(RenderContext_Base_GL* rc) {
	if (allocatedList.size()) {
		// @optimize If more than one is free probably release the memory,
		auto& i = allocatedList.front();
		GLenum result = rc->ClientWaitSync(i.fence, 0, 0);

		if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
			rc->DeleteSync(i.fence);

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
			result = rc->ClientWaitSync(i.fence, 0, 100);
		} while (result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED);
		rc->DeleteSync(i.fence);

		GLuint bufferRet = i.buffer;
		allocatedList.pop_front();
		return bufferRet;
	}
}

void GpuTransientBufferViewGL::Sync(RenderContext_Base_GL* rc) {
	GLsync sync = rc->FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	allocatedList.push_back(Buffer(sync, bufferId));
	bufferId = 0;
}

void GpuTransientBufferViewGL::Destroy(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
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
			result = gl->ClientWaitSync(i.fence, 0, 100);
		} while (result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED);
		gl->DeleteSync(i.fence);
		gl->DestroyBuffer(i.buffer);
	}
	allocatedList.clear();
}

} /* namespace RenderOpenGL */
