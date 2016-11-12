/*
 * GpuBufferPool.h
 *
 *  Created on: Oct 9, 2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDEROPENGL_LOCAL_INCLUDE_GPUBUFFERPOOLGL_H_
#define PLUGINS_RENDEROPENGL_LOCAL_INCLUDE_GPUBUFFERPOOLGL_H_

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
		, buffer(_buffer)
		, nextInList(nullptr) {}
};

struct GpuFreeBufferNode {
	uint32 offset;
	uint32 size;
	GpuFreeBufferNode* nextInList;
};

class GpuObject;

typedef AllocPooledObjectBase< PooledAllocator<GpuObject, 16, MEMCAT_GENERAL> > GpuObjectAllocator;
class GpuObject : public GpuObjectAllocator{
public:

	GpuObject(GLuint gl, uint32 size, GpuObject* head) :
		nameGl(gl),
		allocatedSize(size),
		freeOffset(0),
		freedSize(0),
		freedBuffers(nullptr),
		nextInList(head) {
	}

	GLuint nameGl;
	uint32 allocatedSize;
	uint32 freeOffset;
	uint32 freedSize;
	GpuFreeBufferNode* freedBuffers;
	GpuObject* nextInList;
};

class GpuBufferPoolGL : public AllocGeneral {
public:

	enum {
		NUM_FREEDNODE_PER_BLOCK = BaseConstants::NUM_16B_PER_BLOCK,
		PERCENT_SIZE_LIMIT = 125,
	};

	typedef TypedMemoryPool<GpuFreeBufferNode, NUM_FREEDNODE_PER_BLOCK, AllocatorGeneral, NullMutex> FreeNodeAllocator;
	typedef TypedMemoryPool<GpuSyncedBuffer, 64, AllocatorGeneral, NullMutex> SyncedBufferAllocator;

	inline void SetBlockSize(uint32 size) {
		_blockSize = size;
	}

	GpuBufferRef CreateBuffer(RenderContext_Base_GL* gl, uint32 size, GLenum usage, GLenum type);
	void DestroyBuffer(RenderContext_Base_GL* gl, GpuBufferRef&& buffer);
	GpuSyncedBuffer* CreateSyncedBuffer(RenderContext_Base_GL* gl, GpuBufferRef&&);
	void DestroySyncedBuffer(RenderContext_Base_GL* gl, GpuSyncedBuffer*);
	void Cleanup(RenderContext_Base_GL* gl);

	GpuBufferPoolGL();
	~GpuBufferPoolGL();
protected:

	GpuObject* _AllocContainer(RenderContext_Base_GL* gl, uint32 size, GLenum usage, GLenum type);
	void _FreeContainer(RenderContext_Base_GL* gl, GpuObject* object);


protected:

	uint32 _blockSize;

	SyncedBufferAllocator _allocatorSyncedNodes;
	FreeNodeAllocator _allocatorFreeNode;
	GpuObject* _gpuObjects;
};

} /* namespace RenderOpenGL */

#endif /* PLUGINS_RENDEROPENGL_LOCAL_INCLUDE_GPUBUFFERPOOLGL_H_ */
