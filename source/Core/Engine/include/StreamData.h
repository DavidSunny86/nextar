/* 
 * File:   StreamData.h
 * Author: obhi
 *
 * Created on February 7, 2011, 8:39 PM
 */

#ifndef StreamData_H
#define	StreamData_H

#include <RenderConstants.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <VertexLayout.h>

namespace nextar {

// @optimize Most pointers are of few bytes size. Can be taken as object.
// Need better data structures to account for small object pointers here.
class _NexEngineAPI VertexBufferBinding: public AllocGraphics {
public:

	VertexBufferBinding();
	~VertexBufferBinding() {
	}

	inline uint16 GetBindingNumber() const {
		return bindingNumber;
	}

	/** @remarks Returns the count of vertex buffers */
	inline uint32 GetBufferCount() const {
		return (uint32)bufferList.size();
	}

	/** @remarks Returns the buffer stored at a specified index */
	VertexBuffer& GetBuffer(size_t i) {
		return bufferList[i];
	}

	const VertexBuffer* GetBufferPtr(size_t i) const {
		return &bufferList.at(i);
	}

	VertexBuffer* GetBufferPtr(size_t i) {
		return &bufferList.at(i);
	}

	/** @remarks Set buffer count, this must be called before binding
	 * Use VertexLayout::GetBufferCount to find the number of seperate
	 * buffers.
	 **/
	void SetBufferCount(size_t num) {
		bufferList.resize(num);
	}

	/** @remarks Binds the buffer to the specified index */
	void BindBuffer(size_t index, VertexBuffer& vb) {
		if (bufferList[index].IsTransientBuffer())
			--transientBufferCount;
		bufferList[index] = std::move(vb);
		if (bufferList[index].IsTransientBuffer())
			transientBufferCount++;
		bindingNumber++;
	}

	inline bool HasTransientBuffers() {
		return transientBufferCount != 0;
	}

protected:
	typedef vector<VertexBuffer>::type VertexBufferList;
	uint16 transientBufferCount;
	uint16 bindingNumber;
	VertexBufferList bufferList;
};

enum PrimitiveTypeTag {
	/* Point list primitive type.  */
	PT_POINT_LIST,
	/* Line list primitive type.  */
	PT_LINE_LIST,
	/* Triangle list primitive type.  */
	PT_TRI_LIST,
	/* Triangle strip primitive type.  */
	PT_TRI_STRIP
};

typedef uint8 PrimitiveType;

class VertexData: public AllocGraphics {
public:
	uint32 start;
	uint32 count;

	VertexLayout* layout;
	VertexBufferBinding* binding;

	VertexData() :
			layout(0), binding(0), start(0), count(0) {

	}
};

class IndexData {
public:
	uint32 start;
	uint32 count;

	IndexBufferPtr indices;

	IndexData() :
			start(0), count(0) {

	}

	IndexData(size_t indexStart, size_t indexCount, IndexBufferPtr indexBuffer) :
			start(indexStart), count(indexCount), indices(indexBuffer) {
	}
};

/**
 * This class represents stream data for a single pass.
 * A lot of class expecting single pass rendering can
 * simply use an instance of this class rather than
 * the stream pass map (StreamData)
 */
class _NexEngineAPI StreamData: public AllocGeneral {
public:
	enum Flags {
		DELETE_BINDING = 1 << 0,
		DELETE_LAYOUT = 1 << 1,
	};

	uint8 flags;
	/* Indicates if GraphicsStreamData was created */
	/**/
	PrimitiveType type;
	/* instance count is usually 1 */
	uint16 instanceCount;
	/* Vertex data */
	VertexData vertices;
	/* Indices are not used if invalid */
	IndexData indices;

	StreamData();
	~StreamData();
};

}
#endif	/* StreamData_H */

