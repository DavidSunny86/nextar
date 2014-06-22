/* 
 * File:   BufferManagerGL.h
 * Author: obhi
 *
 * Created on February 21, 2011, 10:21 PM
 */

#ifndef GLBUFFERMANAGER_H
#define	GLBUFFERMANAGER_H

#include <BufferManager.h>
#include <VertexBufferGL.h>
#include <IndexBufferGL.h>
#include <VertexElementGL.h>

namespace RenderOpenGL {

class BufferManagerGL: public BufferManager {
	typedef multimap<GLuint, GLuint>::type FreeBufferPool;

public:
	BufferManagerGL(size_t memoryBudget, uint32 releaseFrameThreshold);
	virtual ~BufferManagerGL();

	virtual VertexLayout* CreateVertexLayoutImpl(bool usesDynamicBinding);

	virtual VertexBufferPtr CreateVertexBuffer(size_t bufferSize,
			uint32 accessFlags, RelocationPolicy policy);

	virtual IndexBufferPtr CreateIndexBuffer(size_t bufferSize,
			uint32 accessFlags,
			IndexBuffer::Type type = IndexBuffer::TYPE_16BIT);

	GLuint GetFreeBuffer(GLuint size) {
		FreeBufferPool::iterator it = freeBuffers.find(size);
		if (it != freeBuffers.end()) {
			freeBuffers.erase(it);
			return (*it).second;
		}
		return 0;
	}

	void SetFreeBuffer(GLuint size, GLuint bufferId) {
		freeBuffers.insert(FreeBufferPool::value_type(size, bufferId));
	}

	/** Free unused buffers */
	virtual void FreeUnusedBuffers();

	/* Push a new temp buffer */
	void AddTempBuffer(VertexBufferGL* b) {
		tempBuffers.push_back(b);
	}

	/* Push a new temp buffer */
	inline void RemoveTempBuffer(VertexBufferGL* b) {
		BestErase(tempBuffers,
				std::find(tempBuffers.begin(), tempBuffers.end(), b));
	}

	inline void TouchVertexBuffer(VertexBufferGL* buffer) {
		buffer->SetFrameThreshold(this->GetReleaseFrameThreshold());
	}

private:

	uint32 memoryInUse;
	uint32 memoryBudget;
	uint16 frameThreshold;

	typedef vector<VertexBufferGL*>::type TemporaryBufferPool;
	TemporaryBufferPool tempBuffers;
	FreeBufferPool freeBuffers;

	NEX_LOG_HELPER(BufferManagerGL)
	;
};
}
#endif	/* GLBUFFERMANAGER_H */

