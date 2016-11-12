/*
* GpuBufferGL.cpp
*
*  Created on: 15-Jun-2014
*      Author: obhi
*/
#include <RenderOpenGL.h>
#include <RenderContext_Base_GL.h>
#include <GpuBufferViewGL.h>
#include <GpuBufferPoolGL.h>

namespace RenderOpenGL {

/********************************
* GpuBufferGL
********************************/
GpuBufferViewGL::GpuBufferViewGL(GLenum _type) :
type(_type),
elementStride(0),
policy(GpuBuffer::NEVER_RELEASED),
syncRequired(false),
pool(nullptr),
usage(0),
size(0) {
	ref.bufferId = 0;
	ref.offset = 0;
}

GpuBufferViewGL::~GpuBufferViewGL() {
}

void GpuBufferViewGL::Destroy(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	pool->DestroyBuffer(gl, std::move(ref));
}

void GpuBufferViewGL::Create(RenderContext* rc, uint32 size,
	uint32 elementStride,
	const void* dataPtr, GpuBuffer::RelocationPolicy policy) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	this->policy = policy;
	auto usageAndPool = gl->GetUsage(policy, type);
	usage = usageAndPool.first;
	pool = usageAndPool.second;
	ref = pool->CreateBuffer(gl, size, usage, type);
	this->elementStride = elementStride;
	if (dataPtr) {
		gl->Bind(type, ref.bufferId);
		gl->WriteBuffer(type, size, usage, dataPtr, ref.offset, size);
	}
}

void GpuBufferViewGL::Read(RenderContext* rc, void *dest, uint32 offset,
	uint32 size) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

void GpuBufferViewGL::Write(RenderContext* rc, const void *src, uint32 offset,
	uint32 size) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->Bind(type, ref.bufferId);
	gl->WriteBuffer(type, size, usage, src, ref.offset + offset, size);
}

GpuBuffer::MapResult GpuBufferViewGL::Map(RenderContext* rc,
	uint32 offset, uint32 size) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	GpuBuffer::MapResult result;
	result.data = static_cast<uint8*>(gl->MapRange(type, ref.offset + offset, size, GL_MAP_WRITE_BIT));
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
GpuBufferViewGL(type), posInList(nullptr), numSyncBuffers(0) {
}

void GpuTransientBufferViewGL::Create(RenderContext* rc, uint32 size,
	uint32 elementStride,
	const void* dataPtr, GpuBuffer::RelocationPolicy policy) {
	NEX_ASSERT(policy == GpuBuffer::REGULARLY_RELEASED ||
		policy == GpuBuffer::IMMEDIATELY_RELEASED);
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	this->policy = policy;
	this->elementStride = elementStride;
	auto usageAndPool = gl->GetUsage(policy, type);
	usage = usageAndPool.first;
	pool = usageAndPool.second;
	if (dataPtr) {
		GetWritable(gl);
		gl->Bind(type, ref.bufferId);
		gl->WriteBuffer(type, size, usage, dataPtr, ref.offset, size);
		syncRequired = true;
	} else {
		ref.bufferId = 0;
	}
}

GpuBuffer::MapResult GpuTransientBufferViewGL::Map(RenderContext* rc,
	uint32 offset, uint32 size) {
	NEX_ASSERT(!syncRequired);
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (!syncRequired || ref.bufferId == 0)
		GetWritable(gl);
	gl->Bind(type, ref.bufferId);
	GpuBuffer::MapResult result;
	result.data = static_cast<uint8*>(gl->MapRange(type, ref.offset + offset, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
	syncRequired = true;
	return result;
}

void GpuTransientBufferViewGL::Destroy(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);

	if (syncRequired) {
		Sync(gl);
	}

	if (ref.bufferId) {
		pool->DestroyBuffer(gl, std::move(ref));
	}

	if (posInList) {
		GpuSyncedBuffer* prev = posInList;
		GpuSyncedBuffer* cur = posInList->nextInList;
		do {
			GLenum result;
			do {
				result = gl->ClientWaitSync(cur->fence, 0, 100);
			} while (result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED);

			pool->DestroyBuffer(gl, std::move(cur->buffer));
			prev = cur;
			cur = cur->nextInList;
			pool->DestroySyncedBuffer(gl, prev);
		} while (prev != posInList);
		posInList = nullptr;
	}
}

void GpuTransientBufferViewGL::Sync(RenderContext_Base_GL* rc) {
	GpuSyncedBuffer* syncedBuffer = pool->CreateSyncedBuffer(rc, std::move(ref));
	syncRequired = false;
	GpuSyncedBuffer* head = syncedBuffer;

	if (posInList) {
		head = posInList->nextInList;
		posInList->nextInList = syncedBuffer;
		numSyncBuffers++;
	}
	posInList = syncedBuffer;
	posInList->nextInList = head;

}

void GpuTransientBufferViewGL::GetWritable(RenderContext_Base_GL* rc) {
	if (posInList) {
		bool explicitSync = (numSyncBuffers >= (uint32)RenderConstants::MAX_TRANSIENT_BUFFER) != 0;
		// @optimize If more than one is free probably release the memory,
		GpuSyncedBuffer* prev = posInList;
		GpuSyncedBuffer* cur = posInList->nextInList;
		do {
			GLenum result;
			do {
				result = rc->ClientWaitSync(cur->fence, 0, 0);
			} while ((result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED) && explicitSync);
			if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
				ref = std::move(cur->buffer);
				pool->DestroySyncedBuffer(rc, cur);
				if (prev != cur)
					prev->nextInList = cur->nextInList;
				else {
					posInList = nullptr;
				}
				--numSyncBuffers;
				return;
			}
			prev = cur;
			cur = cur->nextInList;
		} while (prev != posInList);

	}
	// we either need to allocate or wait till the last one is
	// finished.
	ref = pool->CreateBuffer(rc, size, usage, type);
	return;
}


} /* namespace RenderOpenGL */
