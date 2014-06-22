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

class GpuBufferViewGL : public GpuBuffer::View {
public:
	GpuBufferViewGL(GLenum type);
	virtual ~GpuBufferViewGL();
	virtual void Create(RenderContext* rc, size_t size,
			uint32 elementStride,
			const void* dataPtr, GpuBuffer::RelocationPolicy policy);
	virtual void Read(RenderContext* rc, void *dest, size_t offset,
			size_t size);
	virtual void Write(RenderContext* rc, const void *src, size_t offset,
			size_t size);
	virtual GpuBuffer::MapResult Map(RenderContext* rc, size_t offset = 0, size_t size = 0);
	virtual void Unmap(RenderContext* rc);
	virtual void Destroy(RenderContext* rc);

	bool IsSyncRequired() const {
		return syncRequired;
	}

	GLuint GetStride() const {
		return elementStride;
	}

	GLuint GetBufferId() const {
		NEX_ASSERT(bufferId);
		return bufferId;
	}

	GLenum GetUsage() const {
		return usage;
	}

protected:
	GpuBuffer::RelocationPolicy policy;
	bool syncRequired;
	size_t totalSize;
	GLenum type;
	GLenum usage; // either GL_DYNAMIC_DRAW or GL_STATIC_DRAW
	GLuint elementStride;
	GLuint bufferId;
};

class GpuTransientBufferViewGL : public GpuBufferViewGL {
	NEX_LOG_HELPER(GpuTransientBufferViewGL);
public:
	GpuTransientBufferViewGL(GLenum type);

	virtual void Create(RenderContext* rc, size_t size,
			uint32 elementStride,
			const void* dataPtr, GpuBuffer::RelocationPolicy policy);
	virtual GpuBuffer::MapResult Map(RenderContext* rc, size_t offset = 0, size_t size = 0);
	virtual void Destroy(RenderContext* rc);
	void Sync(RenderContextGL* rc);

protected:
	struct Buffer;

	GLuint GetWritable(RenderContextGL* rc);

	struct Buffer {
		GLsync fence;
		GLuint buffer;
	};

	typedef STDPoolAllocator<Buffer,
			16,
			MEMCAT_GENERAL> BufferAllocator;

	typedef list<Buffer, BufferAllocator>::type BufferList;
	BufferList allocatedList;
};

} /* namespace RenderOpenGL */

#endif /* GPUBUFFERVIEWGL_H_ */
