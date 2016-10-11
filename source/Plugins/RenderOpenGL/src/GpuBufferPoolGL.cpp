/*
 * GpuBufferPoolGL.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: obhi
 */


#include <GpuBufferPoolGL.h>

namespace RenderOpenGL {

GpuBufferPoolGL::GpuBufferPoolGL() : _blockSize(0), _gpuObjects(nullptr) {
}

GpuBufferPoolGL::~GpuBufferPoolGL() {
}

GpuObject* GpuBufferPoolGL::_AllocContainer(RenderContext_Base_GL* gl,
		uint32 size, GLenum usage, GLenum type) {

	GLuint buffer = gl->CreateBuffer(size, usage, type);
	GpuObject *p = NEX_NEW(GpuObject(buffer, size, _gpuObjects));
	_gpuObjects = p;
	return p;
}

void GpuBufferPoolGL::Cleanup(RenderContext_Base_GL* gl) {

	GpuObject *cur = _gpuObjects;
	while (cur) {
		GpuObject *next = cur->nextInList;
		gl->DestroyBuffer(cur->nameGl);
		GpuFreeBufferNode* freedBuff = cur->freedBuffers;
		while(freedBuff) {
			GpuFreeBufferNode* releaseBuff = freedBuff;
			freedBuff = freedBuff->nextInList;
			_allocatorFreeNode.FreeType(releaseBuff);
		}
		NEX_DELETE(cur);
		cur = next;
	}
}

void GpuBufferPoolGL::_FreeContainer(RenderContext_Base_GL* gl, GpuObject* object) {
	GpuObject ** prev = &_gpuObjects;
	GpuObject *cur = _gpuObjects;
	while (cur) {
		if (cur == object) {
			*prev = cur->nextInList;
			gl->DestroyBuffer(cur->nameGl);
			GpuFreeBufferNode* freedBuff = cur->freedBuffers;
			while(freedBuff) {
				GpuFreeBufferNode* releaseBuff = freedBuff;
				freedBuff = freedBuff->nextInList;
				_allocatorFreeNode.FreeType(releaseBuff);
			}
			NEX_DELETE(cur);
			return;
		}
		prev = &cur->nextInList;
		cur = cur->nextInList;
	}
}

GpuBufferRef GpuBufferPoolGL::CreateBuffer(
		RenderContext_Base_GL* gl,
		uint32 size, GLenum usage, GLenum type) {

	const float percentLimit = (float)GpuBufferPoolGL::PERCENT_SIZE_LIMIT / 100.f;

	GpuObject* pObject = nullptr;

	uint32 maxSize = (uint32)(size * percentLimit);
	if (size < _blockSize) {
		// @todo We should start from the tail maybe and
		// keep a double linked list, but it will
		// make the pool buffer go bigger than ideal size

		for (GpuObject *cur = _gpuObjects;
				cur; cur = cur->nextInList) {
			GpuObject& c = (*cur);
			GpuFreeBufferNode** prev = &c.freedBuffers;
			GpuFreeBufferNode* fbn = c.freedBuffers;
			while (fbn) {
				if (fbn->size >= size && fbn->size < maxSize) {
					GpuBufferRef ref;
					ref.bufferId = c.nameGl;
					ref.container = &c;
					ref.offset = fbn->offset;
					ref.allocatedSize = fbn->size;
					c.freedSize -= fbn->size;
					*prev = fbn->nextInList;
					_allocatorFreeNode.FreeType(fbn);
					return ref;
				}
				prev = &fbn->nextInList;
				fbn = fbn->nextInList;
			}
			if ((c.allocatedSize - c.freeOffset) >= size) {
				pObject = &c;
				break;
			}
		}
	}

	if (pObject == nullptr) {
		pObject = _AllocContainer(gl, std::max(size, _blockSize), usage, type);
	}

	if (pObject) {
		GpuBufferRef gpuBufferRef;
		gpuBufferRef.bufferId = pObject->nameGl;
		gpuBufferRef.offset = pObject->freeOffset;
		gpuBufferRef.container = pObject;
		gpuBufferRef.allocatedSize = size;
		pObject->freeOffset += size;
		return gpuBufferRef;
	}
	return GpuBufferRef::Null;
}

void GpuBufferPoolGL::DestroyBuffer(RenderContext_Base_GL* gl,
		GpuBufferRef&& buffer) {

	GpuObject* container = buffer.container;
	buffer.bufferId = 0;
	container->freedSize += buffer.allocatedSize;
	if (container->freedSize == container->allocatedSize) {
		_FreeContainer(gl, container);
		return;
	}
	if (container->freeOffset == buffer.offset + buffer.allocatedSize)
		container->freeOffset -= buffer.allocatedSize;
	else {
		GpuFreeBufferNode* freeNode = _allocatorFreeNode.AllocType();
		freeNode->offset = buffer.offset;
		freeNode->size = buffer.allocatedSize;
		freeNode->nextInList = container->freedBuffers;
		container->freedBuffers = freeNode;
	}
}


GpuSyncedBuffer* GpuBufferPoolGL::CreateSyncedBuffer(
		RenderContext_Base_GL* gl,
		GpuBufferRef&& buffRef) {
	GLsync sync = gl->FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GpuSyncedBuffer* buff = _allocatorSyncedNodes.AllocType();
	buff->fence = sync;
	buff->buffer = buffRef;
	buffRef.bufferId = 0;
	return buff;
}

void GpuBufferPoolGL::DestroySyncedBuffer(
		RenderContext_Base_GL* gl,
		GpuSyncedBuffer* cur) {
	gl->DeleteSync(cur->fence);
	_allocatorSyncedNodes.FreeType(cur);
}

}
