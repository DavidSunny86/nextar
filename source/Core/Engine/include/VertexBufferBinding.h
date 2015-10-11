/**
 * Created on: 8th Oct 2015
 * @author: obhi
 **/
#ifndef VERTEXBUFFERBINDING_H_
#define VERTEXBUFFERBINDING_H_

#include <RenderConstants.h>
#include <VertexBuffer.h>

namespace nextar {

// @todo Do sampling of number of buffers used per binding to see
// what would be a good size of NUM_FIXED_BUFFER
// @optimize Most pointers are of few bytes size. Can be taken as object.
// Need better data structures to account for small object pointers here.
class _NexEngineAPI VertexBufferBinding : public AllocGraphics {
public:
	enum {
		NUM_FIXED_BUFFER = 1,
	};

	VertexBufferBinding();
	~VertexBufferBinding() {
		if (listOfBuffers != fixedBuffer)
			NEX_DELETE_ARRAY(listOfBuffers);
	}

	inline uint16 GetBindingNumber() const {
		return bindingNumber;
	}

	/** @remarks Returns the count of vertex buffers */
	inline uint32 GetBufferCount() const {
		return numBuffers;
	}

	/** @remarks Returns the buffer stored at a specified index */
	inline VertexBufferPtr& GetBuffer(uint32 i) {
		NEX_ASSERT(i < numBuffers);
		return listOfBuffers[i].buffer;
	}

	inline uint32 GetBufferOffset(uint32 i) const {
		NEX_ASSERT(i < numBuffers);
		return listOfBuffers[i].offset;
	}

	inline const VertexBuffer* GetBufferPtr(uint32 i) const {
		NEX_ASSERT(i < numBuffers);
		return listOfBuffers[i].buffer;
	}

	inline VertexBuffer* GetBufferPtr(uint32 i) {
		NEX_ASSERT(i < numBuffers);
		return listOfBuffers[i].buffer;
	}

	/** @remarks Set buffer count, this must be called before binding
	* Use VertexLayout::GetBufferCount to find the number of seperate
	* buffers.
	**/
	inline void SetBufferCount(uint32 num) {
		if (listOfBuffers != fixedBuffer) {
			NEX_DELETE_ARRAY(listOfBuffers);
		} else {
			for (uint32 i = 0; i < NUM_FIXED_BUFFER; ++i)
				fixedBuffer[i].buffer.Clear();
		}
		numBuffers = num;
		if (num <= NUM_FIXED_BUFFER)
			listOfBuffers = fixedBuffer;
		else if (num > 1) // not 0 check
			listOfBuffers = NEX_NEW(BindBufferInfo[num]);
	}

	/** @remarks Binds the buffer to the specified index */
	inline void BindBuffer(uint32 index, VertexBufferPtr vb, uint32 offset) {
		NEX_ASSERT(index < numBuffers);
		BindBufferInfo& bbi = listOfBuffers[index];
		if (bbi.buffer &&
			bbi.buffer->IsTransientBuffer())
			--transientBufferCount;
		bbi.buffer = vb;
		if (bbi.buffer &&
			bbi.buffer->IsTransientBuffer())
			transientBufferCount++;
		bbi.offset = offset;
		bindingNumber++;
	}

	inline bool HasTransientBuffers() {
		return transientBufferCount != 0;
	}

protected:
	struct BindBufferInfo : public AllocGeneral {
		uint32 offset;
		VertexBufferPtr buffer;
		BindBufferInfo() : offset(0) {}
	};

	uint16 transientBufferCount;
	uint16 bindingNumber;
	uint32 numBuffers;
	BindBufferInfo fixedBuffer[NUM_FIXED_BUFFER];
	BindBufferInfo* listOfBuffers;
};

}


#endif // VERTEXBUFFERBINDING_H_
