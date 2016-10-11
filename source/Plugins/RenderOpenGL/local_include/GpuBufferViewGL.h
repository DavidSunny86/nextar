/*
 * GpuBufferGL.h
 *
 *  Created on: 15-Jun-2014
 *      Author: obhi
 */

#ifndef GPUBUFFERVIEWGL_H_
#define GPUBUFFERVIEWGL_H_

#include <RenderOpenGL.h>
#include <GpuBuffer.h>

namespace RenderOpenGL {

class GpuBufferPoolGL;
class GpuObject;

struct GpuBufferRef {
	uint32 offset;
	uint32 allocatedSize;
	GLuint bufferId;
	GpuObject* container;
	static GpuBufferRef Null;
};

struct GpuSyncedBuffer {
	GLsync fence;
	GpuBufferRef buffer;
	GpuSyncedBuffer* nextInList;

	inline GpuSyncedBuffer(GLsync _fence, const GpuBufferRef& _buffer) :
	fence(_fence)
	,buffer(_buffer)
	,nextInList(nullptr) {}
};

class GpuBufferViewGL : public GpuBuffer::View {
public:
	GpuBufferViewGL(GLenum type);
	virtual ~GpuBufferViewGL();

	virtual void Create(RenderContext* rc, uint32 size,
			uint32 elementStride,
			const void* dataPtr, GpuBuffer::RelocationPolicy policy);
	virtual void Read(RenderContext* rc, void *dest, uint32 offset,
			uint32 size);
	virtual void Write(RenderContext* rc, const void *src, uint32 offset,
			uint32 size);
	virtual GpuBuffer::MapResult Map(RenderContext* rc, uint32 offset = 0, uint32 size = 0);
	virtual void Unmap(RenderContext* rc);
	virtual void Destroy(RenderContext* rc);

	bool IsSyncRequired() const {
		return syncRequired;
	}

	GLuint GetStride() const {
		return elementStride;
	}

	GLuint GetBufferId() const {
		NEX_ASSERT(ref.bufferId);
		return ref.bufferId;
	}

	GLenum GetUsage() const {
		return usage;
	}

protected:
	bool syncRequired;
	GpuBuffer::RelocationPolicy policy;
	GpuBufferRef ref;
	GLenum usage;
	uint32 size;
	GLuint elementStride;
	GLenum type;

	GpuBufferPoolGL* pool;
};

class GpuTransientBufferViewGL : public GpuBufferViewGL {
	NEX_LOG_HELPER(GpuTransientBufferViewGL);
public:
	GpuTransientBufferViewGL(GLenum type);

	virtual void Create(RenderContext* rc, uint32 size,
			uint32 elementStride,
			const void* dataPtr, GpuBuffer::RelocationPolicy policy);
	virtual GpuBuffer::MapResult Map(RenderContext* rc, uint32 offset = 0, uint32 size = 0);
	virtual void Destroy(RenderContext* rc);
	void Sync(RenderContext_Base_GL* rc);

protected:

	void GetWritable(RenderContext_Base_GL* rc);
	uint32 numSyncBuffers;
	GpuSyncedBuffer* posInList;
};

} /* namespace RenderOpenGL */

#endif /* GPUBUFFERVIEWGL_H_ */
